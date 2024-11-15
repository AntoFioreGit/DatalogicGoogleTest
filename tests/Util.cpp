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
#include "filesytem_helpers.h"
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
    std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
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
        }
        else if (key == ENABLE_SAVE_AB)
        {
            json_value["save"]["ab"] = true;
        }
        else if (key == DISABLE_SAVE_AB)
        {
            json_value["save"]["ab"] = false;
        }
        else if (key == DISABLE_SAVE_XYZ)
        {
            json_value["save"]["xyz"] = false;
        }
        else if (key == ENABLE_SAVE_XYZ)
        {
            json_value["save"]["xyz"] = true;
        }
    }
    std::ofstream o(newFileGenerate);
    o << std::setw(4) << json_value << std::endl;
    o.close();

    return newFileGenerate;
}
std::string generateTempConf(std::string &confFile, std::map<Keyonfig,std::string> keys){
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
    std::string homeDir = std::getenv("WORKSPACE") != nullptr ? std::getenv("WORKSPACE") : std::getenv("HOME");
    for (auto key : keys)
    {
        if (key.first == ALGO)
        {
            json_value["algo"][0]["config_file"] = absolutePath + "/" + nameprofile3DCalcConf;
        } else if (key.first == OUTDIR)
        {
            std::string currentOutDir = json_value["save"]["out_dir"];
            std::string newDir = homeDir + "/" + currentOutDir;
            json_value["save"]["out_dir"] = newDir;
        } else if (key.first == SAVE_AB){

            bool save=true;
            if (key.second == "false")
                save=false;
            json_value["save"]["ab"] = save;    

        } else if (key.first == SAVE_XYZ){

            bool save=true;
            if (key.second == "false")
                save=false;
            json_value["save"]["xyz"] = save;    

        } else if (key.first == ENABLE){

            bool save=true;
            if (key.second == "false")
                save=false;
            json_value["algo"][0]["enable"] = save;    

        }

    }
     std::ofstream o(newFileGenerate);
    o << std::setw(4) << json_value << std::endl;
    o.close();

     return newFileGenerate;

}

