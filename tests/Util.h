#include <iostream>

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <set>
//#include <filesystem>
enum Keyonfig {
    ALGO,
    OUTDIR,
    ENABLE_SAVE_AB,
    DISABLE_SAVE_AB,
    ENABLE_SAVE_XYZ,
    DISABLE_SAVE_XYZ
};
std::string findFileRecursively(const std::string &directory, const std::string &filename);

//namespace fs = std::filesystem;
//std::string findFileRecursively(const fs::path& directory, const std::string& filename) ;
std::string getTimeStamp();
std::string getDirectoryPath(const std::string& filePath);
std::string generateTempConf(std::string &confFile,std::set <Keyonfig>);
bool removeDirectory(std::string path);