#include "frame_processor.h"

#include <time.h>
#include <glog/logging.h>

#include "io/file_json.h"
#include "utils/geometry.h"
#include "utils/filesytem_helpers.h"
#include "algorithms/profile3d_calculator.h"

using namespace rs;

bool FrameProcessor::loadAlgoParameters(const std::string& filename, ParametersContainerPtr params)
{
    // Check file extension
    std::string ext = rs::utils::filesystem::GetFileExtensionInLowerCase(filename);
    if (ext.empty() || ext != "json")
    {
        LOG(ERROR) << "File " << filename << "is not supperted, it must be in JSON format";
        return false;
    }

    // Parse JSON file
    nlohmann::json json_value;
    if (!rs::io::readJsonFile(filename, json_value))
    {
        LOG(ERROR) << "Failed to parse file " << filename;
        return false;
    }

    LOG(INFO) << "Current algo settings file is: " << filename;

    return params ? params->convertFromJson(json_value) : false;
}

bool FrameProcessor::loadConfig(const std::string& config_file)
{
    // Populate the data from the json file provided
    nlohmann::json json_config;
    if (!rs::io::readJsonFile(config_file, json_config))
    {
        LOG(ERROR) << "Failed to parse configuration file: " << config_file;
        return false;
    }

    LOG(INFO) << "Current configuration file is: " << config_file;

    if (json_config.contains("version"))
    {
        version_ = json_config["version"];
        LOG(INFO) << "File version: " << version_;
    }

    if (json_config.contains("num_cpu"))
    {
        num_cpu_ = json_config["num_cpu"];
        LOG(INFO) << "Num CPUs to use: " << num_cpu_;
    }

    if (json_config.contains("vlog_level"))
    {
        vlog_level_ = json_config["vlog_level"];
        LOG(INFO) << "Debug level: " << vlog_level_;
    }
    
    // Get algo parameters file location (if any) and load file content
    if (json_config.contains("algo"))
    {
        for (auto& json_value : json_config["algo"])
        {
            std::string algo_id = json_value["id"];
            bool algo_enabled = json_value["enable"];
            std::string algo_conf_file = json_value["config_file"];

            ParametersContainerPtr params;
            if (algo_id == "profile3d_calculator")
            {
                params = ParametersContainerPtr(new Profile3dCalculatorParameters());
            }
            else
            {
                LOG(ERROR) << "Algo  " << algo_id << " not supported";
                 return false;
            }

            if (loadAlgoParameters(algo_conf_file, params))
            {
                // Save data settings
                algo_config_map_[algo_id] = params;
            }
            else
            {
                LOG(WARNING) << "Failed to load  " << algo_id << " algo configuration file " << algo_conf_file;
            }
        }
    }

    if (json_config.contains("save"))
    {
        auto json_value = json_config["save"];
        save_ab_ = json_value["ab"];
        save_xyz_ = json_value["xyz"];
        output_dir_ = json_value["out_dir"];
    }

    return true;
}

bool FrameProcessor::initialize(const std::string& config_file)
{
    LOG(INFO) << "Initializing FrameProcessor";

    if (!loadConfig(config_file))
    {
        LOG(ERROR) << "Load configuration data failed" << config_file;
        return false;
    }

    LOG(INFO) << "FrameProcessor initialized";

    return true;
}

bool FrameProcessor::configure(const rs::ConveyorCalibrationParameters& calib_params, 
                               const rs::Intrinsics& intrinsics)
{
    // 1. Set camera parameters
    rs::calib::CameraParameters camera_params;
    const int* sz = intrinsics.image_size;
    const float* f = intrinsics.focal_length;
    const float* pp = intrinsics.principal_point;
    const float* c = intrinsics.distortion_coeffs;

    camera_params.setIntrinsics(sz, f, pp);
    camera_params.setDistortionCoefficients(c);

    const float* orient = calib_params.camera_orientation_;
    const float* pos = calib_params.camera_position_;
    camera_params.setExtrinsics(orient, pos);

    // 2. Allocate target frames
    ab_frame_.prepare(sz[0], sz[1]);
    xyz_frame_.prepare(sz[0], sz[1]);

    // 2. Init algorithms using loaded parameters
    for (auto [algo_id, algo_params] : algo_config_map_)
    {
        if (algo_id == "profile3d_calculator")
        {
            std::shared_ptr<rs::Profile3dCalculatorParameters> p = 
                std::dynamic_pointer_cast<rs::Profile3dCalculatorParameters>(algo_params);
            profile_calc_ = Profile3dCalculatorPtr(new rs::Profile3dCalculator(*p));
            if (!profile_calc_->initialize(camera_params, calib_params.volume_bound_, calib_params.roi_))
                return false;
            profile_calc_->setLogLevel(vlog_level_);
        }
    }
    return true;
}

