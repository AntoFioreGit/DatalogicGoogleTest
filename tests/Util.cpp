#include "Util.h"
#include <sstream>
#include <iomanip>

#include <string>
#include <algorithm>
#include "json.h"
#include "file_json.h"
#include "CommonTest.h"
#include <iostream>
#include <fstream>
#include <glog/logging.h>
std::string getDirectoryPath(const std::string &filePath)
{

    //  Unix-like e '\\'  Windows
    size_t lastSeparator = filePath.find_last_of("/\\");
    if (lastSeparator == std::string::npos)
    {
        return filePath;
    }

    return filePath.substr(0, lastSeparator + 1);
}
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
bool removeDirectory(std::string path)
{
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr)
    {
        LOG(ERROR) << "Error opening directory: " << path;

        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        std::string fullPath = path + "/" + entry->d_name;
        struct stat statbuf;
        if (stat(fullPath.c_str(), &statbuf) == 0)
        {
            if (S_ISDIR(statbuf.st_mode))
            {
                if (!removeDirectory(fullPath.c_str()))
                {
                    closedir(dir);
                    return false;
                }
            }
            else
            {
                if (remove(fullPath.c_str()) != 0)
                {
                    closedir(dir);
                    return false;
                }
            }
        }
        else
        {
            LOG(ERROR) << "Error accessing file/directory: " << fullPath;
            closedir(dir);
            return false;
        }
    }

    closedir(dir);
    if (rmdir(path.c_str()) != 0)
    {
        LOG(ERROR) << "Error removing directory: " << path;
        return false;
    }

    return true;
}
std::string getTimeStamp()
{
    std::stringstream timeStamp;
    time_t const now_c = time(NULL);
    timeStamp << std::put_time(localtime(&now_c), "%Y%m%d_%H%M%S");
    return timeStamp.str();
}
std::string generateTempConf(std::string &confFile, std::set<Keyonfig> keys)
{
    std::string newFileGenerate;

    nlohmann::json json_value;
    bool result = rs::io::readJsonFile(confFile, json_value);
    if (!result)
    {
        // LOG(ERROR) << "Error on read file : " << confFile;
        return newFileGenerate;
    }
    std::string absolutePath = getDirectoryPath(confFile);
    std::string nameTmpConf = std::string("Tmp_") + nameAlgoConf;
    newFileGenerate = absolutePath + "/" + nameTmpConf;
    std::string homeDir = std::getenv("HOME");
    for (auto key : keys)
    {
        if (key == ALGO)
        {
            json_value["algo"][0]["config_file"] = absolutePath + "/" + nameprofile3DCalcConf;
        }
        else if (key == OUTDIR)
        {
            std::string currentOutDir = json_value["save"]["out_dir"];
            std::string newDir = homeDir + "/" + currentOutDir;
            json_value["save"]["out_dir"] = newDir;
        } else if (key == ENABLE_SAVE_AB)
        {           
            json_value["save"]["ab"] = true;
        } else if (key == DISABLE_SAVE_AB)
        {           
            json_value["save"]["ab"] = false;
        } else if (key == DISABLE_SAVE_XYZ)
        {           
            json_value["save"]["xyz"] = false;
        } else if (key == ENABLE_SAVE_XYZ)
        {           
            json_value["save"]["xyz"] = true;
        }
    }
    std::ofstream o(newFileGenerate);
    o << std::setw(4) << json_value << std::endl;
    o.close();

       return newFileGenerate;
}
