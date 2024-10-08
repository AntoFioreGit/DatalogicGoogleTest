#ifndef CAMERA_PARAMETERS_H
#define CAMERA_PARAMETERS_H

#include <vector>
#include <opencv2/core/core.hpp>

namespace rs
{
    namespace calib
    {

        /** \class Pinhole camera parameters.
         *
         * \brief Contains both intrinsic and extrinsic pinhole camera parameters.
         */
        class CameraParameters
        {
        public:
            CameraParameters();

            /**  \brief Set the camera intrinsic parameters.
             *
             * Include the image size, the focal length, and the optical center (the principal point), in pixels.
             */
            void setIntrinsics(int width, int height, float fx, float fy, float ppx, float ppy);
            void setIntrinsics(const int size[2], const float f[2], const float pp[2]);
            bool checkIntrinsics();

            /**  \brief Set the camera lens distortion parameters.
             *
             *  Include the radial and tangential distortion coefficients.
             */
            void setDistortionCoefficients(const float coeffs[8]);

            /** \brief Set the extrinsics from camera pose */
            void setExtrinsics(const float orientation[3], const float position[3]);

        public:
            cv::Size image_size_;
            cv::Mat camera_matrix_;         /** Intrinsic camera matrix, K  = [fx, 0, cx; 0, fy, cy; 0, 0, 1] */
            cv::Mat rotation_;              /** Rotation matrix, R = Rz * Ry * Rx, describing the 3D rotation of the camera relative to a planar calibration target */
            cv::Mat translation_;           /** Translation vector, T = (tx, ty, tz), describing the 3D position of the camera relative to a planar calibration target */
            cv::Mat distortion_coeffs_;     /** Vector of distortion coefficients, d = (k1, k2, p1, p2 [,k3 [,k4, k5, k6]]), of 4, 5, 8 elements */
        };
    }
}

#endif // CAMERA_PARAMETERS_H