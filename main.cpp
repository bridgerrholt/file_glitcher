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

// Used to mark areas in a file (or array for that matter).
class Range
{
public:
	std::size_t min;
	std::size_t max;
};



int main(int argc, char* argv[])
{
	// Loads the file that indicated which image to load and what to
	// save the ouput as.
	std::ifstream inDirectories("provided_files.txt");
	std::vector<std::string> directoryLines;
	std::string line;
	// Get all the lines, there should be 3.
	while (std::getline(inDirectories, line)) {
		directoryLines.push_back(line);
	}

	// Fail if they don't pass an acceptable amount of arguments.
	if (argc < 2 || argc > 3) {
		std::cout <<
			"Incorrect amount of arguments.\n"
			"Expecting \"<glitchness ([1, 100])> -seed-\"\n";
		return 0;
	}

	// The first line is the main directory, uses the current directory if
	// passed a period.
	std::string folder = directoryLines.at(0);
	if (folder == ".") {
		folder = "";
	}

	std::string inFileName = folder + directoryLines.at(1);
	std::string outFileName = folder + directoryLines.at(2);

	// Determines what to set randMax to. Increasing decreases randMax.
	float probability;
	// Seeds the random value, by default is the time.
	std::size_t seed;

	// Determines the range of random values for inserting splices,
	// increasing it decreases the amount of glitch.
	std::size_t randMax;

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

	// Set the random seed, either to the passed argument or
	// to the current time.
	srand(seed);

	// Load the input as a binary file stream.
	std::ifstream inFile(inFileName.c_str(),
		std::ios_base::in | std::ios_base::binary);

	// Push all the binary to a vector.
	std::vector<char> inFileChars;
	char currentChar;
	while (inFile >> std::noskipws >> currentChar) {
		inFileChars.push_back(currentChar);
	}

	// Set the rand max based on the file length.
	randMax = (1.0 - probability*0.01)/(probability*0.1) * inFileChars.size();
	if (randMax == 0)
		randMax = 1;
	std::cout << "randMax is " << randMax << '\n';

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
		std::cout << "Note: No changes occured.\n";
	}

	// Open the ouput file.
	std::ofstream outFile(outFileName.c_str(), std::ios_base::binary);

	// Write the created data.
	for (auto i = 0; i < outFileChars.size(); ++i) {
		outFile << outFileChars[i];
	}

	return 0;
}