void FrameProcessor::populateFrames(const int16_t* const xyz, const uint16_t* const ab)
{
    // Copy the content of the input data to the targets and, if enabled, save data for diagnostics
    int num_pixels = xyz_frame_.resolution();

    xyz_frame_.copyPoints((int16_t*)xyz, num_pixels);
    if (save_xyz_)
    {
        saveFrame(xyz_frame_, true);
    }

    if (ab)
    {
        ab_frame_.copyPixels((uint16_t *)ab, num_pixels);
        if (save_ab_)
        {
            saveFrame(ab_frame_, !save_xyz_);
        }
    }
}

int FrameProcessor::invoke(const int16_t* const xyz, const uint16_t* const ab)
{
    try
    {
        populateFrames(xyz, ab);

        int num_profiles = 0;
        if (profile_calc_)
            num_profiles = profile_calc_->findProfiles(xyz_frame_);

        return num_profiles;
    }
    catch (cv::Exception &e)
    {
        LOG(ERROR) << e.what();
        return -1;
    }
    catch (const std::exception &e)
    {
        LOG(ERROR) << e.what();
        return -1;
    }
    catch (...) // everything else
    {
        LOG(ERROR) << "Something bad happen during profile extraction.";
        return -1;
    }
}

std::vector<std::vector<float> >& FrameProcessor::getProfiles() const
{
    static std::vector<std::vector<float> > vec;
    if (profile_calc_)
        vec = profile_calc_->getProfiles();
    else
        vec.clear();

    return vec;
}

bool FrameProcessor::saveFrame(rs::Frame& frame, bool new_frame)
{
    static size_t max_num_files = 30;
    static size_t file_id = 0;

    if (new_frame)
    {
        if (file_id < max_num_files)
        {
            file_id++;
        }
        else
        {
            file_id = 1;
        }
    }
    
    FrameType frame_type = FrameType::ANY;
    if (frame.is<ImageFrame>())
        frame_type = FrameType::AB;
    else if(frame.is<XyzFrame>())
        frame_type = FrameType::XYZ;

    std::string frame_type_str = frameTypeToName(frame_type);
    std::string file_name = "data/" + frame_type_str + std::to_string(file_id);
#if defined(__linux__)
    file_name = "/" + file_name;
#endif

    if (frame_type == FrameType::AB)
    {
        file_name += ".png";
        if (frame.hasData())
        {
            rs::Rect& roi = profile_calc_->getImageROI();
            std::vector<size_t>& scan_lines = profile_calc_->getImageScanlines();
            ImageFrame &im = dynamic_cast<ImageFrame&>(frame);
            if (!im.exportToPng(file_name, roi, scan_lines))
            {
                LOG(ERROR) << "Unable to save AB frame in " << file_name;
                return false;
            }
        }
    }
    else if (frame_type == FrameType::XYZ)
    {
        file_name += ".ply";
        if (frame.hasData())
        {
            XyzFrame &xyz = dynamic_cast<XyzFrame &>(frame);
            if (!xyz.exportToPly(file_name, nullptr, 1 /*query_vol_.range_z.min*/))
            {
                LOG(ERROR) << "Unable to save XYZ frame in " << file_name;
                return false;
            }
        }
    }
    else
    {
        LOG(ERROR) << "Unsupported frame type " << size_t(frame_type);
        return false;
    }

    return true;
}