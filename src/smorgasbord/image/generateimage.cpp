#include "generateimage.hpp"

#include "image.hpp"

#include <random>

void Smorgasbord::GenerateWhiteNoise(Smorgasbord::Image &image, int seed)
{
	std::default_random_engine randomGenerator(seed);
	std::uniform_int_distribution<uint32_t> randomDistribution(0, 255);
	
	for (size_t i = 0; i < image.dataSize; i++)
	{
		image.data[i] = randomDistribution(randomGenerator);
	}
}
