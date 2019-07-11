//*****************************************************************************************//
//**                                                                                     **//
//**                   Å@Å@         Generator                                            **//
//**                                                                                     **//
//*****************************************************************************************//

#ifndef Class_Generator_Header
#define Class_Generator_Header

#include "../../CNN/CNN.h"
#include "../../Common/Direct3DWrapper/DxText.h"

class Generator {

private:
	CNN* cnn = nullptr;
	PolygonData2D dr;
	UINT NumIn;
	UINT outw;
	UINT outh;
	UINT numFilter;
	UINT numThread;
	UINT NumColor;
	UINT** pixel = nullptr;

	Generator() {}

public:
	Generator(UINT NumInput, UINT outW, UINT outH, UINT numCol, UINT numFil, UINT inputsetnum);
	~Generator();
	void SetNoise(VECTOR3* noise);
	void SetLearningLate(float nn, float cn);
	void SetActivationAlpha(float nn, float cn);
	void ForwardPropagation();
	void BackPropagation();
	ID3D12Resource* GetOutput();
	float GetOutputEl(UINT ElNum, UINT inputsetInd = 0);
	void SetInErrResource(ID3D12Resource* res);
	void DrawOutput();
};

#endif