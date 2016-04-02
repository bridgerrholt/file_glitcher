#ifndef INPUT_OPTION_H
#define INPUT_OPTION_H

#include <string>

class InputOption
{
public:
	InputOption(
		const std::string& inputString,
		std::string::iterator& current);

	char signature;
	std::string data;
};

#endif