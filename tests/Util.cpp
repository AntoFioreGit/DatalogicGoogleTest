#include "Util.h"
/*
std::std::string findFileRecursively(const fs::path& directory, const std::std::string& filename) {
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().filename() == filename) {
            return entry.path().std::string();
        } else if (entry.is_directory()) {
            auto result = findFileRecursively(entry.path(), filename);
            if (result.size()) {
                return result;
            }
        }
    }
    return "";
}
*/
std::string findFileRecursively(const std::string &directory, const std::string &filename)
{
    DIR *dir = opendir(directory.c_str());
    if (dir == nullptr)
    {
        return "";
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        std::string path = directory + "/" + entry->d_name;
        struct stat st;
        if (stat(path.c_str(), &st) == 0)
        {
            if (S_ISREG(st.st_mode) && entry->d_name == filename)
            {
                return path;
            }
            else if (S_ISDIR(st.st_mode))
            {
                auto result = findFileRecursively(path, filename);
                if (!result.empty())
                {
                    return result;
                }
            }
        }
    }

    closedir(dir);
    return "";
}
