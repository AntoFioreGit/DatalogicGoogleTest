#ifndef CALIB_IO_H
#define CALIB_IO_H

#include "calib/calib_parameters.h"

namespace rs
{
    namespace io
    {
        bool readConveyorCalibration(const std::string& filename, rs::ConveyorCalibrationParameters& params);
        bool readConveyorCalibrationFromJSON(const std::string& filename, rs::ConveyorCalibrationParameters& params);

        bool readCameraIntrinsics(const std::string& filename, uint16_t mode, rs::Intrinsics& intrinsics);
        bool readCameraIntrinsicsFromCCB(const std::string& filename, uint16_t mode, rs::Intrinsics& intrinsics);
    }
}

#endif // CALIB_IO_H