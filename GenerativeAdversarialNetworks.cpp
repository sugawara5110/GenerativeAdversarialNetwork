//*****************************************************************************************//
//**                                                                                     **//
//**                   　　GenerativeAdversarialNetworks                                 **//
//**                                                                                     **//
//*****************************************************************************************//

#define _CRT_SECURE_NO_WARNINGS
#include "GenerativeAdversarialNetworks.h"
#define BADGENUM 16

GenerativeAdversarialNetworks::GenerativeAdversarialNetworks(UINT NumNoise, UINT outW, UINT outH, UINT numCol) {
	NumColor = numCol;
	imageWid = outW;
	imageHei = outH;
	NumFil = 10;
	gn = new Generator(NumNoise, imageWid, imageHei, NumColor, NumFil, BADGENUM);
	ng = new NoiseGeneration(NumNoise);
	dis = new Discriminator(imageWid, imageHei, NumColor, NumFil, BADGENUM);
	pixel = new UINT * [outH];
	for (UINT i = 0; i < outH; i++)pixel[i] = new UINT[outW];

	dr.SetCommandList(0);
	dr.GetVBarray2D(1);
	dr.TextureInit(outW, outH);
	dr.TexOn();
	dr.CreateBox(0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, TRUE, TRUE);
}

GenerativeAdversarialNetworks::~GenerativeAdversarialNetworks() {
	S_DELETE(gn);
	S_DELETE(ng);
	S_DELETE(dis);
	ARR_DELETE(image);
	for (UINT i = 0; i < imageHei; i++)ARR_DELETE(pixel[i]);
	ARR_DELETE(pixel);
}

void GenerativeAdversarialNetworks::CreateLearningImagebyte(UINT NumPPM, BYTE* ppm) {
	NumImage = NumPPM;
	image = new BYTE[imageWid * NumColor * imageHei * NumPPM * sizeof(BYTE)];
	memcpy(image, ppm, imageWid * NumColor * imageHei * NumPPM * sizeof(BYTE));
	//教師データシャッフル
	BYTE* tmp = new BYTE[imageWid * NumColor * imageHei];
	size_t tmpSize = sizeof(BYTE) * imageWid * NumColor * imageHei;
	for (UINT i = 0; i < NumPPM; i++) {
		int ind1 = (rand() % NumPPM) * imageWid * NumColor * imageHei;
		int ind2 = (rand() % NumPPM) * imageWid * NumColor * imageHei;
		memcpy(tmp, &image[ind1], tmpSize);
		memcpy(&image[ind1], &image[ind2], tmpSize);
		memcpy(&image[ind2], tmp, tmpSize);
	}
}

void GenerativeAdversarialNetworks::LearningByteImage() {
	switch (path) {
	case TRDis1://Discriminatorの正解データ学習時(教師データ)
	case TRGen://Generator学習時(Discriminatorに生成画像を正解画像とだます為,Target0.99f)
		dis->SetTargetEl(0.99f, 0);
		break;
	case TRDis0://Discriminatorの不正解データ学習時(Discriminatorに生成画像は偽物と判断させる為)
		dis->SetTargetEl(0.01f, 0);
		break;
	}

	int byteInd = 0;
	for (int k = 0; k < BADGENUM; k++) {

		byteInd = rand() % NumImage;
		UINT imIndSt = byteInd * imageWid * NumColor * imageHei;
		for (UINT i = 0; i < imageWid * NumColor * imageHei; i += NumColor) {
			UINT GetOutputElIndexSt = imageWid * NumColor * imageHei;
			UINT elRed = 0;
			UINT elGreen = 0;
			UINT elBlue = 0;
			float el = 0.0f;
			float el1 = 0.0f;
			float el2 = 0.0f;
			for (int i1 = 0; i1 < NumFil; i1++) {
				switch (path) {
				case TRDis1:
					elRed = image[imIndSt + i];
					elGreen = image[imIndSt + i + 1];
					elBlue = image[imIndSt + i + 2];
					pixel[i / NumColor / imageWid][i / NumColor % imageWid] = ((elBlue << 16) | (elGreen << 8) | elRed);
					el = ((float)elRed / 255.0f * 0.99f) - 0.5f;
					el1 = ((float)elGreen / 255.0f * 0.99f) - 0.5f;
					el2 = ((float)elBlue / 255.0f * 0.99f) - 0.5f;
					break;
				case TRDis0:
				case TRGen:
					el = gn->GetOutputEl(GetOutputElIndexSt * i1 + i, k);
					el1 = gn->GetOutputEl(GetOutputElIndexSt * i1 + i + 1, k);
					el2 = gn->GetOutputEl(GetOutputElIndexSt * i1 + i + 2, k);
					break;
				}
				dis->InputArrayEl(el, i1, i, k);//赤
				dis->InputArrayEl(el1, i1, i + 1, k);//緑
				dis->InputArrayEl(el2, i1, i + 2, k);//青
			}
		}
	}
}

