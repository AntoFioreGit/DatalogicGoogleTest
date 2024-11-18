#ifndef Util_h
#define Util_h
#include <iostream>

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <set>
#include <map>
// #include <filesystem>
#include "ConfigManagerTest.h"
enum Keyonfig
{
    ALGO,
    OUTDIR,
    ENABLE_SAVE_AB,
    DISABLE_SAVE_AB,
    ENABLE_SAVE_XYZ,
    DISABLE_SAVE_XYZ,
    SAVE_AB,
    SAVE_XYZ,
    ENABLE
};
enum KeyCalibPar
{
    ROI_X,
    ROI_Y,
    ROI_WIDTH,
    ROI_HEIGHT,
    RANGE_X_MIN,
    RANGE_X_MAX,
    RANGE_Y_MIN,
    RANGE_Y_MAX,
    RANGE_Z_MIN,
    RANGE_Z_MAX,
    CAMERA_POSITION_X,
    CAMERA_POSITION_Y,
    CAMERA_POSITION_Z,
    CAMERA_ORIENTATION_X,
    CAMERA_ORIENTATION_Y,
    CAMERA_ORIENTATION_Z
};
#define TYPE_FILE_CALIB_PR "Calib_Pr"
#define TYPE_FILE_RS_ALGO "Rs_Algo"
#define TYPE_FILE_PROF3D "Prof3D"

std::string findFileRecursively(const std::string &directory, const std::string &filename);

// namespace fs = std::filesystem;
// std::string findFileRecursively(const fs::path& directory, const std::string& filename) ;
std::string getTimeStamp();
std::string getDirectoryPath(const std::string &filePath);
std::string generateTempConf(std::string &confFile, std::set<Keyonfig>);
std::string generateTempConf(std::string &confFile, std::map<Keyonfig,std::string> keys);
std::string generateTempCalibParam(std::string &calibFile, std::map<KeyCalibPar,float>);
bool removeDirectory(std::string path);
void getUpdCalibParameter(std::string namePar, std::string value, std::map<KeyCalibPar, float>& keyVal);
void getUpdRsAlgoParameter(std::string namePar, std::string value, std::set<Keyonfig>& key );
std::string verifyAndUpdCalibrationFile(std::string idTest, int currentStep, std::string homeDir,bool &isRemoveFile);
ExpectedResults *getExpectedResults(std::string idTest, int step);
std::list <ExpectedResults *> getExpectedResultsList(std::string idTest, int step);
std::pair<std::string, std::string> buildTmpAlgoFile(std::string homeDir, std::string nameAlgoFile,std::set<Keyonfig>*Key=nullptr);
void findUpdRsAlgo(std::string idTest,int currentStep,std::set<Keyonfig> &keys);
#endif