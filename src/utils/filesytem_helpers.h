#pragma once

#include <functional>
#include <string>
#include <vector>

#include "json.h"

namespace rs 
{
    namespace utils
    {
        namespace filesystem
        {
            std::string GetFileExtensionInLowerCase(const std::string& filename);
            std::string GetFileNameWithoutExtension(const std::string& filename);
            std::string GetFileNameWithoutDirectory(const std::string& filename);

            std::string GetRegularizedDirectoryName(const std::string& directory);
            std::string GetWorkingDirectory();

            bool FileExists(const std::string &filename);
            bool DirectoryExists(const std::string& directory);
            bool DirectoryIsEmpty(const std::string& directory);

            bool MakeDirectory(const std::string& directory);            
            bool ListDirectory(const std::string &directory,
                               std::vector<std::string>& subdirs,
                               std::vector<std::string>& filenames,
                               bool full_path);
            bool ListFilesInDirectory(const std::string& directory,
                                      std::vector<std::string>& filenames,
                                      bool full_path = false);
            bool ListFilesInDirectoryWithExtension(const std::string& directory,
                                                   const std::string& extname,
                                                   std::vector<std::string>& filenames,
                                                   bool full_path = false);

            std::vector<std::string> FindFilesRecursively(
                const std::string &directory,
                std::function<bool(const std::string &)> is_match);

            // wrapper for fopen that enables unicode paths on Windows
            //FILE *FOpen(const std::string &filename, const std::string &mode);
            //std::string GetIOErrorString(const int errnoVal);
            //bool FReadToBuffer(const std::string &path,
            //                   std::vector<char> &bytes,
            //                   std::string *errorStr);
            bool ReadFileToBuffer(const std::string& path,
                                  std::vector<char>& buffer);

            std::string JoinPath(const std::vector<std::string>& path_components);
            std::string JoinPath(const std::string& path_component1,
                                 const std::string& path_component2);
        } // namespace filesystem
    } // namespace utils
}  // namespace rs