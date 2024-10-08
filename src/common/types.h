#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <cassert>
#include <map>
#include <string>
#include <array>
#include <vector>

namespace rs
{
    /** \brief Frames are different types of data provided by ADTF device. */
    enum FrameType
    {
        ANY,
        AB,         // 16-bit per-pixel active brightness image produced by ADTF
        DEPTH,      // 16-bit per-pixel radial depth image produced by ADTF
        CONFIDENCE, // 32-bit floating point depth confidence level produced by ADTF
        XYZ         // 16-bit per-pixel three-channel image of 3D coordinates produced by ADTF
    };

    static std::map<FrameType, std::string> const frame_type_map =
    {
        {FrameType::AB, "ab"},
        {FrameType::DEPTH, "depth"},
        {FrameType::CONFIDENCE, "conf"},
        {FrameType::XYZ, "xyz"}
    };

    static std::string frameTypeToName(FrameType frame_type)
    {
        std::string frame_name;
        auto it = frame_type_map.find(frame_type);
        if (it != frame_type_map.end())
        {
            frame_name = it->second;
        }
        return frame_name;
    }

    /** \brief VLOG is a recently-added facility of glog that allows for extremely fine-grained logging control. */
    enum VLogLevel
    {
        NONE,
        DEBUG,
        VERBOSE,
        EXPORT_IN,
        EXPORT_OUT
    };
    
    struct float3
    {
        float x, y, z;
        float &operator[](int i)
        {
            assert(i >= 0);
            assert(i < 3);
            return (*(&x + i));
        }
    };
    
    typedef struct
    {
        float min;
        float max;
    } Range;

    typedef struct
    {
        float3 min;
        float3 max;
    } VolumeRange;

    typedef struct
    {
        int width, height;
    } Sizee;

    typedef struct
    {
        int x, y;
        int width, height;
    } Rect;

    typedef struct
    {
        float x, y, z;
        float length, width, height;
    } Box;

    typedef struct
    {
        float3 position;
        float3 orientation;
    } Pose;

    /** \brief Distortion model: defines how pixel coordinates should be mapped to sensor coordinates. */
    enum DistortionModel
    {
        DISTORTION_NONE,                   /**< No distortion compensation required. */
        DISTORTION_OPENCV_BROWN_CONRADY,   /**< Variant of Brown-Conrady distortion implemented in OpenCV */
    };

    /** \brief Intrinsic camera parameters. */
    typedef struct
    {
        int image_size[2];              /**< (width, height) of the image, in pixels */          
        float principal_point[2];       /**< (cx, cy) = coordinates of the principal point of the image, as a pixel offset from the left edge from the top edge, respectively */
        float focal_length[2];          /**< (fx, fy) = focal length of the image plane, as a multiple of pixel width and pixel height, respectively */
        DistortionModel model;          /**< Distortion model of the image */
        float distortion_coeffs[8];     /**< Distortion coefficients (radial and tangential) */
    } Intrinsics;
}
#endif // TYPES_H