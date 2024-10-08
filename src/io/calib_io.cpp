#include "calib_io.h"

#include <glog/logging.h>

#include "io/file_json.h"
#include "utils/filesytem_helpers.h"
#include "calib/aditof_intrinsics.h"

namespace rs
{
    namespace io
    {
        bool readConveyorCalibrationFromJSON(const std::string& filename, rs::ConveyorCalibrationParameters& params)
        {
            std::string filename_ext = rs::utils::filesystem::GetFileExtensionInLowerCase(filename);
            if (filename_ext.empty() || filename_ext != "json")
            {
                LOG(ERROR) << "Unsupported file " << filename << ", file must be in JSON format";
                return false;
            }

            // Parse JSON file
            nlohmann::json json_value;
            if (!rs::io::readJsonFile(filename, json_value))
            {
                LOG(ERROR) << "Failed to parse file " << filename;
                return false;
            }

            LOG(INFO) << "Current calibration parameter file is: " << filename;

            return params.convertFromJson(json_value);
        }

        bool readConveyorCalibration(const std::string& filename, rs::ConveyorCalibrationParameters& params)
        {
            return readConveyorCalibrationFromJSON(filename, params);
        }

        bool readCameraIntrinsicsFromCCB(const std::string& filename, uint16_t mode, rs::Intrinsics& intrinsics)
        {
            if (!rs::utils::filesystem::FileExists(filename))
            {
                LOG(ERROR) <<  "File not found " << filename;
                return false;
            }

            std::string filename_ext = rs::utils::filesystem::GetFileExtensionInLowerCase(filename);
            if (filename_ext.empty() || filename_ext != "ccb")
            {
                LOG(ERROR) << "Unsupported file " << filename << ", file must be in CCB format";
                return false;
            }

            // Parse CCB file
            TofiCCBData ccb_data;
            if (!rs::ReadCCB(filename, &ccb_data, mode))
            {
                LOG(ERROR) << "Failed to parse file " << filename;
                return false;
            }

            LOG(INFO) << "Current intrinsic camera parameter file is: " << filename;

            // Populate
            intrinsics.image_size[0] = ccb_data.n_cols;
            intrinsics.image_size[1] = ccb_data.n_rows;
            intrinsics.principal_point[0] = ccb_data.camera_intrinsics.cx / ccb_data.row_bin_factor;
            intrinsics.principal_point[1] = ccb_data.camera_intrinsics.cy / ccb_data.col_bin_factor;
            intrinsics.focal_length[0] = ccb_data.camera_intrinsics.fx / ccb_data.row_bin_factor;
            intrinsics.focal_length[1] = ccb_data.camera_intrinsics.fy / ccb_data.col_bin_factor;

            // In the distort/undistort functions the coefficients are passed as:
            // (k1, k2, p1, p2[, k3 [,k4, k5, k6]])
            // see https://docs.opencv.org/4.x/d9/d0c/group__calib3d.html
            intrinsics.model = DistortionModel::DISTORTION_OPENCV_BROWN_CONRADY;
            float c[8] = {
                ccb_data.camera_intrinsics.k1, ccb_data.camera_intrinsics.k2,
                ccb_data.camera_intrinsics.p1, ccb_data.camera_intrinsics.p2,
                ccb_data.camera_intrinsics.k3, ccb_data.camera_intrinsics.k4,
                ccb_data.camera_intrinsics.k5, ccb_data.camera_intrinsics.k6,
            };
            memcpy(intrinsics.distortion_coeffs, c, 8 * sizeof(float));

            return true;
        }

        bool readCameraIntrinsics(const std::string& filename, uint16_t mode, rs::Intrinsics& intrinsics)
        {
            return readCameraIntrinsicsFromCCB(filename, mode, intrinsics);
        }
    }
}