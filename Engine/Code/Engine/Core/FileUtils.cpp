#include "FileUtils.h"
#include <fstream>



int FileReadToBuffer(std::string const& filepath, uint8_t* bufferData, size_t bufferSize)
{
    int fileMode = std::ios::in | std::ios::ate | std::ios::binary;
    std::fstream filestream(filepath.data(), fileMode);
    if (filestream.is_open())
    {
        size_t fileSizeBytes = filestream.tellg(); // already at the end from std::ios::ate
        fileSizeBytes = std::min(fileSizeBytes, bufferSize); // don't let it write more than the buffer can take
        filestream.seekg(std::ios::beg);
        filestream.read((char*) bufferData, static_cast<int>(fileSizeBytes));
        filestream.close();
        return static_cast<int>(fileSizeBytes);
    }
    // Error
    return 0;
}



int FileWriteToDisk(std::string const& filepath, uint8_t const* bufferData, size_t bufferSize)
{
    int fileMode = std::ios::out | std::ios::binary;
    std::fstream filestream(filepath.data(), fileMode);
    if (filestream.is_open())
    { 
        filestream.write(reinterpret_cast<char const*>(bufferData), static_cast<int>(bufferSize));
        filestream.close();
        return static_cast<int>(bufferSize);
    }
    // Error
    return 0;
}



int FileReadToBuffer(std::string const& filepath, std::vector<uint8_t>& buffer)
{
    std::fstream filestream;
    int fileMode = std::ios::in | std::ios::ate | std::ios::binary;
    filestream.open(filepath.data(), fileMode);
    if (filestream.is_open())
    {
        size_t fileSizeBytes = filestream.tellg();
        filestream.seekg(std::ios::beg);
        buffer.clear();
        buffer.resize(fileSizeBytes);
        filestream.read((char*) buffer.data(), static_cast<int>(fileSizeBytes));
        filestream.close();
        return static_cast<int>(fileSizeBytes);
    }
    // Error
    return 0;
}



int FileWriteToDisk(std::string const& filepath, std::vector<uint8_t> const& buffer)
{
    int fileMode = std::ios::out | std::ios::binary | std::ios::trunc;
    std::fstream filestream(filepath.data(), fileMode);
    if (filestream.is_open())
    { 
        filestream.write(reinterpret_cast<char const*>(buffer.data()), static_cast<int>(buffer.size()));
        filestream.close();
        return static_cast<int>(buffer.size());
    }
    // Error
    return 0;
}



int FileReadToString(std::string const& filepath, std::string& string)
{
    std::vector<uint8_t> buffer;
    int bytesRead = FileReadToBuffer(filepath, buffer);
    string = std::string(buffer.begin(), buffer.end());
    return bytesRead;
}



int StringWriteToDisk(std::string const& filepath, std::string const& string)
{
    int fileMode = std::ios::out | std::ios::binary | std::ios::trunc;
    std::fstream filestream(filepath.data(), fileMode);
    if (filestream.is_open())
    { 
        filestream.write(string.data(), static_cast<int>(string.size()));
        filestream.close();
        return static_cast<int>(string.size());
    }
    // Error
    return 0;
}