//*****************************************************************************************//
//**                                                                                     **//
//**                   Å@Å@GenerativeAdversarialNetworks                                 **//
//**                                                                                     **//
//*****************************************************************************************//

#define _CRT_SECURE_NO_WARNINGS
#include "GenerativeAdversarialNetworks.h"
#define BADGENUM 1

GenerativeAdversarialNetworks::GenerativeAdversarialNetworks(UINT NumNoise, UINT outW, UINT outH) {
	imageWid = outW;
	imageHei = outH;
	NumColor = 3;
	gn = new Generator(NumNoise, outW, outH);
	ng = new NoiseGeneration(NumNoise);
	dis = new Discriminator(imageWid, imageHei, NumColor, BADGENUM);
	path = TRDis1;
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
	//ã≥étÉfÅ[É^ÉVÉÉÉbÉtÉã
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
	case TRDis1://DiscriminatorÇÃê≥âÉfÅ[É^äwèKéû(ã≥étÉfÅ[É^)
	case TRGen://GeneratoräwèKéû(DiscriminatorÇ…ê∂ê¨âÊëúÇê≥ââÊëúÇ∆ÇæÇ‹Ç∑à◊,Target0.99f)
		dis->SetTargetEl(0.99f, 0);
		break;
	case TRDis0://DiscriminatorÇÃïsê≥âÉfÅ[É^äwèKéû(DiscriminatorÇ…ê∂ê¨âÊëúÇÕãUï®Ç∆îªífÇ≥ÇπÇÈà◊)
		dis->SetTargetEl(0.01f, 0);
		break;
	}

	int byteInd = 0;
	for (int k = 0; k < BADGENUM; k++) {

		byteInd = rand() % NumImage;
		UINT imIndSt = byteInd * imageWid * NumColor * imageHei;
		for (UINT i = 0; i < imageWid * imageHei; i++) {
			UINT elRed = 0;
			UINT elGreen = 0;
			UINT elBlue = 0;
			float el = 0.0f;
			float el1 = 0.0f;
			float el2 = 0.0f;
			UINT GetOutputElIndexSt = imageWid * imageHei;
			switch (path) {
			case TRDis1:
				elRed = image[imIndSt + i * NumColor];
				elGreen = image[imIndSt + i * NumColor + 1];
				elBlue = image[imIndSt + i * NumColor + 2];
				pixel[i / imageWid][i % imageWid] = ((elBlue << 16) | (elGreen << 8) | elRed);
				el = ((float)elRed / 255.0f * 0.99f) + 0.01f;
				el1 = ((float)elGreen / 255.0f * 0.99f) + 0.01f;
				el2 = ((float)elBlue / 255.0f * 0.99f) + 0.01f;
				break;
			case TRDis0:
			case TRGen:
				el = gn->GetOutputEl(i, k);
				el1 = gn->GetOutputEl(GetOutputElIndexSt * 1 + i, k);
				el2 = gn->GetOutputEl(GetOutputElIndexSt * 2 + i, k);
				break;
			}
			dis->InputArrayEl(el, 0, i, k);//ê‘
			dis->InputArrayEl(el1, 1, i, k);//óŒ
			dis->InputArrayEl(el2, 2, i, k);//ê¬
		}
	}
}

void GenerativeAdversarialNetworks::TrainingDiscriminator() {
	ng->CreateNoise(10.0f);
	gn->SetNoise(ng->GetNoiseArray());
	gn->SetActivationAlpha(0.1f, 0.1f);
	gn->ForwardPropagation();
	dis->SetLearningLate(0.1f, 0.1f);
	dis->SetActivationAlpha(0.1f, 0.1f);
	LearningByteImage();
	dis->ForwardPropagation();
	dis->BackPropagation();
}

void GenerativeAdversarialNetworks::TrainingGenerator() {
	ng->CreateNoise(10.0f);
	gn->SetNoise(ng->GetNoiseArray());
	gn->SetLearningLate(0.1f, 0.1f);
	gn->SetActivationAlpha(0.1f, 0.1f);
	gn->ForwardPropagation();
	dis->SetLearningLate(0.1f, 0.1f);
	dis->SetActivationAlpha(0.1f, 0.1f);
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
		path = TRDis0;
		outputDis99 = dis->GetOutputEl(0, 0);
		return;
	case TRDis0:
		outputDis01 = dis->GetOutputEl(0, 0);
		path = TRGen;
		return;
	case TRGen:
		outputGen99 = dis->GetOutputEl(0, 0);
		path = TRDis1;
		return;
	}
}

void GenerativeAdversarialNetworks::Draw() {
	DxText::GetInstance()->UpDateText(L"ã≥étâÊëú ", 150.0f, 0.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	gn->DrawOutput();
	dis->Draw();
	dr.Update(150.0f, 20.0f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f, 100.0f, 100.0f);
	dr.SetTextureMPixel(pixel, 0xff, 0xff, 0xff, 0xff, 0);
	dr.Draw();

	DxText::GetInstance()->UpDateText(L"DiscriminatoräwèKã≥étâÊëú ", 300.0f, 10.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateValue((int)(outputDis99 * 100.0f), 610.0f, 10.0f, 15.0f, 2, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateText(L"DiscriminatoräwèKê∂ê¨âÊëú ", 300.0f, 25.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateValue((int)(outputDis01 * 100.0f), 610.0f, 25.0f, 15.0f, 2, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateText(L"Generator    äwèKê∂ê¨âÊëú ", 300.0f, 40.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	DxText::GetInstance()->UpDateValue((int)(outputGen99 * 100.0f), 610.0f, 40.0f, 15.0f, 2, { 1.0f, 1.0f, 1.0f, 1.0f });
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