//*****************************************************************************************//
//**                                                                                     **//
//**                   @@       Discriminator                                          **//
//**                                                                                     **//
//*****************************************************************************************//

#include "Discriminator.h"

Discriminator::Discriminator(UINT numWid, UINT numHei, UINT numFil, UINT inputsetnum) {
	inWid = numWid;
	inHei = numHei;
	numFilter = numFil;
	numThread = inputsetnum;

	Layer layer[7];
	UINT layerCnt = 0;

	layer[0].mapWid = inWid;
	layer[0].mapHei = inHei;
	layer[0].maxThread = numThread;
	layer[layerCnt].layerName = CONV;
	layer[layerCnt].acName = ReLU;
	layer[layerCnt].NumFilter = numFilter;
	layer[layerCnt].NumConvFilterWid = 7;
	layer[layerCnt++].NumConvFilterSlide = 1;

	layer[layerCnt].layerName = POOL;
	layer[layerCnt++].NumFilter = numFilter;

	layer[layerCnt].layerName = CONV;
	layer[layerCnt].acName = ReLU;
	layer[layerCnt].NumFilter = numFilter * 2;
	layer[layerCnt].NumConvFilterWid = 5;
	layer[layerCnt++].NumConvFilterSlide = 1;

	layer[layerCnt].layerName = POOL;
	layer[layerCnt++].NumFilter = numFilter * 2;

	layer[layerCnt].layerName = CONV;
	layer[layerCnt].acName = ReLU;
	layer[layerCnt].NumFilter = numFilter * 4;
	layer[layerCnt].NumConvFilterWid = 3;
	layer[layerCnt++].NumConvFilterSlide = 1;

	layer[layerCnt].layerName = POOL;
	layer[layerCnt++].NumFilter = numFilter * 4;

	layer[layerCnt].layerName = AFFINE;
	layer[layerCnt].acName = ReLU;
	layer[layerCnt].topAcName = CrossEntropySigmoid;
	layer[layerCnt].NumFilter = numFilter * 4;
	layer[layerCnt].numNode[0] = 64;
	layer[layerCnt].numNode[1] = 1;
	layer[layerCnt++].NumDepthNotInput = 2;

	cnn = new CNN(10, 10, layer, layerCnt);
}

Discriminator::~Discriminator() {
	S_DELETE(cnn);
}

void Discriminator::SetLearningLate(float nn, float cn) {
	cnn->SetLearningLate(nn, cn);
}

void Discriminator::SetActivationAlpha(float nn, float cn) {
	cnn->SetActivationAlpha(nn, cn);
}

void Discriminator::ForwardPropagation() {
	cnn->TrainingFp();
}

void Discriminator::BackPropagation() {
	cnn->TrainingBp();
}

void Discriminator::BackPropagationNoUpdate() {
	cnn->TrainingBpNoUpdate();
}

float Discriminator::GetcrossEntropyError() {
	return cnn->GetcrossEntropyError();
}

void Discriminator::TrainingDraw(float x, float y) {
	cnn->TrainingDraw(x, y);
}

float Discriminator::GetOutputEl(UINT ElNum, UINT inputsetInd) {
	return cnn->GetOutputEl(ElNum, inputsetInd);
}

void Discriminator::SetTargetEl(float el, UINT ElNum) {
	cnn->SetTargetEl(el, ElNum);
}

void Discriminator::InputArrayEl(float el, UINT arrNum, UINT ElNum, UINT inputsetInd) {
	cnn->InputArrayEl(el, arrNum, ElNum, inputsetInd);
}

ID3D12Resource* Discriminator::GetOutErrResource() {
	return cnn->GetOutErrResource();
}

void Discriminator::Draw() {
	cnn->TrainingDraw(150.0f, 120.0f);
}