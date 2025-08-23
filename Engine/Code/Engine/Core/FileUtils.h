// Bradley Christensen - 2022-2023
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



//----------------------------------------------------------------------------------------------------------------------
// Write files to disk
//
int FileWriteFromBuffer(const std::string& filepath, const uint8_t* bufferData, size_t bufferSize, bool makeDirs = true);
int FileWriteFromBuffer(const std::string& filepath, const std::vector<uint8_t>& buffer, bool makeDirs = true);
int FileWriteFromString(const std::string& filepath, const std::string& string, bool makeDirs = true);



//----------------------------------------------------------------------------------------------------------------------
// Read files from disk
//
int FileReadToBuffer(const std::string& filepath, uint8_t* bufferData, size_t bufferSize);
int FileReadToBuffer(const std::string& filepath, std::vector<uint8_t>& buffer);
int FileReadToString(const std::string& filepath, std::string& string);



//----------------------------------------------------------------------------------------------------------------------
// Utility functions
//
int GetFileSize(const std::string& filepath);
bool FileMakeDirsInPath(const std::string& filepath);
