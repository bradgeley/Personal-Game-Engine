// Bradley Christensen - 2022-2026
#define _CRT_SECURE_NO_WARNINGS
#include "FileUtils.h"
#include "StringUtils.h"
#include "ErrorUtils.h"
#include <cstdio>
#include <filesystem>



//----------------------------------------------------------------------------------------------------------------------
int FileUtils::FileWriteFromBuffer(const std::string& filepath, const uint8_t* bufferData, size_t bufferSize, bool makeDirs)
{
    ASSERT_OR_DIE(!filepath.empty(), "Trying to write to empty filepath");
    ASSERT_OR_DIE(bufferSize > 0, "Trying to write a file of size 0");
    ASSERT_OR_DIE(bufferData, "Trying to write from a null buffer");

    if (makeDirs)
    {
        FileMakeDirsInPath(filepath);
    }

    FILE* file = fopen(filepath.c_str(), "wb");
    if (file)
    {
        size_t bytesWritten = fwrite(bufferData, 1, bufferSize, file);
        fclose(file);
        return static_cast<int>(bytesWritten);
    }
    // Error
    return 0;
}



//----------------------------------------------------------------------------------------------------------------------
int FileUtils::FileWriteFromBuffer(const std::string& filepath, const std::vector<uint8_t>& buffer, bool makeDirs)
{
    return FileWriteFromBuffer(filepath, buffer.data(), buffer.size(), makeDirs);
}



//----------------------------------------------------------------------------------------------------------------------
int FileUtils::FileWriteFromString(const std::string& filepath, const std::string& string, bool makeDirs)
{
    return FileWriteFromBuffer(filepath, reinterpret_cast<const uint8_t*>(string.data()), string.size(), makeDirs);
}



//----------------------------------------------------------------------------------------------------------------------
int FileUtils::FileReadToBuffer(const std::string& filepath, uint8_t* bufferData, size_t bufferSize)
{
    FILE* file = fopen(filepath.c_str(), "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        size_t fileSizeBytes = static_cast<size_t>(ftell(file));
        fileSizeBytes = std::min(fileSizeBytes, bufferSize); // don't let it write more than the buffer can take
        fseek(file, 0, SEEK_SET);
        size_t bytesRead = fread(bufferData, 1, fileSizeBytes, file);
        fclose(file);
        return static_cast<int>(bytesRead);
    }
    // Error
    return 0;
}



//----------------------------------------------------------------------------------------------------------------------
int FileUtils::FileReadToBuffer(const std::string& filepath, std::vector<uint8_t>& buffer)
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
int FileUtils::FileReadToString(const std::string& filepath, std::string& string)
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
int FileUtils::GetFileSize(const std::string& filepath)
{
    FILE* file = fopen(filepath.c_str(), "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        int fileSize = static_cast<int>(ftell(file));
        fclose(file);
        return fileSize;
    }
    return 0;
}



//----------------------------------------------------------------------------------------------------------------------
bool FileUtils::FileMakeDirsInPath(const std::string& filepath)
{
    Strings pathSections = StringUtils::SplitStringOnDelimiter(filepath, '.');
    if (pathSections.size() == 2)
    {
        // Has an extension, lop it off and the last string 
        Strings pathSectionsNoFileOrExt = StringUtils::SplitStringOnAnyDelimiter(pathSections[0], "/\\");
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
