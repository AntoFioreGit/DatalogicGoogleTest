#include "geometry.h"

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <glog/logging.h>

namespace rs
{
    namespace utils
    {
        namespace geometry
        {
            cv::Mat getRotationMatrixX(const float radians)
            {
                cv::Mat Rx = (cv::Mat_<float>(3, 3) << 1, 0, 0,
                              0, cos(radians), -sin(radians),
                              0, sin(radians), cos(radians));
                return Rx;
            }

            cv::Mat getRotationMatrixY(const float radians)
            {
                cv::Mat Ry = (cv::Mat_<float>(3, 3) << cos(radians), 0, -sin(radians),
                              0, 1, 0,
                              sin(radians), 0, cos(radians));
                return Ry;
            }

            cv::Mat getRotationMatrixZ(const float radians)
            {
                cv::Mat Rz = (cv::Mat_<float>(3, 3) << cos(radians), -sin(radians), 0,
                              sin(radians), cos(radians), 0,
                              0, 0, 1);
                return Rz;
            }

            cv::Mat getRotationMatrix3d(const float eulerAngles[3])
            {
                float x_angle = eulerAngles[0]; // pitch (aka tilt)
                float y_angle = eulerAngles[1]; // roll
                float z_angle = eulerAngles[2]; // yaw
                cv::Mat Rx = getRotationMatrixX(x_angle);
                cv::Mat Ry = getRotationMatrixY(y_angle);
                cv::Mat Rz = getRotationMatrixZ(z_angle);
                cv::Mat R = Rz * Ry * Rx;
                return R;
            }

            cv::Point2f project3dToPixel(const float point[3],
                                         const float rotation[9], const float translation[3],
                                         const float camera_matrix[9], const float distortion_coeffs[8])
            {
                std::vector<cv::Point2f> projected_point;
                std::vector<cv::Point3f> object_point;
                object_point.push_back(cv::Point3f(point[0], point[1], point[2]));
                
                cv::Mat K = cv::Mat(3, 3, CV_32F, (float*)camera_matrix);
                cv::Mat rmat = cv::Mat(3, 3, CV_32F, (float *)rotation);
                cv::Mat rvec(3, 1, CV_32F); // rodrigues rotation matrix
                cv::Mat tvec = cv::Mat(3, 1, CV_32F, (float *)translation);
                cv::Mat dist_coeffs = cv::Mat(8, 1, CV_32F, (float *)distortion_coeffs);

                //LOG(INFO) << "K\n" << K;
                //LOG(INFO) << "rmat\n" << rmat;
                //LOG(INFO) << "tvec\n" << tvec;

                cv::Rodrigues(rmat, rvec);
                cv::projectPoints(object_point, rvec, tvec, K, dist_coeffs, projected_point);

                return projected_point[0];
            }
        }
    }
}