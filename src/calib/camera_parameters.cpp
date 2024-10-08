#include "camera_parameters.h"

#include <glog/logging.h>
#include "utils/geometry.h"

namespace rs
{
    namespace calib
    {
        CameraParameters::CameraParameters()
        {
            image_size_ = cv::Size(0, 0);
            camera_matrix_ = cv::Mat::zeros(3, 3, CV_32F);
            rotation_ = cv::Mat::eye(3, 3, CV_32F);            // assume that camera is not oriented
            translation_ = cv::Mat::zeros(3, 1, CV_32F);       // assume that there world and camera coordinate systems have the same origin
            distortion_coeffs_ = cv::Mat::zeros(8, 1, CV_32F); // assume that there is no lens distortion
        }

        void CameraParameters::setIntrinsics(int width, int height, float fx, float fy, float ppx, float ppy)
        {
            image_size_.width = width;
            image_size_.height = height;
            camera_matrix_.at<float>(0, 0) = fx;
            camera_matrix_.at<float>(0, 2) = ppx;
            camera_matrix_.at<float>(1, 1) = fy;
            camera_matrix_.at<float>(1, 2) = ppy;
            camera_matrix_.at<float>(2, 2) = 1.f;
        }

        void CameraParameters::setIntrinsics(const int size[2], const float f[2], const float pp[2])
        {
            setIntrinsics(size[0], size[1], f[0], f[1], pp[0], pp[1]);
        }

        bool CameraParameters::checkIntrinsics()
        {
            const float *K = camera_matrix_.ptr<float>();

            float fx = K[0];
            float fy = K[4];
            if (fx <= 0 || fy <= 0)
            {
                LOG(ERROR) << "Focal length (fx and fy) must be positive";
                return false;
            }

            float cx = K[2];
            float cy = K[5];
            if (cx < 0 || cx >= image_size_.width ||
                cy < 0 || cy >= image_size_.height)
            {
                LOG(ERROR) << "Principal point must be within the image";
                return false;
            }

            if (fabs(K[1]) > 1e-5 || fabs(K[3]) > 1e-5 ||
                fabs(K[6]) > 1e-5 || fabs(K[7]) > 1e-5 ||
                fabs(K[8] - 1.f) > 1e-5)
            {
                LOG(ERROR) << "Intrinsic camera matrix must have [fx 0 cx; 0 fy cy; 0 0 1] shape";
                return false;
            }

            return true;
        }

        void CameraParameters::setDistortionCoefficients(const float coeffs[8])
        {
            memcpy(distortion_coeffs_.ptr<float>(), coeffs, 8 * sizeof(float));
        }

        void CameraParameters::setExtrinsics(const float orientation[3], const float position[3])
        {
            // Set rotation matrix
            float angles[3] = {0};
            for (int i = 0; i < 3; i++)
                angles[i] = rs::utils::geometry::deg2rad(orientation[i]);

            rotation_ = rs::utils::geometry::getRotationMatrix3d(angles);

            // Set translation vector
            memcpy(translation_.ptr<float>(), position, 3 * sizeof(float));
        }
    }
}