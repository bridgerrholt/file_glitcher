#ifndef VARIABLE_MANAGER_H
#define VARIABLE_MANAGER_H

#include <string>
#include <map>

class InputOption;
class GlitchArguments;

class VariableManager
{
public:
	VariableManager();

	class Variable
	{
	public:
		Variable();

		std::string data;
		bool isSet;
	};

	// Returns true if a glitch operation should be performed.
	bool takeInput(
		const InputOption& inputOption,
		GlitchArguments& outGlitchArguments);

private:
	std::map<char, Variable> variables_;

	std::size_t startingSeed_;
	std::size_t currentSeed_;

};

#endif