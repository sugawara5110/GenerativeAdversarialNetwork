//*****************************************************************************************//
//**                                                                                     **//
//**                   Å@Å@     NoiseGeneration                                          **//
//**                                                                                     **//
//*****************************************************************************************//

#include "NoiseGeneration.h"
#include <random>

NoiseGeneration::NoiseGeneration(UINT numInput) {
	NumInput = numInput;
	input = new VECTOR3[NumInput];
}

NoiseGeneration::~NoiseGeneration() {
	ARR_DELETE(input);
}

void NoiseGeneration::CreateNoise(float rate) {
	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());
	std::normal_distribution<> dist(0.0, 2 / sqrt(NumInput) * rate);
	for (UINT i = 0; i < NumInput; i++) {
		input[i].x = (float)dist(engine);
		input[i].y = (float)dist(engine);
		input[i].z = (float)dist(engine);
	}
}

void NoiseGeneration::CreateNoiseRand() {
	srand(time(NULL));
	for (UINT i = 0; i < NumInput; i++) {
		input[i].x = (float)rand() / RAND_MAX;
		input[i].y = (float)rand() / RAND_MAX;
		input[i].z = (float)rand() / RAND_MAX;
	}
}

VECTOR3* NoiseGeneration::GetNoiseArray() {
	return input;
}