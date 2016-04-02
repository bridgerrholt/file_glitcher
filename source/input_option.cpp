#include "input_option.h"

#include <exception>
#include <stdexcept>

InputOption::InputOption(
  const std::string& inputString,
  std::string::iterator& current) :

  data("")
{
  // Fail if there is no signature character. This is expected to
  // only happen when a hyphen doesn't have a signature character after it.
  if (current >= inputString.end() || *current == ' ')
    throw std::runtime_error("Hyphen with no signature");

  // The current character
  signature = *current;
  ++current;

  // Check if the input starts with a quotation.
  bool inQuotes = false;
  char quoteKind;
  // No error is thrown if no text is actually inputted.
  if (current != inputString.end() &&
    (*current == '"' || *current == '\'')) {
    inQuotes = true;
    quoteKind = *current;

    // Advance to the text in between the quotations.
    ++current;
  }

  // Capture the input until either an ending quotation or a space.
  while (true) {
    if (current == inputString.end()) {
      // Fail if an ending quotation was never found.
      if (inQuotes)
        throw std::runtime_error("Quotated text ended prematurely");

      // Further guarantees no infinite loops.
      break;
    }

    // A backslash indicates a special character.
    if (*current == '\\') {
      ++current;

      if (current == inputString.end())
        throw std::runtime_error("Backslash with no character after");
      else
        // Simply add what comes after the backslash; this way
        // backslashes/spaces/quotations can be represented.
        data += *current;
    }

    else {
      // Break if found the ending quotation,
      if (inQuotes) {
        if (*current == quoteKind) break;
      }

      // or the ending space.
      else {
        if (*current == ' ') break;
      }

      // Add the character since it proves to be simply text.
      data += *current;
    }

    ++current;
  }
}