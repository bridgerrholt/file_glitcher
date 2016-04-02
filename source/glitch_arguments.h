// Simple data for all the variables the user can set.

#ifndef GLITCH_ARGUMENTS_H
#define GLITCH_ARGUMENTS_H

#include <string>

class GlitchArguments
{
public:
  std::string inputFileName;
  std::string outputFileName;

  // Added before the file name.
  std::string folder;
  
  // Added after the file name.
  std::string extension;

  float probability;
  std::size_t seed;
};

#endif