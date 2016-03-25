#include "stoi.h"

int patch::stoi(const std::string& inputString)
{
	std::stringstream stream(inputString);

	int outInt;
	if ((stream >> outInt).fail() || !(stream >> std::ws).eof()) {
		throw std::bad_cast();
	}

	return outInt;
}