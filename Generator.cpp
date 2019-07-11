//*****************************************************************************************//
//**                                                                                     **//
//**                   Å@Å@         Generator                                            **//
//**                                                                                     **//
//*****************************************************************************************//

#include "Generator.h"

Generator::Generator(UINT NumInput, UINT outW, UINT outH, UINT numCol, UINT numFil, UINT inputsetnum) {
	NumColor = numCol;
	NumIn = NumInput;
	outw = outW * NumColor;
	outh = outH;
	numFilter = numFil;
	numThread = inputsetnum;
	pixel = new UINT * [outh * numFilter];
	for (UINT i = 0; i < outh * numFilter; i++)pixel[i] = new UINT[outW];

	Layer layer[4];
	UINT layerCnt = 0;

	layer[0].mapWid = NumIn * NumColor;
	layer[0].mapHei = 1;
	layer[0].maxThread = numThread;

	layer[layerCnt].layerName = AFFINE;
	layer[layerCnt].acName = ReLU;
	layer[layerCnt].topAcName = ReLU;
	layer[layerCnt].NumFilter = numFilter;
	layer[layerCnt].numNode[0] = NumIn * 2;
	layer[layerCnt].numNode[1] = outw * outh;
	layer[layerCnt].topNodeWid = outw;
	layer[layerCnt++].NumDepthNotInput = 2;

	layer[layerCnt].layerName = CONV;
	layer[layerCnt].acName = ReLU;
	layer[layerCnt].NumFilter = numFilter;
	layer[layerCnt].NumConvFilterWid = 3;
	layer[layerCnt++].NumConvFilterSlide = 1;

	layer[layerCnt].layerName = CONV;
	layer[layerCnt].acName = ReLU;
	layer[layerCnt].NumFilter = numFilter;
	layer[layerCnt].NumConvFilterWid = 5;
	layer[layerCnt++].NumConvFilterSlide = 1;

	layer[layerCnt].layerName = CONV;
	layer[layerCnt].acName = Sigmoid;
	layer[layerCnt].NumFilter = numFilter;
	layer[layerCnt].NumConvFilterWid = 7;
	layer[layerCnt++].NumConvFilterSlide = 1;

	cnn = new CNN(10, 10, layer, layerCnt);

	dr.SetCommandList(0);
	dr.GetVBarray2D(1);
	dr.TextureInit(outW, outh * numFilter);
	dr.TexOn();
	dr.CreateBox(0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, TRUE, TRUE);
}

Generator::~Generator() {
	for (UINT i = 0; i < outh * numFilter; i++)ARR_DELETE(pixel[i]);
	ARR_DELETE(pixel);
	S_DELETE(cnn);
}

void Generator::SetNoise(VECTOR3* noise) {
	for (UINT k = 0; k < numThread; k++) {
		for (UINT i = 0; i < numFilter; i++) {
			for (UINT i1 = 0; i1 < NumIn; i1++) {
				cnn->InputArrayEl(noise[i1].x, i, i1 * NumColor, k);
				cnn->InputArrayEl(noise[i1].y, i, i1 * NumColor + 1, k);
				cnn->InputArrayEl(noise[i1].z, i, i1 * NumColor + 2, k);
			}
		}
	}
}

void Generator::SetLearningLate(float nn, float cn) {
	cnn->SetLearningLate(nn, cn);
}

void Generator::SetActivationAlpha(float nn, float cn) {
	cnn->SetActivationAlpha(nn, cn);
}

void Generator::ForwardPropagation() {
	cnn->TrainingFp();
}

void Generator::BackPropagation() {
	cnn->TrainingBp();
}

ID3D12Resource* Generator::GetOutput() {
	return cnn->GetOutputResource();
}

float Generator::GetOutputEl(UINT ElNum, UINT inputsetInd) {
	return cnn->GetOutputEl(ElNum, inputsetInd);
}

void Generator::SetInErrResource(ID3D12Resource* res) {
	cnn->SetInErrResource(res);
}

void Generator::DrawOutput() {
	for (UINT h = 0; h < outh * numFilter; h++) {
		for (UINT w = 0; w < outw; w += NumColor) {
			UINT ElIndex = outw * h + w;
			UINT elRed = (UINT)(cnn->GetOutputEl(ElIndex, 0) * 255.0f);
			UINT elGreen = (UINT)(cnn->GetOutputEl(ElIndex + 1, 0) * 255.0f);
			UINT elBlue = (UINT)(cnn->GetOutputEl(ElIndex + 2, 0) * 255.0f);
			UINT pixout = 0;
			pixout = ((elBlue << 16) | (elGreen << 8) | elRed);
			pixel[h][w / NumColor] = pixout;
		}
	}
	DxText::GetInstance()->UpDateText(L"ê∂ê¨âÊëú ", 250.0f, 0.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	dr.Update(250.0f, 20.0f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f, 100.0f, 100.0f * numFilter);
	dr.SetTextureMPixel(pixel, 0xff, 0xff, 0xff, 0xff, 0);
	dr.Draw();

	cnn->TrainingDraw(150.0, 10.0f);
}