#include "file_json.h"

#include <fstream>
#include <glog/logging.h>

using json = nlohmann::json;

namespace rs
{
    namespace io
    {
        bool readJsonFile(const std::string& filename, json& json_content)
        {
            std::ifstream ifs(filename);
            if (!ifs.is_open())
            {
                // Path is empty or does not exist
                LOG(ERROR) << "Failed to open: " << filename;
                return false;
            }

            // Parse with exceptions
            try
            {
                json_content = json::parse(ifs);
            }
            catch (const json::parse_error &e)
            {
                LOG(ERROR) << e.what();
                return false; // input is invalid JSON
            }

            return true;
        }
    }
}