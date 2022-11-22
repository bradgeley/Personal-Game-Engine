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
    StringWriteToDisk(filePath, fileContents);

    // Read the file
    std::string readFromDisk;
    FileReadToString(filePath, readFromDisk);

    // Compare the original string to the written/read string
    EXPECT_STREQ(fileContents.data(), readFromDisk.data());
}
