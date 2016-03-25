#include "rand_double.h"
#include "rand_range.h"

int randRange(int min, int max)
{

	return min + randRange(max-min);

	/*return min + static_cast<int>(std::rand())/(static_cast<float>(RAND_MAX/(max-min))); 

	float returnVal = min + ((max * std::rand()) / (RAND_MAX + 1.0));
	std::cout << returnVal << '\n';
	return returnVal;*/
}



unsigned int randRange(unsigned int max)
{
	//std::cout << RAND_MAX << '\n';
	//return static_cast<int>(std::rand())/(static_cast<float>(RAND_MAX/max));
	return randDouble()*max;
}