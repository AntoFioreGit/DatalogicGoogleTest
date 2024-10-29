#include <iostream>

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
std::string findFileRecursively(const std::string &directory, const std::string &filename);
//#include <filesystem>

//namespace fs = std::filesystem;
//std::string findFileRecursively(const fs::path& directory, const std::string& filename) ;
std::string getTimeStamp();
std::string getDirectoryPath(const std::string& filePath);