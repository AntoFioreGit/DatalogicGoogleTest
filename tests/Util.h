#include <iostream>

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <set>
#include <map>
// #include <filesystem>
enum Keyonfig
{
    ALGO,
    OUTDIR,
    ENABLE_SAVE_AB,
    DISABLE_SAVE_AB,
    ENABLE_SAVE_XYZ,
    DISABLE_SAVE_XYZ
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
std::string generateTempCalibParam(std::string &calibFile, std::map<KeyCalibPar,float>);
bool removeDirectory(std::string path);
void getUpdCalibParameter(std::string namePar, std::string value, std::map<KeyCalibPar, float>& keyVal);
void getUpdRsAlgoParameter(std::string namePar, std::string value, std::set<Keyonfig>& key );