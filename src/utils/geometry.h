#pragma once

#include <math.h>
#include <vector>
#include <opencv2/core/types.hpp>

#include "common/types.h"


static const double pi = std::acos(-1);
static const double d2r = pi / 180;
static const double r2d = 180 / pi;

namespace rs
{
    namespace utils
    {
        namespace geometry
        {
            template <typename T>
            T deg2rad(T val)
            {
                return T(val * d2r);
            }

            template <typename T>
            T rad2deg(T val)
            {
                return T(val * r2d);
            }

            inline size_t units2pix(const float length, float pixel_size)
            {
                return size_t(round(fabs(length) / pixel_size));
            }

            /** \brief Calculate 3x3 rotation matrix about the x-axis */
            cv::Mat getRotationMatrixX(const float radians);
            /** \brief Calculate 3x3 rotation matrix about the y-axis */
            cv::Mat getRotationMatrixY(const float radians);
            /** \brief Calculate 3x3 rotation matrix about the z-axis */
            cv::Mat getRotationMatrixZ(const float radians);

            /** \brief Calculate rotation matrix from Euler angles.
             *
             * Order for Euler angle rotations is "ZYX". The three specified Euler angles
             * are interpreted in order as a rotation around the z-axis, a rotation around
             * the y-axis, and a rotation around the x-axis.
             *
             * When applying this rotation to a point, it will apply the axis rotations in
             * the (reverse) order x, then y, then z.
             *
             * When using the rotation matrix, pre-multiply it with the points to be rotated
             * (as opposed to post-multiplying):
             *    p' = R * p, where p is a column vector (or p' = p * R, where p is a row vector)
             *
             *
             * \param[in] eulerAngles - sequence of z, y, and x angles in radians.
             * \return R - rotation matrix
             **/
            cv::Mat getRotationMatrix3d(const float eulerAngles[3]);

            /** \brief Compute extrinsic parameters from camera pose. */
            //rs::Extrinsics fromPose(const rs::Pose& a);
            //rs::Pose toPose(const rs::Extrinsics& a);

            /**
             * \brief Project a 3d point to pixel coordinates.
             *
             * This is the inverse of projectPixelTo3dRay().
             *
             * \param xyz 3d point in the camera coordinate frame
             * \return (u,v) in (unrectified) pixel coordinates
             */
            cv::Point2f project3dToPixel(const float point[3], const float camera_matrix[9],
                                         const float rotation[9], const float translation[3],
                                         const float distortion_coeffs[8]);
        }
    }
}
