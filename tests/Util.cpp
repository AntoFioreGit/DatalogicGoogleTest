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
std::string getDirectoryPath(const std::string& filePath) {
 
    //  Unix-like e '\\'  Windows
    size_t lastSeparator = filePath.find_last_of("/\\");
    if (lastSeparator == std::string::npos) {
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
std::string getTimeStamp()
{
    std::stringstream timeStamp;
    time_t const now_c = time(NULL);
    timeStamp << std::put_time(localtime(&now_c), "%Y%m%d_%H%M%S");
    return timeStamp.str();
}
std::string generateTempConf1(std::string &confFile)
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
     json_value["algo"][0]["config_file"] = absolutePath + "/" + nameprofile3DCalcConf;
     std::ofstream o(newFileGenerate);
     o << std::setw(4) << json_value << std::endl;
     o.close();
     return newFileGenerate;
}