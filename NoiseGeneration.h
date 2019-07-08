//*****************************************************************************************//
//**                                                                                     **//
//**                   Å@Å@     NoiseGeneration                                          **//
//**                                                                                     **//
//*****************************************************************************************//

#ifndef Class_NoiseGeneration_Header
#define Class_NoiseGeneration_Header

#include "../../CNN/CNN.h"

class NoiseGeneration {

private:
	UINT NumInput;
	VECTOR3* input = nullptr;

public:
	NoiseGeneration(UINT numInput);
	~NoiseGeneration();
	void CreateNoise(float rate);
	VECTOR3* GetNoiseArray();
};

#endif
