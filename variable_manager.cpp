#include "variable_manager.h"

#include <chrono>

#include "dependencies/patch/stoi.h"
#include "dependencies/patch/stof.h"

#include "input_option.h"
#include "glitch_arguments.h"

VariableManager::Variable::Variable() :
	data(""),
	isSet(false)
{

}



VariableManager::VariableManager()
{
	auto currentDuration =
		std::chrono::system_clock::now().time_since_epoch();
	startingSeed_ = std::chrono::duration_cast<
		std::chrono::milliseconds>(currentDuration).count();
	currentSeed_ = startingSeed_;

	variables_['?'] = Variable(); // Input file
	variables_['!'] = Variable(); // Output file
	variables_['f'] = Variable(); // Folder
	variables_['x'] = Variable(); // Extension
	variables_['g'] = Variable(); // Glitchness
	variables_['s'] = Variable(); // Seed
}



bool VariableManager::takeInput(
	const InputOption& inputOption,
	GlitchArguments& outGlitchArguments)
{
	// Try to find the indicated variable.
	auto variable = variables_.find(inputOption.signature);

	// If found, set the variable data.
	if (variable != variables_.end()) {
		variable->second.isSet = true;
		variable->second.data = inputOption.data;
	}

	// Otherwise, fail.
	else {
		std::string exceptionMessage =
			"No signature \"" + inputOption.signature + '"';
		throw std::runtime_error(exceptionMessage);
	}

	if (inputOption.signature == '!') {
		if (!variables_.at('?').isSet) {
			throw std::runtime_error("No input file");
		}

		float probability;
		if (variables_.at('g').isSet) {
			try {
				probability = patch::stof(variables_.at('g').data);
				if (probability < 1 || probability > 100) {
					throw std::runtime_error(
						"Glitchness must be in [1, 100]");
				}
			}
			catch (std::exception& exception) {
				std::string exceptionMessage =
				std::string("Failure to set glitchness: ") +
				exception.what();
				throw std::runtime_error(exceptionMessage);
			}
		}
		else {
			throw std::runtime_error("Glitchness never set");
		}

		std::size_t seed;
		if (variables_.at('s').isSet) {
			try {
				seed = patch::stoi(variables_.at('s').data);
			}
			catch (std::exception& exception) {
				std::string exceptionMessage =
					std::string("Failure to set seed: ") +
					exception.what();
				throw std::runtime_error(exceptionMessage);
			}
		}
		else {
			seed = currentSeed_++;
		}

		outGlitchArguments = {
			variables_.at('?').data,
			variables_.at('!').data,
			variables_.at('f').data,
			variables_.at('x').data,

			probability,
			seed
		};

		return true;
	}

	return false;
}