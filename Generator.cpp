//*****************************************************************************************//
//**                                                                                     **//
//**                   Å@Å@         Generator                                            **//
//**                                                                                     **//
//*****************************************************************************************//

#include "Generator.h"

Generator::Generator(UINT NumInput, UINT outW, UINT outH) {
	NumIn = NumInput;
	outw = outW;
	outh = outH;
	numFilter = 3;
	numThread = 1;
	pixel = new UINT * [outh];
	for (UINT i = 0; i < outh; i++)pixel[i] = new UINT[outw];

	Layer layer[4];
	UINT layerCnt = 0;

	layer[0].mapWid = layer[0].mapHei = (UINT)sqrt(NumIn);
	layer[0].maxThread = numThread;

	layer[layerCnt].layerName = AFFINE;
	layer[layerCnt].acName = ReLU;
	layer[layerCnt].topAcName = ReLU;
	layer[layerCnt].NumFilter = numFilter;
	layer[layerCnt].numNode[0] = NumIn * 2;
	layer[layerCnt].numNode[1] = outW * outH;
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
	layer[layerCnt].acName = ReLU;
	layer[layerCnt].NumFilter = numFilter;
	layer[layerCnt].NumConvFilterWid = 7;
	layer[layerCnt++].NumConvFilterSlide = 1;

	cnn = new CNN(10, 10, layer, layerCnt);

	dr.SetCommandList(0);
	dr.GetVBarray2D(1);
	dr.TextureInit(outw, outh);
	dr.TexOn();
	dr.CreateBox(0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, TRUE, TRUE);
}

Generator::~Generator() {
	for (UINT i = 0; i < outh; i++)ARR_DELETE(pixel[i]);
	ARR_DELETE(pixel);
	S_DELETE(cnn);
}

void Generator::SetNoise(VECTOR3* noise) {
	for (UINT i = 0; i < NumIn; i++) {
		cnn->InputArrayEl(noise->x, 0, i, 0);
		cnn->InputArrayEl(noise->y, 1, i, 0);
		cnn->InputArrayEl(noise->z, 2, i, 0);
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
	for (UINT h = 0; h < outh; h++) {
		for (UINT w = 0; w < outw; w++) {
			UINT oneFilNumEl = outh * outw;
			UINT ElIndex = outw * h + w;
			UINT elRed = (UINT)(cnn->GetOutputEl(ElIndex, 0) * 255.0f);
			UINT elGreen = (UINT)(cnn->GetOutputEl(ElIndex + oneFilNumEl, 0) * 255.0f);
			UINT elBlue = (UINT)(cnn->GetOutputEl(ElIndex + oneFilNumEl * 2, 0) * 255.0f);
			UINT pixout = 0;
			pixout = ((elRed << 16) | (elGreen << 8) | elBlue);
			pixel[h][w] = pixout;
		}
	}
	DxText::GetInstance()->UpDateText(L"ê∂ê¨âÊëú ",10.0f, 0.0f, 15.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
	dr.Update(10.0f, 20.0f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f, 100.0f, 100.0f);
	dr.SetTextureMPixel(pixel, 0xff, 0xff, 0xff, 0xff, 0);
	dr.Draw();

	cnn->TrainingDraw(10.0, 120.0f);
}