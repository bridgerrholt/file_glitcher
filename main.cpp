#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <string>

#include <vector>
#include <streambuf>
#include <time.h>
#include <exception>

#include "dependencies/patch/stoi.cpp"
#include "dependencies/patch/stof.cpp"
#include "dependencies/rand_double.cpp"
#include "dependencies/rand_range.cpp"

#include "input_option.h"
#include "glitch_arguments.h"
#include "variable_manager.h"

// Used to mark areas in a file (or array for that matter).
class Range
{
  public:
    std::size_t min;
    std::size_t max;
};

void glitchFile(const GlitchArguments& arguments)
{
	
	std::string inFileName =
		arguments.folder + arguments.inputFileName + arguments.extension;
	std::string outFileName =
		arguments.folder + arguments.outputFileName + arguments.extension;

	// Give information on the current file.
	std::cout <<
		"!"   << outFileName <<
		" -g" << arguments.probability <<
		" -s" << arguments.seed;

	// Set the random seed, either to the passed argument or
	// to the current time.
	srand(arguments.seed);

	// Load the input as a binary file stream.
	std::ifstream inFile(inFileName.c_str(),
		std::ios_base::in | std::ios_base::binary);

	// Push all the binary to a vector.
	std::vector<char> inFileChars;
	char currentChar;
	while (inFile >> std::noskipws >> currentChar) {
		inFileChars.push_back(currentChar);
	}

	// Determines the range of random values for inserting splices,
	// increasing it decreases the amount of glitch.
	std::size_t randMax;

	// Set the rand max based on the file length.
	randMax =
		(1.0 - arguments.probability*0.01)/(arguments.probability*0.1) *
		inFileChars.size();
	if (randMax == 0)
		randMax = 1;


	// An empty vector for the binary output.
	std::vector<char> outFileChars;
	
	// Split the input into ranges containing the entire binary.
	std::vector<Range> ranges;
	// Start off at index 0 (the max will be set later on).
	ranges.push_back({0, 0});

	// Loop through the input file.
	for (std::size_t i = 0; i < inFileChars.size(); ++i) {
		// Randomly decide whether to insert a divisor or not.
		if (randRange(randMax) == 0) {
			// Set the previous range's maximum.
			ranges.back().max = i;
			// Push a new range (the max will be set later on).
			if (i != inFileChars.size()-1) {
				ranges.push_back({i+1, i+1});
			}
		}

		// If it's the last character, set the final max.
		if (i == inFileChars.size()-1)
			ranges.back().max = i;
	}

	// Indexes of unused ranges. Allocate with the exact size required.
	std::vector<std::size_t> remainingRanges(ranges.size());
	// The reorganized ranges.
	std::vector<Range> randomRanges;

	// Store indexes for all the remaining ranges.
	for (std::size_t i = 0; i < ranges.size(); ++i) {
		remainingRanges[i] = i;
	}

	// Randomly select the next range to push (from the
	// ranges not yet chosen).
	for (std::size_t i = 0; i < ranges.size(); ++i) {
		// Randomly select the range index,
		std::size_t index = randRange(remainingRanges.size());
		// push the corresponding range,
		randomRanges.push_back(ranges[remainingRanges[index]]);
		// erase the used index.
		remainingRanges.erase(remainingRanges.begin()+index);
	}

	// Compares the new list to the old list, informs the user if
	// no changes took place.
	bool changesOccured = false;
	// Loop through the randomized list of ranges.
	for (auto i = randomRanges.begin(); i != randomRanges.end(); ++i) {
		// For each range, loop through its marked section within the
		// input binary, pushing each character to the output binary.
		for (auto j = inFileChars.begin()+i->min;
			j != inFileChars.begin()+i->max+1; ++j) {
			outFileChars.push_back(*j);
		}

		// Changes occured if a range is different.
		std::size_t index = i - randomRanges.begin();
		if (i->min != ranges[index].min || i->max != ranges[index].max)
			changesOccured = true;
	}

	if (!changesOccured) {
		std::cout << "\nNote: No changes occured.";
	}

	// Open the ouput file.
	std::ofstream outFile(outFileName.c_str(), std::ios_base::binary);

	// Write the created data.
	for (auto i = 0; i < outFileChars.size(); ++i) {
		outFile << outFileChars[i];
	}

	std::cout << '\n';
}


