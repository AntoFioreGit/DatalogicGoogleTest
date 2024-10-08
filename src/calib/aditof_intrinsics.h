#ifndef ADITOF_INTRINSICS_H
#define ADITOF_INTRINSICS_H

#include <cstdint>
#include <string>

namespace rs
{  
    // Structure for the camera intrinsic data
    typedef struct
    {
        // Focal
        float fx; // focal length of the image plane as a multiple of pixel width
        float fy; // focal length of the image plane as a multiple of pixel height
        // Principal point
        float cx; // horizontal coordinate of the principal point of the image as a pixel offset from the left edge
        float cy; // vertical coordinate of the principal point of the image as a pixel offset from the top edge
        // ????
        float codx;
        float cody;
        // Radial distorion coefficients
        float k1;
        float k2;
        float k3;
        float k4;
        float k5;
        float k6;
        // Tangential distortion coefficients
        float p2;
        float p1;
    } CameraIntrinsics;

    // Structure for the CCB data
    typedef struct
    {
        int n_rows; // height of the image in pixels
        int n_cols; // width of the image in pixels
        uint8_t n_freqs;
        uint8_t row_bin_factor;
        uint8_t col_bin_factor;
        uint16_t n_offset_rows;
        uint16_t n_offset_cols;
        uint16_t n_sensor_rows;
        uint16_t n_sensor_cols;
        CameraIntrinsics camera_intrinsics;
    } TofiCCBData;

    typedef struct
    {
        unsigned char* p_data;
        size_t size;
    } FileData;

    bool ReadCCB(const std::string& filename, TofiCCBData* p_ccb_data, uint16_t mode);
}

#endif // ADITOF_INTRINSICS_H