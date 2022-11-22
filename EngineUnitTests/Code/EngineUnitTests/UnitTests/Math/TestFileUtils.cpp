// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Engine/Core/FileUtils.h"



static std::string filePath = "../../Temporary/TestFileUtilsBasicReadWrite.txt";
static std::string fileContents =
        "Hello, SoBali Sai\r"
        "This is file writing to Disk test\r"
        "Here is every ascii character:\r"
        " \"!#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'";



TEST(FileUtils, BasicReadWrite)
{
    // Create or write the file
    std::vector<uint8_t> fileContentsAsBytes(fileContents.size());
    memcpy(fileContentsAsBytes.data(), fileContents.data(), fileContents.size());
    StringWriteToDisk(filePath, fileContents);

    // Read the file
    FileReadToBuffer(filePath, fileContentsAsBytes);
    std::string fileContentsBackToString;
    fileContentsBackToString.resize(fileContentsAsBytes.size());
    memcpy(fileContentsBackToString.data(), fileContentsAsBytes.data(), fileContentsAsBytes.size());

    // Compare the original string to the written/read string
    EXPECT_STREQ(fileContents.data(), fileContentsBackToString.data());
}



static std::string addTestFilePath = "../../Temporary/TestFileUtilsAddToFileOnDisk.txt";
static std::string stuffToAdd = "Added stuff at end";
static std::string fileContentsAfterAdd =
        "Hello, World\r"
        "This is file writing to Disk test\r"
        "Here is every ascii character:\r"
        " \"!#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'Added stuff at end";



TEST(FileUtils, AddToFileOnDisk)
{
    // Create or write the file
    StringWriteToDisk(addTestFilePath, fileContents);

    // Read the file
    std::string fileReadBackAsString;
    FileReadToString(filePath, fileReadBackAsString);

    // Compare the original string to the written/read string
    EXPECT_STREQ(fileContents.data(), fileReadBackAsString.data());
}