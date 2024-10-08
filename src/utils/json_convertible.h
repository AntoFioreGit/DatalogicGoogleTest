#pragma once

#include "json.h"

/** \brief Class JsonConvertible defines the behavior of a class that can convert
 *   itself to/from a json object.
*/
class JsonConvertible
{
public:
    virtual ~JsonConvertible() {}

public:
    virtual bool convertToJson(nlohmann::json& value) const = 0;
    virtual bool convertFromJson(const nlohmann::json& value) = 0;
};
