//*****************************************************************************************//
//**                                                                                     **//
//**                   Å@Å@GenerativeAdversarialNetworks                                 **//
//**                                                                                     **//
//*****************************************************************************************//

#ifndef Class_GenerativeAdversarialNetworks_Header
#define Class_GenerativeAdversarialNetworks_Header

#include "NoiseGeneration.h"
#include "Generator.h"
#include "Discriminator.h"

enum Path {
	TRDis1,
	TRDis0,
	TRGen
};

class GenerativeAdversarialNetworks {

private:
	Generator* gn = nullptr;
	NoiseGeneration* ng = nullptr;
	Discriminator* dis = nullptr;
	UINT imageWid;
	UINT imageHei;
	UINT NumFil;
	UINT NumColor;
	BYTE* image = nullptr;
	UINT NumImage;
	Path path = TRDis1;
	PolygonData2D dr;
	UINT** pixel = nullptr;
	UINT pathCnt = 0;
	UINT allCnt = 0;
	bool pathF = true;
	float outputDis99 = 0.0f;
	float outputDis01 = 0.0f;
	float outputGen99 = 0.0f;

	GenerativeAdversarialNetworks() {}
	void LearningByteImage();
	void TrainingDiscriminator();
	void TrainingGenerator();

public:
	GenerativeAdversarialNetworks(UINT NumNoise, UINT outW, UINT outH, UINT numCol);
	~GenerativeAdversarialNetworks();
	void CreateLearningImagebyte(UINT NumPPM, BYTE* ppm);
	void Com();
	void Draw();
	void SaveDataSet();
	bool LoadDataSet();
};

#endif

