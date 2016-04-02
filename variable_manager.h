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

    // Returns true if a glitch operation should be performed.
    bool takeInput(
      const InputOption& inputOption,
      GlitchArguments& outGlitchArguments);

  private:
    // Very simple structure for holding information about a
    // specific variable. All Variable objects should be constructed in
    // VariableManager's constructor.
    class Variable
    {
      public:
        Variable();

        std::string data;
        bool isSet;
    };

    // The key is the signature used for the specific variable.
    std::map<char, Variable> variables_;

    // The seed derived from the construction.
    std::size_t startingSeed_;
    // The useable seed, incremented after every use.
    std::size_t currentSeed_;

};

#endif