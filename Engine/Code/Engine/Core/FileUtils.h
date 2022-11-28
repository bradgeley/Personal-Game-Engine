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

// Write files to disk
int FileWriteFromBuffer(const std::string& filepath, const uint8_t* bufferData, size_t bufferSize);
int FileWriteFromBuffer(const std::string& filepath, const std::vector<uint8_t>& buffer);
int FileWriteFromString(const std::string& filepath, const std::string& string);

// Read files from disk
int FileReadToBuffer(const std::string& filepath, uint8_t* bufferData, size_t bufferSize);
int FileReadToBuffer(const std::string& filepath, std::vector<uint8_t>& buffer);
int FileReadToString(const std::string& filepath, std::string& string);

// Utility functions
int GetFileSize(const std::string& filepath);
