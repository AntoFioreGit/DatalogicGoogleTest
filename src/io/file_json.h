#ifndef FILE_JSON_H
#define FILE_JSON_H

#include <string>
#include <vector>

#include "utils/json.h"

namespace rs
{
    namespace io
    {
        bool readJsonFile(const std::string& filename, nlohmann::json& json_content);
    } 
}

#endif // FILE_JSON_H