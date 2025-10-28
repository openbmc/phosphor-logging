// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: Copyright 2021 IBM Corporation

#include "extensions/openpower-pels/temporary_file.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

#include <gtest/gtest.h>

using namespace openpower::pels::util;
namespace fs = std::filesystem;

/**
 * Modify the specified file so that fs::remove() can successfully delete it.
 *
 * Undo the modifications from an earlier call to makeFileUnRemovable().
 *
 * @param path path to the file
 */
inline void makeFileRemovable(const fs::path& path)
{
    // makeFileUnRemovable() creates a directory at the file path.  Remove the
    // directory and all of its contents.
    fs::remove_all(path);

    // Rename the file back to the original path to restore its contents
    fs::path savePath{path.native() + ".save"};
    fs::rename(savePath, path);
}

/**
 * Modify the specified file so that fs::remove() fails with an exception.
 *
 * The file will be renamed and can be restored by calling makeFileRemovable().
 *
 * @param path path to the file
 */
inline void makeFileUnRemovable(const fs::path& path)
{
    // Rename the file to save its contents
    fs::path savePath{path.native() + ".save"};
    fs::rename(path, savePath);

    // Create a directory at the original file path
    fs::create_directory(path);

    // Create a file within the directory.  fs::remove() will throw an exception
    // if the path is a non-empty directory.
    std::ofstream childFile{path / "childFile"};
}

class TemporaryFileTests : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Create temporary file with some data
        std::string buf{"FFDCDATA"};
        uint32_t size = buf.size();
        tmpFile = new TemporaryFile(buf.c_str(), size);

        // Create temporary file with no data
        std::string noData{""};
        tmpFileNoData = new TemporaryFile(noData.c_str(), 0);
    }

    void TearDown() override
    {
        std::filesystem::remove_all(tmpFile->getPath());
        delete tmpFile;

        std::filesystem::remove_all(tmpFileNoData->getPath());
        delete tmpFileNoData;
    }

    // temporary file with Data
    TemporaryFile* tmpFile;

    // temporary file with no data
    TemporaryFile* tmpFileNoData;
};

TEST_F(TemporaryFileTests, DefaultConstructor)
{
    fs::path path = tmpFile->getPath();
    EXPECT_FALSE(path.empty());
    EXPECT_TRUE(fs::exists(path));
    EXPECT_TRUE(fs::is_regular_file(path));

    fs::path parentDir = path.parent_path();
    EXPECT_EQ(parentDir, "/tmp");

    std::string fileName = path.filename();
    std::string namePrefix = "phosphor-logging-";
    EXPECT_EQ(fileName.compare(0, namePrefix.size(), namePrefix), 0);
}

TEST_F(TemporaryFileTests, DefaultConstructorNoData)
{
    fs::path path = tmpFileNoData->getPath();
    EXPECT_FALSE(path.empty());
    EXPECT_TRUE(fs::exists(path));
    EXPECT_TRUE(fs::is_regular_file(path));

    fs::path parentDir = path.parent_path();
    EXPECT_EQ(parentDir, "/tmp");

    std::string fileName = path.filename();
    std::string namePrefix = "phosphor-logging-";
    EXPECT_EQ(fileName.compare(0, namePrefix.size(), namePrefix), 0);
}

TEST_F(TemporaryFileTests, MoveConstructor)
{
    // verify temporary file exists
    EXPECT_FALSE(tmpFile->getPath().empty());
    EXPECT_TRUE(fs::exists(tmpFile->getPath()));

    // Save path to temporary file
    fs::path path = tmpFile->getPath();

    // Create second TemporaryFile object by moving first object
    TemporaryFile file{std::move(*tmpFile)};

    // Verify first object now has an empty path
    EXPECT_TRUE(tmpFile->getPath().empty());

    // Verify second object now owns same temporary file and file exists
    EXPECT_EQ(file.getPath(), path);
    EXPECT_TRUE(fs::exists(file.getPath()));

    // Delete file
    std::filesystem::remove_all(file.getPath());
}

TEST_F(TemporaryFileTests, MoveAssignmentOperatorTest1)
{
    // Test where works: TemporaryFile object is moved
    // verify temporary file exists
    EXPECT_FALSE(tmpFile->getPath().empty());
    EXPECT_TRUE(fs::exists(tmpFile->getPath()));

    // Save path to first temporary file
    fs::path path1 = tmpFile->getPath();

    // Verify second temporary file exists
    EXPECT_FALSE(tmpFileNoData->getPath().empty());
    EXPECT_TRUE(fs::exists(tmpFileNoData->getPath()));

    // Save path to second temporary file
    fs::path path2 = tmpFileNoData->getPath();

    // Verify temporary files are different
    EXPECT_NE(path1, path2);

    // Move first object into the second
    *tmpFileNoData = std::move(*tmpFile);

    // Verify first object now has an empty path
    EXPECT_TRUE(tmpFile->getPath().empty());

    // Verify second object now owns first temporary file and file exists
    EXPECT_EQ(tmpFileNoData->getPath(), path1);
    EXPECT_TRUE(fs::exists(path1));

    // Verify second temporary file was deleted
    EXPECT_FALSE(fs::exists(path2));
}

