// Bradley Christensen - 2022-2023
#include "FileUtils.h"
#include "StringUtils.h"
#include "ErrorUtils.h"
#include <fstream>
#include <filesystem>



//----------------------------------------------------------------------------------------------------------------------
int FileWriteFromBuffer(const std::string& filepath, const uint8_t* bufferData, size_t bufferSize, bool makeDirs)
{
    ASSERT_OR_DIE(!filepath.empty(), "Trying to write to empty filepath");
    ASSERT_OR_DIE(bufferSize > 0, "Trying to write a file of size 0");
    ASSERT_OR_DIE(bufferData, "Trying to write from a null buffer");

    if (makeDirs)
    {
        FileMakeDirsInPath(filepath);
    }

    int fileMode = std::ios::out | std::ios::binary;
    std::fstream filestream(filepath.data(), fileMode);
    if (filestream.is_open())
    {
        filestream.write(reinterpret_cast<const char*>(bufferData), static_cast<int>(bufferSize));
        filestream.close();
        return static_cast<int>(bufferSize);
    }
    // Error
    return 0;
}



//----------------------------------------------------------------------------------------------------------------------
int FileWriteFromBuffer(const std::string& filepath, const std::vector<uint8_t>& buffer, bool makeDirs)
{
    return FileWriteFromBuffer(filepath, buffer.data(), buffer.size(), makeDirs);
}



//----------------------------------------------------------------------------------------------------------------------
int FileWriteFromString(const std::string& filepath, const std::string& string, bool makeDirs)
{
    return FileWriteFromBuffer(filepath, reinterpret_cast<const uint8_t*>(string.data()), string.size(), makeDirs);
}



//----------------------------------------------------------------------------------------------------------------------
int FileReadToBuffer(const std::string& filepath, uint8_t* bufferData, size_t bufferSize)
{
    int fileMode = std::ios::in | std::ios::ate | std::ios::binary;
    std::fstream filestream(filepath.data(), fileMode);
    if (filestream.is_open())
    {
        size_t fileSizeBytes = filestream.tellg(); // already at the end from std::ios::ate
        fileSizeBytes = std::min(fileSizeBytes, bufferSize); // don't let it write more than the buffer can take
        filestream.seekg(std::ios::beg);
        filestream.read((char*)bufferData, static_cast<int>(fileSizeBytes));
        filestream.close();
        return static_cast<int>(fileSizeBytes);
    }
    // Error
    return 0;
}



//----------------------------------------------------------------------------------------------------------------------
int FileReadToBuffer(const std::string& filepath, std::vector<uint8_t>& buffer)
{
    int fileSize = GetFileSize(filepath);
    if (fileSize > 0)
    {
        buffer.clear();
        buffer.resize(fileSize);
        FileReadToBuffer(filepath, buffer.data(), fileSize);
    }
    return fileSize;
}



//----------------------------------------------------------------------------------------------------------------------
int FileReadToString(const std::string& filepath, std::string& string)
{
    int fileSize = GetFileSize(filepath);
    if (fileSize > 0)
    {
        string.clear();
        string.resize(fileSize);
        auto stringDataAsBytes = reinterpret_cast<uint8_t*>(string.data());
        FileReadToBuffer(filepath, stringDataAsBytes, fileSize);
    }
    return fileSize;
}



//----------------------------------------------------------------------------------------------------------------------
int GetFileSize(const std::string& filepath)
{
    int fileMode = std::ios::in | std::ios::ate | std::ios::binary;
    std::fstream filestream(filepath.data(), fileMode);
    if (filestream.is_open())
    {
        return static_cast<int>(filestream.tellg());
    }
    return 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool FileMakeDirsInPath(const std::string& filepath)
{
    Strings pathSections = SplitStringOnDelimeter(filepath, '.');
    if (pathSections.size() == 2)
    {
        // Has an extension, lop it off and the last string 
        Strings pathSectionsNoFileOrExt = SplitStringOnAnyDelimeter(pathSections[0], "/\\");
        if (pathSectionsNoFileOrExt.size() >= 2)
        {
            pathSectionsNoFileOrExt.erase(pathSectionsNoFileOrExt.begin() + pathSectionsNoFileOrExt.size() - 1);
            pathSections = pathSectionsNoFileOrExt;
        }
    }

    bool madeAtLeastOnePath = false;
    std::string currentPath;
    for (auto& pathSection : pathSections)
    {
        currentPath.append(pathSection);
        currentPath.push_back('/');
        bool success = std::filesystem::create_directory(currentPath);
        madeAtLeastOnePath = success ? true : false;
    }
    return madeAtLeastOnePath;
}
