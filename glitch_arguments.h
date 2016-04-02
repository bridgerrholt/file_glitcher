#ifndef GLITCH_ARGUMENTS_H
#define GLITCH_ARGUMENTS_H

#include <string>

class GlitchArguments
{
public:
	std::string inputFileName;
	std::string outputFileName;
	std::string folder;
	std::string extension;

	float probability;
	std::size_t seed;

private:

};

#endif