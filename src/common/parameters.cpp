#include "parameters.h"

#include <glog/logging.h>

#include "io/file_json.h"
#include "utils/filesytem_helpers.h"

namespace rs 
{
    bool ParameterBase::isValid(float value) const
    {
        if (!std::isnormal(param_range_.step) && param_range_.step != 0)
        {
            LOG(ERROR) << "Step is not properly defined. (" << param_range_.step << ")";
            return false;
        }

        if ((value < param_range_.min) || (value > param_range_.max))
            return false;

        if (param_range_.step == 0)
            return true;

        auto n = (value - param_range_.min) / param_range_.step;
        return (fabs(fmod(n, 1)) < std::numeric_limits<float>::min());
    }

    ParameterRange ParameterBase::getRange() const
    {
        return param_range_;
    }

    float ParameterBase::getDefault() const
    {
        return param_range_.def;
    }

    /////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> ParametersContainer::getSupportedParameters() const
    {
        std::vector<std::string> param_keys;
        for (auto& p : params_map_)
            param_keys.push_back(p.first);

        return param_keys;
    }

    bool ParametersContainer::convertFromJson(const nlohmann::json& json_value)
    {
        LOG(INFO) << "Parsed parameters:\n"
                  << std::setw(4) << json_value;

        // Check if json contains all the supported options
        for (auto &key : getSupportedParameters())
        {
            if (!json_value.contains(key))
            {
                LOG(ERROR) << "Bad file, parameter " << key << " not found.";
                return false;
            }
        }

        // Set parameter using the value currently assigned into the json
        for (auto &[key, value] : json_value.items())
        {
            if (supportParameter(key))
            {
                auto &opt = getParameter(key);
                if (!opt.set(value))
                {
                    LOG(ERROR) << "Bad file, given value " << value << " of parameter " << key << " is out of range";
                    return false;
                }
            }
            else
            {
                LOG(WARNING) << "Parameter " << key << " not supported, skip";
            }
        }

        return true;
    }

    bool ParametersContainer::convertToJson(nlohmann::json& params) const
    {
        return false; // TODO: implement
    }
}