std::string generateTempCalibParam(std::string &calibFile, std::map<KeyCalibPar, float> keyVal)
{
    std::string newFileGenerate;
    nlohmann::json json_value;
    bool result = rs::io::readJsonFile(calibFile, json_value);
    if (!result)
    {
        LOG(ERROR) << "Error on read file : " << calibFile;
        return newFileGenerate;
    }
    std::string absolutePath = getDirectoryPath(calibFile);
    std::string nameTmpConib = std::string("Tmp_") + nameCalibConf;
    newFileGenerate = absolutePath + "/" + nameTmpConib;

    for (auto item : keyVal)
    {
        switch (item.first)
        {
        case ROI_X:
            json_value["roi_x"] = item.second;
            break;
        case ROI_Y:
            json_value["roi_y"] = item.second;
            break;
        case ROI_WIDTH:
            json_value["roi_width"] = item.second;
            break;
        case ROI_HEIGHT:
            json_value["roi_height"] = item.second;
            break;
        case RANGE_X_MIN:
            json_value["range_x_min"] = item.second;
            break;
        case RANGE_X_MAX:
            json_value["range_x_max"] = item.second;
            break;
        case RANGE_Y_MAX:
            json_value["range_y_max"] = item.second;
            break;
        case RANGE_Y_MIN:
            json_value["range_y_min"] = item.second;
            break;
        case RANGE_Z_MIN:
            json_value["range_z_min"] = item.second;
            break;
        case RANGE_Z_MAX:
            json_value["range_z_max"] = item.second;
            break;
        case CAMERA_POSITION_X:
            json_value["camera_position_x"] = item.second;
            break;
        case CAMERA_POSITION_Y:
            json_value["camera_position_y"] = item.second;
            break;
        case CAMERA_POSITION_Z:
            json_value["camera_position_z"] = item.second;
            break;
        case CAMERA_ORIENTATION_X:
            json_value["camera_orientation_x"] = item.second;
            break;
        case CAMERA_ORIENTATION_Y:
            json_value["camera_orientation_y"] = item.second;
            break;
        case CAMERA_ORIENTATION_Z:
            json_value["camera_orientation_z"] = item.second;
            break;
        default:
            break;
        }
    }
    std::ofstream o(newFileGenerate);
    o << std::setw(4) << json_value << std::endl;
    o.close();
    return newFileGenerate;
}
void getUpdCalibParameter(std::string namePar, std::string value, std::map<KeyCalibPar, float> &keyVal)
{

    float valueF = std::stof(value.c_str());
    if (namePar == "roi_x")
    {
        keyVal[ROI_X] = valueF;
    }
    else if (namePar == "roi_y")
    {
        keyVal[ROI_Y] = valueF;
    }
    else if (namePar == "roi_width")
    {
        keyVal[ROI_WIDTH] = valueF;
    }
    else if (namePar == "roi_height")
    {
        keyVal[ROI_HEIGHT] = valueF;
    }
    else if (namePar == "range_x_min")
    {
        keyVal[RANGE_X_MIN] = valueF;
    }
    else if (namePar == "range_x_max")
    {
        keyVal[RANGE_X_MAX] = valueF;
    }
    else if (namePar == "range_y_max")
    {
        keyVal[RANGE_Y_MAX] = valueF;
    }
    else if (namePar == "range_y_min")
    {
        keyVal[RANGE_Y_MIN] = valueF;
    }
    else if (namePar == "range_z_min")
    {
        keyVal[RANGE_Z_MIN] = valueF;
    }
    else if (namePar == "range_z_max")
    {
        keyVal[RANGE_Z_MAX] = valueF;
    }
    else if (namePar == "camera_position_x")
    {
        keyVal[CAMERA_POSITION_X] = valueF;
    }
    else if (namePar == "camera_position_y")
    {
        keyVal[CAMERA_POSITION_Y] = valueF;
    }
    else if (namePar == "camera_position_z")
    {
        keyVal[CAMERA_POSITION_Z] = valueF;
    }
    else if (namePar == "camera_orientation_x")
    {
        keyVal[CAMERA_ORIENTATION_X] = valueF;
    }
    else if (namePar == "camera_orientation_y")
    {
        keyVal[CAMERA_ORIENTATION_Y] = valueF;
    }
    else if (namePar == "camera_orientation_z")
    {
        keyVal[CAMERA_ORIENTATION_Z] = valueF;
    }
}
void getUpdRsAlgoParameter(std::string namePar, std::string value, std::set<Keyonfig> &key)
{

    if (namePar == "ab")
    {

        if (value == "true")
        {
            key.insert(ENABLE_SAVE_AB);
        }
        else
        {
            key.insert(DISABLE_SAVE_AB);
        }
    }
    else if (namePar == "xyz")
    {

        if (value == "true")
        {
            key.insert(ENABLE_SAVE_XYZ);
        }
        else
        {
            key.insert(DISABLE_SAVE_XYZ);
        }
    }
    else if (namePar == "xyz")
    {

        if (value == "true")
        {
            key.insert(ENABLE_SAVE_XYZ);
        }
        else
        {
            key.insert(DISABLE_SAVE_XYZ);
        }
    }
}
std::string verifyAndUpdCalibrationFile(std::string idTest, int currentStep, std::string homeDir, bool &isRemoveFile)
{
    isRemoveFile = false;
    std::string fullNameCalib = "";
    std::string tmpNameFile;
    std::map<KeyCalibPar, float> keyVal;
    auto listUpd = ConfigManagerTest::getInstance().getUpParameter(idTest);
    if (listUpd.size() != 0)
    {
        // if (listUpd.front().typeFile() == TYPE_FILE_CALIB_PR)
        {
            // tmpNameFile = listUpd.front().file();
            for (auto &itemUpdCalib : listUpd)
            {
                if (itemUpdCalib.typeFile() == TYPE_FILE_CALIB_PR)
                {
                    if (itemUpdCalib.currentStep() == currentStep)
                    {
                        tmpNameFile = itemUpdCalib.file();
                        getUpdCalibParameter(itemUpdCalib.parameterName(), itemUpdCalib.value(), keyVal);
                    }
                }
            }
        }
    }
    if (keyVal.size() == 0)
    {
        auto testMap = ConfigManagerTest::getInstance().getTestsMap();
        if (testMap.find(idTest) == testMap.end())
        {
            LOG(ERROR) << "Error Test:" << idTest << " Not found ";
        }
        else
        {
            fullNameCalib = findFileRecursively(homeDir, testMap[idTest].calibFile());
        }
    }
    else
    {
        tmpNameFile = findFileRecursively(homeDir, tmpNameFile);
        fullNameCalib = generateTempCalibParam(tmpNameFile, keyVal);
        isRemoveFile = true;
    }
    return fullNameCalib;
    if (!fullNameCalib.size())
    {
        LOG(ERROR) << "Error generate file  : " << tmpNameFile;
    }

    return fullNameCalib;
}
ExpectedResults *getExpectedResults(std::string idTest, int step)
{
    ExpectedResults *result = nullptr;

    auto listResult = ConfigManagerTest::getInstance().getExpectedRes(idTest);
    for (ExpectedResults &itemResult : listResult)
    {
        if (itemResult.currentStep() == step)
        {
            result = &itemResult;
            break;
        }
    }
    return result;
}
std::pair<std::string, std::string> buildTmpAlgoFile(std::string homeDir, std::string nameAlgoFile,std::set<Keyonfig>*Key)
{
    std::pair<std::string, std::string> retValue{"", ""};
    std::string pathAlgoFile = findFileRecursively(homeDir, nameAlgoFile);

    std::set<Keyonfig> keys{ALGO, OUTDIR};
    if (Key != nullptr) {
        for (Keyonfig  item:*Key){
            keys.insert(item);

        }
    }

    std::string tmpConfFile = generateTempConf(pathAlgoFile, keys);
    if (!tmpConfFile.size())
        return retValue;
    nlohmann::json json_value;
    bool result = rs::io::readJsonFile(tmpConfFile, json_value);
    if (!result)
    {
        LOG(ERROR) << "Error on load configuration file " << tmpConfFile;
        return retValue;
    }
    std::string outDir = json_value["save"]["out_dir"];
    rs::utils::filesystem::MakeDirectory(outDir);
    retValue.first = tmpConfFile;
    retValue.second = outDir;

    return retValue;
}
void findUpdRsAlgo(std::string idTest, int currentStep, std::set<Keyonfig> &keys)
{

    auto listUpd = ConfigManagerTest::getInstance().getUpParameter(idTest);
    if (listUpd.size() != 0)
    {
        for (auto &itemUpdCalib : listUpd)
        {
            if (itemUpdCalib.typeFile() == TYPE_FILE_RS_ALGO)
            {                
                getUpdRsAlgoParameter(itemUpdCalib.parameterName(), itemUpdCalib.value(),keys);
            }
        }
    }
}