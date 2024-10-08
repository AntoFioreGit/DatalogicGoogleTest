#pragma once

#include <memory>
#include <map>

#include "utils/json_convertible.h"

namespace rs
{
    struct ParameterRange
    {
        float min;
        float max;
        float step;
        float def;
    };

    class Parameter
    {
    public:
        virtual bool set(float value) = 0;
        virtual float query() const = 0;

        virtual float getDefault() const = 0;
        virtual ParameterRange getRange() const = 0;
        virtual bool isReadOnly() const { return false; }
        virtual const std::string& getDescription() const = 0;

        virtual ~Parameter() = default;
    };

    class ParameterBase : public virtual Parameter
    {
    public:
        ParameterBase(const ParameterRange& param_range) :
             param_range_(param_range)
        {}

        bool isValid(float value) const;
        ParameterRange getRange() const override;
        float getDefault() const override;

    protected:
        const ParameterRange param_range_;
    };

    class InvalidParameter : public ParameterBase
    {
    public:
        InvalidParameter():
            ParameterBase( {0, 0, 0, 0})
        {}

        bool set(float value) override
        {
            return false;
        }

        float query() const override
        {
            return std::numeric_limits<float>::quiet_NaN();
        }

        const std::string& getDescription() const override { return descr_; }

        std::string descr_ = "Invalid option";
    };

    template<class T>
    class PtrParameter : public ParameterBase
    {
    public:
        PtrParameter(T min, T max, T step, T def, T* value, const std::string& descr = {}) : 
              ParameterBase({ 
                static_cast<float>(min), 
                static_cast<float>(max),
                static_cast<float>(step), 
                static_cast<float>(def)}),
            min_(min), max_(max), step_(step), def_(def), value_(value), descr_(descr)
        {
            static_assert((std::is_arithmetic<T>::value), "PtrParameter class supports arithmetic built-in types only");
            //on_set_ = [](float x) {};
        }

        bool set(float value) override
        {
            T val = static_cast<T>(value);
            if ((max_ < val) || (min_ > val))
                return false;
            *value_ = val;
            return true;
        }

        float query() const override
        {
            return static_cast<float>(*value_);
        }

        const std::string& getDescription() const override 
        { 
            return descr_; 
        }

    private:
        T* value_;
        std::string descr_;
        T min_, max_, step_, def_; // stored separately so that logic can be done in base type
        // std::function<void(float)> on_set_;
    };
    
    //////////////////////////////////////////////////////////////
    /*
    class options
    {
    public:
        virtual rs::Parameter &getParameter(std::string id) = 0;
        virtual const rs::Parameter &getParameter(std::string id) const = 0;
        virtual bool supportParameter(std::string id) const = 0;
        virtual std::vector<std::string> getSupportedParameters() const = 0;
        //virtual std::string const& get_option_name(option) const = 0;
        virtual ~options() = default;
    };
    */

    class ParametersContainer : public JsonConvertible
    {
    public:
        bool supportParameter(std::string id) const //override
        {
            auto it = params_map_.find(id);
            if (it == params_map_.end())
                return false;
            return true;
        }

        rs::Parameter& getParameter(std::string id) //override
        {
            return const_cast<rs::Parameter&>(const_cast<const ParametersContainer*>(this)->getParameter(id));
        }
        
        const rs::Parameter& getParameter(std::string id) const // override
        {
            auto it = params_map_.find(id);
            static InvalidParameter invalid_opt; // use std::nullopt instead
            return (it == params_map_.end() ? invalid_opt : *it->second);
        }

        std::shared_ptr<rs::Parameter> getParameterHandler(std::string id)
        {
            return (const_cast<const ParametersContainer*>(this)->getParameterHandler(id));
        }

        std::shared_ptr<rs::Parameter> getParameterHandler(std::string id) const
        {
            auto it = params_map_.find(id);
            return (it == params_map_.end() ? std::shared_ptr<rs::Parameter>(nullptr) : it->second);
        }

        void addParameter(std::string id, std::shared_ptr<rs::Parameter> option)
        {
            params_map_[id] = option;
        }

        bool convertFromJson(const nlohmann::json& value) override;
        bool convertToJson(nlohmann::json &value) const override;

        std::vector<std::string> getSupportedParameters() const; // override;

        void setDefaults()
        {
            for (auto& [key, value] : params_map_)
            {
                auto def_value = value->getDefault();
                value->set(def_value);
            }
        }

        void setAll(ParametersContainer phandler)
        {
            auto& new_params_map = phandler.params_map_;
            for (auto &[key, value] : params_map_)
            {
                if (new_params_map.find(key) != new_params_map.end())
                {
                    value = phandler.params_map_[key]; // update current with new one
                }
            }
        }

        protected:
            std::map<std::string, std::shared_ptr<rs::Parameter> > params_map_; // use param name as key
        };
}