void GenerativeAdversarialNetworks::TrainingDiscriminator() {
	ng->CreateNoise(1.0f);
	gn->SetNoise(ng->GetNoiseArray());
	gn->SetActivationAlpha(0.01f, 0.01f);
	gn->ForwardPropagation();
	dis->SetLearningLate(0.05f, 0.05f);
	dis->SetActivationAlpha(0.2f, 0.2f);
	LearningByteImage();
	dis->ForwardPropagation();
	dis->BackPropagation();
}

void GenerativeAdversarialNetworks::TrainingGenerator() {
	ng->CreateNoise(1.0f);
	gn->SetNoise(ng->GetNoiseArray());
	gn->SetLearningLate(0.05f, 0.05f);
	gn->SetActivationAlpha(0.2f, 0.2f);
	gn->ForwardPropagation();
	dis->SetLearningLate(0.05f, 0.05f);
	dis->SetActivationAlpha(0.2f, 0.2f);
	LearningByteImage();
	dis->ForwardPropagation();
	dis->BackPropagationNoUpdate();
	gn->SetInErrResource(dis->GetOutErrResource());
	gn->BackPropagation();
}

void GenerativeAdversarialNetworks::Com() {
	switch (path) {
	case TRDis1:
	case TRDis0:
		TrainingDiscriminator();
		break;
	case TRGen:
		TrainingGenerator();
		break;
	}

	switch (path) {
	case TRDis1:
		outputDis99 = dis->GetOutputEl(0, 0);
		break;
	case TRDis0:
		outputDis01 = dis->GetOutputEl(0, 0);
		break;
	case TRGen:
		outputGen99 = dis->GetOutputEl(0, 0);
		break;
	}

	if (pathF) {
		switch (path) {
		case TRDis1:
			path = TRDis0;
			break;
		case TRDis0:
			path = TRDis1;
			break;
		}
	}

	pathCnt++;
	allCnt++;
	if (pathF && pathCnt > 1) {
		pathF = false;
		pathCnt = 0;
		path = TRGen;
	}
	if (!pathF && pathCnt > 2) {
		pathF = true;
		pathCnt = 0;
		path = TRDis1;
	}
}

void GenerativeAdversarialNetworks::Draw() {
	DxText::GetInstance()->UpDateText(L"教師画像 ", 10.0f, 0.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	gn->DrawOutput();
	dis->Draw();
	dr.Update(10.0f, 20.0f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f, 100.0f, 100.0f);
	dr.SetTextureMPixel(pixel, 0xff, 0xff, 0xff, 0xff, 0);
	dr.Draw();

	DxText::GetInstance()->UpDateText(L"Discriminator学習教師画像 ", 400.0f, 10.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateValue((int)(outputDis99 * 100.0f), 710.0f, 10.0f, 15.0f, 2, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateText(L"Discriminator学習生成画像 ", 400.0f, 25.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateValue((int)(outputDis01 * 100.0f), 710.0f, 25.0f, 15.0f, 2, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateText(L"Generator    学習生成画像 ", 400.0f, 40.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateValue((int)(outputGen99 * 100.0f), 710.0f, 40.0f, 15.0f, 2, { 1.0f, 1.0f, 1.0f, 1.0f });

	if (pathF) {
		DxText::GetInstance()->UpDateText(L"Discriminator学習中 1回  ", 400.0f, 60.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	}
	else {
		DxText::GetInstance()->UpDateText(L"Generator    学習中 2回  ", 400.0f, 60.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	}
	DxText::GetInstance()->UpDateValue((int)pathCnt, 400.0f, 75.0f, 15.0f, 2, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateText(L"エポック  ", 400.0f, 90.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateValue((int)allCnt / NumImage, 400.0f, 105.0f, 15.0f, 2, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateText(L"カウント  ", 400.0f, 120.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateValue((int)allCnt, 400.0f, 135.0f, 15.0f, 2, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void GenerativeAdversarialNetworks::SaveDataSet() {
	FILE* fp = nullptr;
	size_t size = NumImage * imageWid * NumColor * imageHei * sizeof(BYTE);

	fp = fopen("datasetByte/dataset.da", "wb");
	fwrite(image, size, 1, fp);
	fclose(fp);

	FILE* fp2 = nullptr;
	fp2 = fopen("datasetByte/datasetnum.da", "wb");
	UINT numImage[1];
	numImage[0] = NumImage;
	fwrite(numImage, sizeof(UINT), 1, fp2);
	fclose(fp2);
}

bool GenerativeAdversarialNetworks::LoadDataSet() {
	FILE* fp = nullptr;
	FILE* fp2 = nullptr;

	fp = fopen("datasetByte/dataset.da", "rb");
	fp2 = fopen("datasetByte/datasetnum.da", "rb");
	if (!fp || !fp2) return false;

	UINT numImage[1];
	fread(numImage, sizeof(UINT), 1, fp2);
	NumImage = numImage[0];
	fclose(fp2);

	image = new BYTE[NumImage * imageWid * NumColor * imageHei];
	fread(image, NumImage * imageWid * NumColor * imageHei * sizeof(BYTE), 1, fp);
	fclose(fp);
	return true;
}