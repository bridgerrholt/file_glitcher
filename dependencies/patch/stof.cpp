#include "stof.h"

float patch::stof(const std::string& inputString)
{
	std::stringstream stream(inputString);

	float outFloat;
	if ((stream >> outFloat).fail() || !(stream >> std::ws).eof()) {
		throw std::bad_cast();
	}

	return outFloat;
}