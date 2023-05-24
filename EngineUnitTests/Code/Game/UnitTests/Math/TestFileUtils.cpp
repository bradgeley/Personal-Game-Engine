// Bradley Christensen - 2022
#include "gtest/gtest.h"
#include "Engine/Core/FileUtils.h"



//----------------------------------------------------------------------------------------------------------------------
static std::string filePath = "../../Temporary/TestFileUtilsBasicReadWrite.txt";
static std::string fileContents =
    "Hello, SoBali Sai\r"
    "This is file writing to Disk test\r"
    "Here is every ascii character:\r"
    " \"!#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHI\0JKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'";



//----------------------------------------------------------------------------------------------------------------------
TEST(FileUtils, StringReadWrite)
{
    // Create or write the file
    FileWriteFromString(filePath, fileContents);

    // Read the file
    std::string readFromDisk;
    FileReadToString(filePath, readFromDisk);

    // Compare the original string to the written/read string
    EXPECT_EQ(fileContents.size(), readFromDisk.size());
    EXPECT_STREQ(fileContents.data(), readFromDisk.data());
}



//----------------------------------------------------------------------------------------------------------------------
TEST(FileUtils, ByteVectorReadWrite)
{
    // Create or write the file
    std::vector<uint8_t> buffer(fileContents.begin(), fileContents.end());
    FileWriteFromBuffer(filePath, buffer);

    // Read the file
    std::vector<uint8_t> readFromDisk;
    FileReadToBuffer(filePath, readFromDisk);

    // Compare the original string to the written/read string
    EXPECT_EQ(fileContents.size(), readFromDisk.size());
    if (fileContents.size() == readFromDisk.size())
    {
        for (int i = 0; i < static_cast<int>(readFromDisk.size()); ++i)
        {
            EXPECT_EQ(fileContents[i], readFromDisk[i]);
        }
    }
}