int main(int argc, char* argv[])
{
  try {
    // Is created once, modified as the user's file is read.
    VariableManager variableManager;

    // Is recreated everytime an output file is declared in
    // the user's file.
    GlitchArguments glitchArguments;

    // Loads the user's file, it determines which files to load and
    // how to process them.
    std::ifstream inDirectories("provided_files.txt");

    // Reads the user's file line by line.
    std::string line;
    while (std::getline(inDirectories, line)) {
      for (auto i = line.begin(); i < line.end(); ++i) {

        // Nothing is done unless the current character is
        // one of the input-starting characters.
        if (*i == '-' || *i== '?' || *i == '!') {

          // Advance if it's a dash, since dashes denote that the
          // next character is the signature; '?' and '!' stand as
          // signatures on their own.
          if (*i == '-')
            ++i;

          // Control over the iterator is taken during the
          // InputOption constructor.
          InputOption inputOption = InputOption(line, i);

          /*
          std::cout <<
          inputOption.signature << " " <<
          inputOption.data << '\n';
          */

          // If the current input option declares an output file,
          // the passed GlitchArguments will be modified and passed to
          // the processing function.
          if (variableManager.takeInput(inputOption, glitchArguments))
            glitchFile(glitchArguments);

        }
      }
    }
  }

  catch (std::exception& e) {
    std::cout << "Failure:\n" << e.what() << '\n';
    return 1;
  }


	/*// Fail if they don't pass an acceptable amount of arguments.
	if (argc < 2 || argc > 3) {
		std::cout <<
			"Incorrect amount of arguments.\n"
			"Expecting \"<glitchness [1, 100]> -seed-\"\n";
		return 0;
	}

	// Determines what to set randMax to. Increasing decreases randMax.
	float probability;
	// Seeds the random value, by default is the time.
	std::size_t seed;


	// Verifies the passed arguments are correct
	try {
		probability = patch::stof(argv[1]);
		if (probability < 1 || probability > 100) {
			throw std::runtime_error("Glitchness must be in [1, 100]");
		}

		// Set the seed to the current time if a different number was
		// not specified.
		if (argc >= 3)
			seed = patch::stoi(argv[2]);
		else
			seed = time(0);
	}
	catch (std::exception& e) {
		std::cout << e.what() << '\n';
		return 1;
	}


	// Loads the file that indicated which image to load and what to
	// save the ouput as.
	std::ifstream inDirectories("provided_files.txt");
	std::vector<std::string> directoryLines;
	std::string line;
	// Get all the lines, it should have sets of 3 lines,
	// with a blank line in between each set.
	while (std::getline(inDirectories, line)) {
		directoryLines.push_back(line);
	}

	try {
		// Each loop deals with a single set.
		for (auto i = directoryLines.begin();
			i != directoryLines.end(); ++i) {

			// The first line is the main directory,
			// uses the current directory if passed a period.
			std::string folder = *i;
			if (folder == ".") {
				folder = "";
			}

			if (++i == directoryLines.end())
				throw std::runtime_error("No inFileName given");
			std::string inFileName = folder + *i;

			if (++i == directoryLines.end())
				throw std::runtime_error("No outFileName given");
			std::string outFileName = folder + *i;

			glitchFile(inFileName, outFileName, probability, seed);
			++seed;

			// Skip the empty line.
			if (++i == directoryLines.end())
				break;

		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << '\n';
		return 1;
	}*/




  return 0;
}