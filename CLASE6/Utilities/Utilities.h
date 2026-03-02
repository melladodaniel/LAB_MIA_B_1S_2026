#pragma once
#include <fstream>
#include <string>

namespace Utilities {

bool CreateFile(const std::string& name);
std::fstream OpenFile(const std::string& name);

template<typename T>
bool WriteObject(std::fstream& file, const T& data, std::streampos pos);

template<typename T>
bool ReadObject(std::fstream& file, T& data, std::streampos pos);

}