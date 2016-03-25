#include <cstdlib>

#define RS_SCALE (1.0 / (1.0 + RAND_MAX))

double randDouble()
{
	double value;
	do {
		value = (((std::rand()*RS_SCALE) + std::rand())*RS_SCALE +
			std::rand()) * RS_SCALE;
	} while (value >= 1); // Round off.

	return value;
}