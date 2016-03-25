#ifndef PATCH_STOF_H
#define PATCH_STOF_H

#include <iostream>
#include <typeinfo>
#include <sstream>
#include <string>

namespace patch
{
	float stof(const std::string& inputString);
}

#endif