
#include "filesytem_helpers.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#ifdef WIN32
#include <windows.h>
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <glog/logging.h>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace rs
{
    namespace utils
    {
        namespace filesystem
        {
            std::string GetPathSeparator()
            {
#ifdef _WIN32
                return "\\";
#else
                return "/";
#endif
            }

            std::string GetFileExtensionInLowerCase(const std::string &filename)
            {
                size_t dot_pos = filename.find_last_of(".");
                if (dot_pos >= filename.length())
                    return "";

                if (filename.find_first_of("/\\", dot_pos) != std::string::npos)
                    return "";

                std::string filename_ext = filename.substr(dot_pos + 1);

                std::transform(filename_ext.begin(), filename_ext.end(),
                               filename_ext.begin(), ::tolower);

                return filename_ext;
            }

            std::string GetFileNameWithoutExtension(const std::string& filename)
            {
                size_t dot_pos = filename.find_last_of(".");

                return filename.substr(0, dot_pos);
            }

            std::string GetFileNameWithoutDirectory(const std::string& filename)
            {
                size_t slash_pos = filename.find_last_of("/\\");
                if (slash_pos == std::string::npos)
                {
                    return filename;
                }
                else
                {
                    return filename.substr(slash_pos + 1);
                }
            }

            std::string GetRegularizedDirectoryName(const std::string& directory)
            {
                if (directory.empty())
                {
                    return "/";
                }
                else if (directory.back() != '/' && directory.back() != '\\')
                {
                    return directory + GetPathSeparator();
                }
                else
                {
                    return directory;
                }
            }

            std::string GetWorkingDirectory()
            {
                fs::path full_path = fs::current_path();
                return full_path.string();
            }

            bool FileExists(const std::string &filename)
            {
                return fs::exists(filename) && fs::is_regular_file(filename);
            }

            bool DirectoryExists(const std::string& directory)
            {
                return fs::is_directory(directory);
            }

            bool DirectoryIsEmpty(const std::string& directory)
            {
                if (!DirectoryExists(directory))
                {
                    LOG(ERROR) << "Directory " << directory << " does not exist.";
                }
                return fs::is_empty(directory);
            }

            bool MakeDirectory(const std::string& directory)
            {
                if (fs::create_directory(directory))
                {
                    fs::perms perms = fs::perms::owner_read | 
                                      fs::perms::owner_write | 
                                      fs::perms::owner_exec;
                    fs::permissions(directory, perms);
                    return true;
                }
                return false;
            }

            bool ListDirectory(const std::string& directory,
                               std::vector<std::string>& subdirs,
                               std::vector<std::string>& filenames,
                               bool full_path)
            {
                filenames.clear();

                if (DirectoryIsEmpty(directory))
                {
                    return false;
                }

                // If path does not end with "\\" or "/" then we need to add it
                std::string path = GetRegularizedDirectoryName(directory);
#ifdef WIN32
                WIN32_FIND_DATA file_info;
                HANDLE hFind = FindFirstFile((path + "*").c_str(), &file_info);

                if (hFind != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        std::string file_name = file_info.cFileName;
                        size_t dot_pos = file_name.find_last_of('.');
                        if (dot_pos < file_name.length() - 1)
                            filenames.push_back(full_path ? path + file_name : file_name);
                    } while (FindNextFile(hFind, &file_info));
                }

                FindClose(hFind);
#else
                DIR* dir = opendir(directory.c_str());
                if (!dir)
                {
                    return false;
                }

                struct stat st;
                struct dirent* entry = readdir(dir);
                while ((entry = readdir(dir)) != NULL)
                {
                    const std::string file_name = entry->d_name;
                    if (file_name[0] == '.')
                        continue;

                    std::string full_file_name = path + file_name;
                    if (stat(full_file_name.c_str(), &st) == -1)
                        continue;
                    if (S_ISDIR(st.st_mode))
                        subdirs.push_back(full_file_name);
                    else if (S_ISREG(st.st_mode))
                        filenames.push_back(full_path ? full_file_name : file_name);
                }

                closedir(dir);
#endif

                // Natural sort, useful if part of name contains numbers
                const std::string numbers = "0123456789";
                std::sort(filenames.begin(), filenames.end(),
                          [&numbers](const std::string& a, const std::string& b)
                          {
                              size_t a_start = a.find_first_of(numbers);
                              size_t a_end = a.find_last_of(numbers);
                              std::string cmp_a(a.begin() + a_start, a.begin() + a_end + 1);
                              size_t b_start = b.find_first_of(numbers);
                              size_t b_end = b.find_last_of(numbers);
                              std::string cmp_b(b.begin() + b_start, b.begin() + b_end + 1);
                              return stoi(cmp_a) < stoi(cmp_b);
                          });

                return true;
            }

            bool ListFilesInDirectory(const std::string &directory,
                                      std::vector<std::string> &filenames, 
                                      bool full_path)
            {
                std::vector<std::string> subdirs;
                return ListDirectory(directory, subdirs, filenames, full_path);
            }

            bool ListFilesInDirectoryWithExtension(const std::string& directory,
                                                   const std::string& extname,
                                                   std::vector<std::string>& filenames,
                                                   bool full_path)
            {
                filenames.clear();

                std::vector<std::string> file_list;
                if (!ListFilesInDirectory(directory, file_list, full_path))
                {
                    return false;
                }

                std::string ext_in_lower = extname;
                std::transform(ext_in_lower.begin(), ext_in_lower.end(),
                               ext_in_lower.begin(), ::tolower);

                for (const auto &fn : file_list)
                {
                    if (GetFileExtensionInLowerCase(fn) == ext_in_lower)
                    {
                        filenames.push_back(fn);
                    }
                }

                return true;
            }

            std::string GetIOErrorString(const int errnoVal)
            {
                switch (errnoVal)
                {
                case EPERM:
                    return "Operation not permitted";
                case EACCES:
                    return "Access denied";
                // Error below could be EWOULDBLOCK on Linux
                case EAGAIN:
                    return "Resource unavailable, try again";
#ifndef WIN32
                case EDQUOT:
                    return "Over quota";
#endif
                case EEXIST:
                    return "File already exists";
                case EFAULT:
                    return "Bad filename pointer";
                case EINTR:
                    return "open() interrupted by a signal";
                case EIO:
                    return "I/O error";
                case ELOOP:
                    return "Too many symlinks, could be a loop";
                case EMFILE:
                    return "Process is out of file descriptors";
                case ENAMETOOLONG:
                    return "Filename is too long";
                case ENFILE:
                    return "File system table is full";
                case ENOENT:
                    return "No such file or directory";
                case ENOSPC:
                    return "No space available to create file";
                case ENOTDIR:
                    return "Bad path";
                case EOVERFLOW:
                    return "File is too big";
                case EROFS:
                    return "Can't modify file on read-only filesystem";
#if EWOULDBLOCK != EAGAIN
                case EWOULDBLOCK:
                    return "Operation would block calling process";
#endif
                default:
                {
                    std::stringstream s;
                    s << "IO error " << errnoVal << " (see sys/errno.h)";
                    return s.str();
                }
                }
            }

            bool ReadFileToBuffer(const std::string& path,
                                  std::vector<char>& buffer)
            {
                std::ifstream file_stream(path, std::ios::in | std::ios::binary);
                if (!file_stream.is_open()) // check for invalid input
                {
                    LOG(ERROR) << "Could not open or find file " << path;
                    return false;
                }

                std::string content((std::istreambuf_iterator<char>(file_stream)), (std::istreambuf_iterator<char>()));
                size_t filesize = content.size();
                buffer.resize(filesize);
                memcpy(buffer.data(), content.data(), content.size());
                file_stream.close();

                return true;
            }

            std::string JoinPath(const std::string& path_component1,
                                const std::string& path_component2)
            {
                fs::path path(path_component1);
                return (path / path_component2).string();
            }

            std::string JoinPath(const std::vector<std::string>& path_components)
            {
                fs::path path;
                for (const auto &pc : path_components)
                {
                    path /= pc;
                }
                return path.string();
            }
        } // namespace filesystem
    } // namespace utils
} // namespace rs