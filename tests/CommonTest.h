#include <iostream>
#include <string>

// used in File_JsonTest
static std::string versionConfigValue = "0.1.0";
static int logLevelConfigValue = 0;
static int numCpuConfigValue = 1;
static bool enableConfigValue = true;
static bool abConfig = true;
static bool xyzConfig = true;
static std::string outDirConfig = "data";
static std::string fileConfig = "data/profile3d_calculator.json";
static std::string algoConfigValue = "profile3d_calculator";

// used in File_JsonTest and FrameProcessorTest
static std::string fullNameConfig = "../../tests/data/config/rs_algo_config.json";

// used  FrameProcessorTest
static std::string profile3DCalcConfig = "../../tests/data/config/profile3d_calculator.json";
static std::string calibConfig = "../../tests/data/config/calib_data.json";

static int scanLine_spacingConfig = 70;
static int num_scalelineConfig = 3;
static bool static_roiConfig = true;

// used in Calib_IO_Test
static int roi_xConfigValue = 0;
static int roi_yConfigValue = 0;
static int roi_widthConfigValue = 0;
static int roi_heightConfigValue = 0;
static float range_x_minConfigValue = -549.0;
static float range_x_maxConfigValue = 553.0;
static float range_y_minConfigValue = -313.1;
static float range_y_maxConfigValue = 315.4;
static float range_z_minConfigValue = -100.0;
static float range_z_maxConfigValue = 3200.0;
static float camera_position_xConfigValue = 0.0;
static float camera_position_yConfigValue = 0.0;
static int camera_position_zConfigValue = 1335;
static float camera_orientation_xConfigValue = 11.0;
static float camera_orientation_yConfigValue = 0.0;
static float camera_orientation_zConfigValue = 0.0;

static std::string ConveyorCalibrationConfig = "../../tests/data/crXXX.ccb";
enum DistortionModel
{
    DISTORTION_NONE,                 /**< No distortion compensation required. */
    DISTORTION_OPENCV_BROWN_CONRADY, /**< Variant of Brown-Conrady distortion implemented in OpenCV */
};
struct Intrinsics
{
    int image_size[2];          /**< (width, height) of the image, in pixels */
    float principal_point[2];   /**< (cx, cy) = coordinates of the principal point of the image, as a pixel offset from the left edge from the top edge, respectively */
    float focal_length[2];      /**< (fx, fy) = focal length of the image plane, as a multiple of pixel width and pixel height, respectively */
    DistortionModel model;      /**< Distortion model of the image */
    float distortion_coeffs[8]; /**< Distortion coefficients (radial and tangential) */
};

static Intrinsics IntrinsicsConigValue  {
    {512,512},
    {264.687744,255.533188},
    {388.100891,388.21579},
    DISTORTION_OPENCV_BROWN_CONRADY,
    {-0.109356761,
    0.0615734681,
    -8.5030646e-05,
    -2.79931082e-05,
    0.0844119191,
    0.235934585,
    -0.0846114829,
    0.169457629}
};