TEST_F(TemporaryFileTests, MoveAssignmentOperatorTest2)
{
    // Test where does nothing: TemporaryFile object is moved into itself
    // Verify temporary file exists
    EXPECT_FALSE(tmpFile->getPath().empty());
    EXPECT_TRUE(fs::exists(tmpFile->getPath()));

    // Save path to temporary file
    fs::path path = tmpFile->getPath();

    // Try to move object into itself; should do nothing
    *tmpFile = static_cast<TemporaryFile&&>(*tmpFile);

    // Verify object still owns same temporary file and file exists
    EXPECT_EQ(tmpFile->getPath(), path);
    EXPECT_TRUE(fs::exists(path));
}

TEST_F(TemporaryFileTests, MoveAssignmentOperatorTest3)
{
    // Test where fails: Cannot delete temporary file
    // Verify temporary file exists
    EXPECT_FALSE(tmpFile->getPath().empty());
    EXPECT_TRUE(fs::exists(tmpFile->getPath()));

    // Save path to first temporary file
    fs::path path1 = tmpFile->getPath();

    // Verify temporary file exists
    EXPECT_FALSE(tmpFileNoData->getPath().empty());
    EXPECT_TRUE(fs::exists(tmpFile->getPath()));

    // Save path to second temporary file
    fs::path path2 = tmpFileNoData->getPath();

    // Verify temporary files are different
    EXPECT_NE(path1, path2);

    // Make second temporary file unremoveable
    makeFileUnRemovable(path2);

    try
    {
        // Try to move first object into the second; should throw exception
        *tmpFileNoData = std::move(*tmpFile);
        ADD_FAILURE() << "Should not have reached this line.";
    }
    catch (const std::exception& e)
    {
        // This is expected.  Exception message will vary.
    }

    // Verify first object has not changed and first temporary file exists
    EXPECT_EQ(tmpFile->getPath(), path1);
    EXPECT_TRUE(fs::exists(path1));

    // Verify second object has not changed and second temporary file exists
    EXPECT_EQ(tmpFileNoData->getPath(), path2);
    EXPECT_TRUE(fs::exists(path2));

    // Make second temporary file removeable so destructor can delete it
    makeFileRemovable(path2);
}

TEST_F(TemporaryFileTests, Destructor)
{
    // Test where works: Temporary file is not deleted
    {
        fs::path path{};
        {
            TemporaryFile file("", 0);
            path = file.getPath();
            EXPECT_TRUE(fs::exists(path));
        }
        EXPECT_TRUE(fs::exists(path));
        fs::remove(path);
    }

    // Test where works: Temporary file was already deleted
    {
        fs::path path{};
        {
            TemporaryFile file("", 0);
            path = file.getPath();
            EXPECT_TRUE(fs::exists(path));
            file.remove();
            EXPECT_FALSE(fs::exists(path));
        }
        EXPECT_FALSE(fs::exists(path));
    }

    // Test where fails: Cannot delete temporary file: No exception thrown
    {
        fs::path path{};
        try
        {
            TemporaryFile file("", 0);
            path = file.getPath();
            EXPECT_TRUE(fs::exists(path));
            makeFileUnRemovable(path);
        }
        catch (...)
        {
            ADD_FAILURE() << "Should not have caught exception.";
        }

        // Temporary file should still exist
        EXPECT_TRUE(fs::exists(path));

        // Make file removable and delete it
        makeFileRemovable(path);
        fs::remove(path);
    }
}

TEST_F(TemporaryFileTests, RemoveTest1)
{
    // Test where works
    // Vverify temporary file exists
    EXPECT_FALSE(tmpFile->getPath().empty());
    EXPECT_TRUE(fs::exists(tmpFile->getPath()));

    // Save path to temporary file
    fs::path path = tmpFile->getPath();

    // Delete temporary file
    tmpFile->remove();

    // Verify path is cleared and file does not exist
    EXPECT_TRUE(tmpFile->getPath().empty());
    EXPECT_FALSE(fs::exists(path));

    // Delete temporary file again; should do nothing
    tmpFile->remove();
    EXPECT_TRUE(tmpFile->getPath().empty());
    EXPECT_FALSE(fs::exists(path));
}

TEST_F(TemporaryFileTests, RemoveTest2)
{
    // Test where fails
    // Create TemporaryFile object and verify temporary file exists
    EXPECT_FALSE(tmpFile->getPath().empty());
    EXPECT_TRUE(fs::exists(tmpFile->getPath()));

    // Make file unremovable
    makeFileUnRemovable(tmpFile->getPath());

    try
    {
        // Try to delete temporary file; should fail with exception
        tmpFile->remove();
        ADD_FAILURE() << "Should not have reached this line.";
    }
    catch (const std::exception& e)
    {
        // This is expected.  Exception message will vary.
    }

    // Make file removable again so it will be deleted by the destructor
    makeFileRemovable(tmpFile->getPath());
}

TEST_F(TemporaryFileTests, GetPath)
{
    EXPECT_FALSE(tmpFile->getPath().empty());
    EXPECT_EQ(tmpFile->getPath().parent_path(), "/tmp");
    EXPECT_TRUE(fs::exists(tmpFile->getPath()));
}
