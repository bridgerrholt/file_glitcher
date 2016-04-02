#include "variable_manager.h"

#include <chrono>

#include <patch/stoi.h>
#include <patch/stof.h>

#include "input_option.h"
#include "glitch_arguments.h"

VariableManager::Variable::Variable() :
  data(""),
  isSet(false)
{

}



VariableManager::VariableManager()
{
  // Set the starting seed with the current milliseconds.
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

  // Fail; indicated variable not found.
  else {
    std::string exceptionMessage =
      "No variable of signature \"" + inputOption.signature + '"';
    throw std::runtime_error(exceptionMessage);
  }

  // If it's the "glitchness" variable.
  if (inputOption.signature == 'g') {
    try {
      float probability = patch::stof(inputOption.data);
      if (probability < 1 || probability > 100)
        throw std::runtime_error("Glitchness must be in [1, 100]");
    }

    // Fail; not a correct number.
    catch (std::exception& exception) {
      std::string exceptionMessage =
        std::string("Failure to set glitchness: ") + exception.what();
      throw std::runtime_error(exceptionMessage);
    }
  }

  // If it's the "seed" variable.
  else if (inputOption.signature == 's') {
    try {
      currentSeed_ = patch::stoi(variables_.at('s').data);
    }

    // Fail; not an integer.
    catch (std::exception& exception) {
      std::string exceptionMessage =
        std::string("Failure to set seed: ") + exception.what();
      throw std::runtime_error(exceptionMessage);
    }
  }

  // If it's the output file declaraction.
  else if (inputOption.signature == '!') {
    // Fail; an input file must be given.
    if (!variables_.at('?').isSet) {
      throw std::runtime_error("No input file");
    }

    // Fail; "glitchness" must be set by the user.
    if (!variables_.at('g').isSet) {
      throw std::runtime_error("Glitchness never set");
    }

    float probability = patch::stof(variables_.at('g').data);

    std::size_t seed = currentSeed_++;

    outGlitchArguments = {
      variables_.at('?').data,
      variables_.at('!').data,
      variables_.at('f').data,
      variables_.at('x').data,

      probability,
      seed
    };

    // The glitch arguments have been set, commence processing.
    return true;
  }

  return false;
}