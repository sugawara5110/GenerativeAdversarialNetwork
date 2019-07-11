//*****************************************************************************************//
//**                                                                                     **//
//**                   Å@Å@       Discriminator                                          **//
//**                                                                                     **//
//*****************************************************************************************//

#ifndef Class_Discriminator_Header
#define Class_Discriminator_Header

#include "../../CNN/CNN.h"
#include "../../Common/Direct3DWrapper/DxText.h"

class Discriminator {

private:
	CNN* cnn = nullptr;
	UINT inWid;
	UINT inHei;
	UINT numFilter;
	UINT numThread;
	UINT NumColor;

public:
	Discriminator(UINT numWid, UINT numHei, UINT numCol, UINT numFil, UINT inputsetnum);
	~Discriminator();
	void SetLearningLate(float nn, float cn);
	void SetActivationAlpha(float nn, float cn);
	void ForwardPropagation();
	void BackPropagation();
	void BackPropagationNoUpdate();
	float GetcrossEntropyError();
	void TrainingDraw(float x = 0.0f, float y = 0.0f);
	float GetOutputEl(UINT ElNum, UINT inputsetInd = 0);
	void SetTargetEl(float el, UINT ElNum);
	void InputArrayEl(float el, UINT arrNum, UINT ElNum, UINT inputsetInd = 0);
	ID3D12Resource* GetOutErrResource();
	void Draw();
};

#endif
