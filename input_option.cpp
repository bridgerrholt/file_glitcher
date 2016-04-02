#include "input_option.h"

#include <exception>
#include <stdexcept>

InputOption::InputOption(
	const std::string& inputString,
	std::string::iterator& current) :

	data("")
{
	if (current >= inputString.end() || *current == ' ') {
		throw std::runtime_error("Isolated hyphen " + inputString);
	}

	signature = *current;
	++current;

	// Check if the input starts with a quotation.
	bool inQuotes = false;
	char quoteKind;
	if (current != inputString.end() &&
		(*current == '"' || *current == '\'')) {
		inQuotes = true;
		quoteKind = *current;
		++current;
	}

	// Capture the input until the ending quotation or a space.
	while (true) {
		if (current == inputString.end()) {
			if (inQuotes) {
				throw std::runtime_error("Quotated text ended prematurely");
			}
			break;
		}

		if (*current == '\\') {
			++current;
			if (current == inputString.end()) {
				throw std::runtime_error(
					"Backslash with no following character");
			}
			else {
				data += *current;
			}
		}

		else {
			if (inQuotes) {
				if (*current == quoteKind) break;
			}
			else {
				if (*current == ' ') break;
			}

			data += *current;
		}
	
		++current;
	}
}