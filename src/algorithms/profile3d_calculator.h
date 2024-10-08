#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <opencv2/core.hpp>
#include <glog/logging.h>

#include "common/frame.h"
#include "common/types.h"
#include "common/parameters.h"
#include "calib/camera_parameters.h"

namespace cv
{
    class Mat;
}

namespace rs
{
    class Profile3dCalculatorParameters : public ParametersContainer
    {
    public:
        Profile3dCalculatorParameters()
        {
            auto static_roi = std::make_shared<PtrParameter<bool>>(
                0, 1, 1, 0, &static_roi_, 
                "Roi does not change when processing");

            auto num_scanlines = std::make_shared<PtrParameter<size_t>>(
                1, 100, 1, 0, &num_scanlines_, 
                "Number of scanlines where compute the profile");

            auto scanline_spacing = std::make_shared<PtrParameter<float>>(
                1,  1000, 1, 50, &scaline_spacing_, 
                "Distance between two scanlines (in millimeters)");

            addParameter("static_roi", static_roi);
            addParameter("num_scanlines", num_scanlines);
            addParameter("scanline_spacing", scanline_spacing);
        }

    public:
        bool static_roi_;
        size_t num_scanlines_;
        float scaline_spacing_;
    };

    class Profile3dCalculator
    {
        using Parameters = typename rs::Profile3dCalculatorParameters;

    public:
        Profile3dCalculator(const Parameters& params = Parameters()) :
            query_vol_{ {0, 0, 0}, {0, 0, 0}},
            image_roi_{0, 0, 0, 0},
            vlog_level_(-1),
            params_(params)
        {}

        void setLogLevel(int level)
        {
            vlog_level_ = level;
        }

        void setParameters(const Parameters& params)
        {
            params_.setAll(params);
        }
        
        int findProfiles(rs::XyzFrame& xyz_frame);
        bool initialize(const rs::calib::CameraParameters &camera_params, const rs::VolumeRange &volume_bound, const rs::Rect &image_roi);

        std::vector<std::vector<float> >& getProfiles () { return profiles_; }
        std::vector<size_t>& getImageScanlines() { return scan_lines_; }
        rs::Rect& getImageROI() { return image_roi_; }

    protected:
        int findObjects(rs::XyzFrame& xyz_frame, const float min_size[3], const float max_size[3], bool calib_target, cv::Mat &obj_mask);
        void lineProfile(rs::XyzFrame& xyz_frame, const cv::Mat& obj_mask, int scan_line, int approx_func, std::vector<float>& coords);

        std::vector<size_t> getScanlines(float c[3], int anchor_row, size_t n_lines, float spacing);
        rs::Rect getRoi(float c[3], float br[3]);

        inline bool isValidRoi(const rs::Rect& roi)
        {
            return !(roi.x == 0 && roi.y == 0 &&
                     roi.width == 0 && roi.height == 0);
        }

        inline bool isValidVolume(const rs::VolumeRange& vol_range)
        {
            float x_min = vol_range.min.x;
            float x_max = vol_range.max.x;
            float y_min = vol_range.min.y;
            float y_max = vol_range.max.y;
            float z_min = vol_range.min.z;
            float z_max = vol_range.max.z;
            if (x_min >= x_max || y_min >= y_max || z_min >= z_max ||
                fabs(x_max - x_min) < 100 || fabs(y_max - y_min) < 100 || fabs(z_max - z_min) < 100)
            {
                return false;
            }

            return true;
        }

    private : 
        std::vector<size_t> scan_lines_;
        std::vector<std::vector<float> > profiles_;
        rs::Rect image_roi_;
        rs::VolumeRange query_vol_;
        rs::calib::CameraParameters camera_params_;
        int vlog_level_;
        Parameters params_;
    };
}