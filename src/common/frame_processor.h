#ifndef FRAME_PROCESSOR_H
#define FRAME_PROCESSOR_H

#define LIB_RS_API

#include <string>
#include <memory>

#include "calib/calib_parameters.h"
#include "frame.h"

namespace rs 
{
    class ParametersContainer;
    class Profile3dCalculator;
    class Profile3dCalculatorParameters;
}

class LIB_RS_API FrameProcessor
{
    using Profile3dCalculatorPtr = std::shared_ptr<rs::Profile3dCalculator>;
    using Profile3dCalculatorParametersPtr = std::shared_ptr<rs::Profile3dCalculatorParameters>;
    using ParametersContainerPtr = std::shared_ptr<rs::ParametersContainer>;

public:
    FrameProcessor(): 
        profile_calc_(nullptr), num_cpu_(-1),
        save_ab_(false), save_xyz_(false), vlog_level_(-1)
    {
        algo_config_map_.emplace("profile3d_calculator", nullptr);
    }

    /** \brief Load the algo parameters from configuration file.
     *
     * \param[in] filename: JSON configuration file (including extension) which should be specific to the given processor.
     * \return: returns true on success.
     */
    bool loadAlgoParameters(const std::string& filename, ParametersContainerPtr params);
    bool loadConfig(const std::string& filename);

    /** \brief Initialize the processing blocks and configure. 
     *  This is required before performing any operation on the input frame.
     * 
     * \param[in] config_file - The JSON configuration file (including extension).
     * \return: returns true on success.
     */
    bool initialize(const std::string& config_file = {});
    bool configure(const rs::ConveyorCalibrationParameters& tgt_calib, const rs::Intrinsics& cam_intrinsics);

    /** \brief Function to process input frame and output algo results.
     * 
     * \param[in] xyz: XYZ frame data.
     * \param[in] ab: Active Brightness frame data.
     * \return: returns 0 on success, error code (>1) on failure.
    */
    int invoke(const int16_t* const xyz, const uint16_t* const ab);

    /** \brief Recover the line profiles.*/
    std::vector<std::vector<float> >& getProfiles() const;

protected: 
    //void prepareFrames();
    void populateFrames(const int16_t *const xyz, const uint16_t *const ab);
    bool saveFrame(rs::Frame& frame, bool new_frame = true);

private:
    std::string version_;
    int num_cpu_;
    int vlog_level_;
    bool save_ab_;
    bool save_xyz_;
    std::string output_dir_;

    std::map<std::string, ParametersContainerPtr> algo_config_map_;

    rs::XyzFrame xyz_frame_;
    rs::ImageFrame ab_frame_;
    Profile3dCalculatorPtr profile_calc_;
};

#endif // FRAME_PROCESSOR_H