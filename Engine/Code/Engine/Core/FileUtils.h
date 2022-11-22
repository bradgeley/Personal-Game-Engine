// Bradley Christensen - 2022
#pragma once
#include <cstdint>
#include <string>
#include <vector>



//----------------------------------------------------------------------------------------------------------------------
// FileUtils
//
// Common functions for reading and writing to files
// All functions return the number of bytes read/written
//
int FileReadToBuffer(std::string const& filepath, uint8_t* bufferData, size_t bufferSize);
int FileWriteToDisk(std::string const& filepath, uint8_t const* bufferData, size_t bufferSize);

int FileReadToBuffer(std::string const& filepath, std::vector<uint8_t>& buffer);
int FileWriteToDisk(std::string const& filepath, std::vector<uint8_t> const& buffer);

int FileReadToString(std::string const& filepath, std::string& string);
int StringWriteToDisk(std::string const& filepath, std::string const& string);

