#ifndef CALIB_PARAMETERS_H
#define CALIB_PARAMETERS_H

#include "common/types.h"
#include "common/parameters.h"

namespace rs
{
    class ConveyorCalibrationParameters : public ParametersContainer
    {
    public:
        ConveyorCalibrationParameters()
        {
            auto roi_x = std::make_shared<PtrParameter<int> >(0, 4096, 1, 0, &roi_.x);
            auto roi_y = std::make_shared<PtrParameter<int> >(0, 4096, 1, 0, &roi_.y);
            auto roi_width = std::make_shared<PtrParameter<int> >(0, 4096, 1, 0, &roi_.width);
            auto roi_height = std::make_shared<PtrParameter<int> >(0, 4096, 1, 0, &roi_.height);

            // Belt width (x) is in the range [700, 1500] mm, mounting height (z) is in the range (1200, 1700) mm.
            auto range_x_min = std::make_shared<PtrParameter<float> >(-3000, 3000, 1, 0, &volume_bound_.min.x);
            auto range_x_max = std::make_shared<PtrParameter<float> >(-3000, 3000, 1, 0, &volume_bound_.max.x);

            auto range_y_min = std::make_shared<PtrParameter<float> >(-3000, 3000, 1, 0, &volume_bound_.min.y);
            auto range_y_max = std::make_shared<PtrParameter<float> >(-3000, 3000, 1, 0, &volume_bound_.max.y);

            auto range_z_min = std::make_shared<PtrParameter<float> >(-500, 3500, 1, 0, &volume_bound_.min.z);
            auto range_z_max = std::make_shared<PtrParameter<float> >(-500, 3500, 1, 0, &volume_bound_.max.z);

            auto camera_position_x = std::make_shared<PtrParameter<float> >(-3000, 3000, 1, 0, &camera_position_[0]);
            auto camera_position_y = std::make_shared<PtrParameter<float> >(-3000, 3000, 1, 0, &camera_position_[1]);
            auto camera_position_z = std::make_shared<PtrParameter<float> >(-500, 3500, 1, 0, &camera_position_[2]);

            auto camera_orientation_x = std::make_shared<PtrParameter<float> >(-90, 90, 0.1, 0, &camera_orientation_[0]);
            auto camera_orientation_y = std::make_shared<PtrParameter<float> >(-90, 90, 0.1, 0, &camera_orientation_[1]);
            auto camera_orientation_z = std::make_shared<PtrParameter<float> >(-90, 90, 0.1, 0, &camera_orientation_[2]);

            addParameter("roi_x", roi_x);
            addParameter("roi_y", roi_y);
            addParameter("roi_width", roi_width);
            addParameter("roi_height", roi_height);
            addParameter("range_x_min", range_x_min);
            addParameter("range_x_max", range_x_max);
            addParameter("range_y_min", range_y_min);
            addParameter("range_y_max", range_y_max);
            addParameter("range_z_min", range_z_min);
            addParameter("range_z_max", range_z_max);
            addParameter("camera_position_x", camera_position_x);
            addParameter("camera_position_y", camera_position_y);
            addParameter("camera_position_z", camera_position_z);
            addParameter("camera_orientation_x", camera_orientation_x);
            addParameter("camera_orientation_y", camera_orientation_y);
            addParameter("camera_orientation_z", camera_orientation_z);
        }

    public:
        Rect roi_;                      /** The image roi where to search for an object, e.g. a parcel */
        VolumeRange volume_bound_;      /** Volume limits where to search for an object. The struct specifes: */
                                        /** range_x: x-coordinates, in millimeters, of Horizontal FOV limits left (along the conveyor width) */
                                        /** range_y: y-coordinates, in millimeters, of Vertical FOV limits (along the travelling direction) */
                                        /** range_z: z-coordinates, in millimeters, of Near and Far clipping planes (perpendicular to the viewing direction) */
        float camera_position_[3];      /** 3D position of the camera relative to the planar object (the conveyor) used as calibration target. */
                                        /** The vector specifies the x, y, and z position  in millimeters, e.g. z is the orthogonal distance between camera center and conveyor plane */
        float camera_orientation_[3];   /** 3D rotation of the camera relative to the planar object (the conveyor) used as calibration target. */
                                        /** The vector specifies the x, y, and z axis about which the camera is rotated, where the magnitude is the rotation angle in degrees. */
    };
}
#endif // CALIB_PARAMETERS_H