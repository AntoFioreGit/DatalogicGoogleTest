#include "Util.h"
std::string findFileRecursively(const fs::path& directory, const std::string& filename) {
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().filename() == filename) {
            return entry.path().string();
        } else if (entry.is_directory()) {
            auto result = findFileRecursively(entry.path(), filename);
            if (result.size()) {
                return result;
            }
        }
    }
    return "";
}
