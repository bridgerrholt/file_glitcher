// A simple structure (however with a complex constructor) providing all the
// information necessary for a single user input. Uses public member
// variables because it's such a simple structure.

#ifndef INPUT_OPTION_H
#define INPUT_OPTION_H

#include <string>

class InputOption
{
  public:
    InputOption(
      const std::string& inputString,
      std::string::iterator& current);

    // Indicated what kind of input is following. Does not include the
    // hyphen that's required before non-special inputs.
    // Example: "-g" would store 'g', while "!" would store '!'.
    char signature;

    // Basically the block of text found after an input signature.
    // Example: "-g50" would store "50".
    std::string data;
};

#endif