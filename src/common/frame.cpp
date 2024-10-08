#include "frame.h"
#include <glog/logging.h>

namespace rs
{
    bool ImageFrame::exportToPng(const std::string& fname, const rs::Rect& roi, const std::vector<size_t>& scan_lines, float gamma)
    {
        // Normalize values
        cv::Mat ab_mat;
        convertToMat(ab_mat);
        cv::Mat ab_display(ab_mat.size(), CV_8U);
        cv::normalize(ab_mat, ab_display, 0, 255, cv::NORM_MINMAX, CV_8U);

        // Apply gamma correction
        std::vector<uint8_t> lut(256);
        for (int k = 0; k < 256; k++)
        {
            lut[k] = cv::saturate_cast<uint8_t>(pow((k / 255.0f), gamma) * 255.0f);
        }
        cv::LUT(ab_display, lut, ab_display);

        // Draw geometries
        cv::cvtColor(ab_display, ab_display, cv::COLOR_GRAY2RGB);
        if (roi.width > 0 && roi.height > 0)
        {
            cv::Rect rect(roi.x, roi.y, roi.width, roi.height);
            cv::rectangle(ab_display, rect.tl(), rect.br(), cv::Scalar(0, 255, 0));
            for (auto y : scan_lines)
            {
                cv::Point p1(rect.tl().x, y);
                cv::Point p2(rect.br().x, y);
                cv::line(ab_display, p1, p2, cv::Scalar(0, 0, 255));
            }   
        }

        // Save PNG image
        std::vector<int> compression_params;
        compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
        compression_params.push_back(9);
        bool result = cv::imwrite(fname, ab_display, compression_params);

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////

    std::vector<std::pair<int16_t, int16_t> > XyzFrame::computeMinMaxBound() const
    {
        auto point_count = resolution();
        int16_t* points = (int16_t *)getPoints();
        std::vector<int16_t> min_val(3, std::numeric_limits<int16_t>::max());
        std::vector<int16_t> max_val(3, std::numeric_limits<int16_t>::min());

        for (size_t i = 0; i < point_count; ++i)
        {
            if (pointIsValid(points))
            {
                // lower bounds
                if (points[0] < min_val[0])
                    min_val[0] = points[0];
                if (points[1] < min_val[1])
                    min_val[1] = points[1];
                if (points[2] < min_val[2])
                    min_val[2] = points[2];

                // upper bounds
                if (points[0] > max_val[0])
                    max_val[0] = points[0];
                if (points[1] > max_val[1])
                    max_val[1] = points[1];
                if (points[2] > max_val[2])
                    max_val[2] = points[2];
            }
            points += 3;
        }

        std::vector<std::pair<int16_t, int16_t> > min_max = 
        {
            {min_val[0], max_val[0]},
            {min_val[1], max_val[1]},
            {min_val[2], max_val[2]}
        };

        return min_max;
    }

    std::vector<float> XyzFrame::computeCentroid() const
    {
        auto point_count = resolution();
        int16_t *points = (int16_t *)getPoints();

        std::vector<float> center = {0.f, 0.f, 0.f};
        size_t valid_count = 0;

        for (size_t i = 0; i < point_count; ++i)
        {
            if (pointIsValid(points))
            {
                center[0] += points[0];
                center[1] += points[1];
                center[2] += points[2];
                valid_count++;
            }
            points += 3;
        }

        if (valid_count > 0)
        {
            center[0] /= float(valid_count);
            center[1] /= float(valid_count);
            center[2] /= float(valid_count);
        }

        return center;
    }

    size_t XyzFrame::invalidatePoints(int16_t invalid_z)
    {
        auto point_count = resolution();
        int16_t *points = (int16_t *)getPoints();
        size_t invalid_count = 0;

        for (size_t i = 0; i < point_count; ++i)
        {
            if (points[2] == invalid_z) // point is valid iff z is non zero
            {
                points[0] = std::numeric_limits<int16_t>::min();
                points[1] = std::numeric_limits<int16_t>::min();
                points[2] = std::numeric_limits<int16_t>::min();
                invalid_count++;
            }
            points += 3;
        }

        return (point_count - invalid_count); // number of valid points
    }

    XyzFrame& XyzFrame::translatePoints(const float translation[3], bool relative)
    {
        auto point_count = resolution();
        int16_t *points = (int16_t *)getPoints();

        std::vector<float> t(translation, translation + 3);
        if (!relative)
        {
            std::vector<float> center = computeCentroid();
            t[0] -= center[0];
            t[1] -= center[1];
            t[2] -= center[2];
        }

        for (size_t i = 0; i < point_count; ++i)
        {
            if (pointIsValid(points))
            {
                // p' = p + t
                points[0] += t[0];
                points[1] += t[1];
                points[2] += t[2];
            }
            points += 3;
        }

        return *this;
    }

    XyzFrame& XyzFrame::rotatePoints(const float rotation[9], const float center[3])
    {
        auto point_count = resolution();
        int16_t *points = (int16_t *)getPoints();

        int16_t to_point[3] = {0};
        int16_t from_point[3] = {0};

        for (size_t i = 0; i < point_count; ++i)
        {
            from_point[0] = points[0];
            from_point[1] = points[1];
            from_point[2] = points[2];

            if (pointIsValid(from_point))
            {
                // p' = R * (p - c) + c
                from_point[0] -= center[0];
                from_point[1] -= center[1];
                from_point[2] -= center[2];

                to_point[0] = rotation[0] * from_point[0] + rotation[1] * from_point[1] + rotation[2] * from_point[2];
                to_point[1] = rotation[3] * from_point[0] + rotation[4] * from_point[1] + rotation[5] * from_point[2];
                to_point[2] = rotation[6] * from_point[0] + rotation[7] * from_point[1] + rotation[8] * from_point[2];

                points[0] = to_point[0] + center[0];
                points[1] = to_point[1] + center[1];
                points[2] = to_point[2] + center[2];
            }

            points += 3;
        }

        return *this;
    }

    XyzFrame& XyzFrame::transformPoints(const float rotation[9], const float translation[3])
    {
        auto point_count = resolution();
        int16_t *points = (int16_t *)getPoints();

        int16_t to_point[3] = {0};
        int16_t from_point[3] = {0};
        float tx = translation[0];
        float ty = translation[1];
        float tz = translation[2];

        for (size_t i = 0; i < point_count; ++i)
        {
            from_point[0] = points[0];
            from_point[1] = points[1];
            from_point[2] = points[2];

            if (pointIsValid(from_point))
            {
                // p' = R * p  + t
                to_point[0] = rotation[0] * from_point[0] + rotation[1] * from_point[1] + rotation[2] * from_point[2] + translation[0];
                to_point[1] = rotation[3] * from_point[0] + rotation[4] * from_point[1] + rotation[5] * from_point[2] + translation[1];
                to_point[2] = rotation[6] * from_point[0] + rotation[7] * from_point[1] + rotation[8] * from_point[2] + translation[2];

                points[0] = to_point[0];
                points[1] = -to_point[1]; // new y-axis is pointing in the direction of travel
                points[2] = -to_point[2]; // new z-axis is pointing toward the camera
            }

            points += 3;
        }

        return *this;
    }

    void XyzFrame::findRoiMask(const float vol_lb[3], const float vol_ub[3], cv::Mat& roi_mask)
    {
        auto point_count = resolution();
        int16_t* points = (int16_t*)getPoints();

        // lower bound
        int16_t x_min = static_cast<int16_t>(floor(vol_lb[0]));
        int16_t y_min = static_cast<int16_t>(floor(vol_lb[1]));
        int16_t z_min = static_cast<int16_t>(floor(vol_lb[2]));
        // upper bound
        int16_t x_max = static_cast<int16_t>(ceil(vol_ub[0]));
        int16_t y_max = static_cast<int16_t>(ceil(vol_ub[1]));
        int16_t z_max = static_cast<int16_t>(ceil(vol_ub[2]));

        roi_mask = cv::Mat(height_, width_, CV_8U);
        uchar *roi_ptr = roi_mask.ptr<uchar>();

        // discard points that fall outside bounds
        for (size_t i = 0; i < point_count; ++i)
        {
            int16_t x = points[0];
            int16_t y = points[1];
            int16_t z = points[2];
            *roi_ptr = 0;

            if (z != 0) // point is valid iff z is non zero
            {
                if (y > y_min && y < y_max &&
                    x > x_min && x < x_max &&
                    z > z_min && z < z_max)
                {
                    *roi_ptr = 255;
                }
            }

            roi_ptr++;
            points += 3;
        }
    }

    /**
     * Convert the organized point cloud data to a single stream
     * of 3D coordinates.
     *
     * \param[out] coords - list of (x,y,z) coordinates
     */
    void XyzFrame::getCoordinates(std::vector<float3> &coords) const
    {
        auto point_count = resolution();
        const int16_t* points = getPoints();

        coords.reserve(point_count);

        for (size_t i = 0; i < point_count; ++i)
        {
            auto x = static_cast<float>(points[0]);
            auto y = static_cast<float>(points[1]);
            auto z = static_cast<float>(points[2]);
            coords.push_back({x, y, z});
            points += 3;

        }
        coords.shrink_to_fit();
    }

    void XyzFrame::getCoordinates(std::vector<float> &coords, int idx) const
    {
        auto point_count = resolution();
        const int16_t *points = getPoints();

        coords.reserve(point_count);

        for (size_t i = 0; i < point_count; ++i)
        {
            auto c = static_cast<float>(points[idx]);
            coords.push_back(c);
            points += 3;
        }

        coords.shrink_to_fit();
    }

    void XyzFrame::getX(std::vector<float> &x) const
    {
        getCoordinates(x, 0);
    }

    void XyzFrame::getY(std::vector<float> &y) const
    {
        getCoordinates(y, 1);
    }

    void XyzFrame::getZ(std::vector<float> &z) const
    {
        getCoordinates(z, 2);
    }

    /**
     * Export the point cloud to a PLY file.
     * \param[in] fname - file name of the PLY to be saved
     * \param[in] texture - the optional texture data for the PLY.
     */
    bool XyzFrame::exportToPly(const std::string& fname, const uint16_t* texture, float min_distance)
    {
        std::vector<float3> vertices;
        getCoordinates(vertices);

        std::vector<float3> new_vertices;
        std::vector<std::tuple<uint8_t, uint8_t, uint8_t> > new_tex;

        new_vertices.reserve(vertices.size());
        new_tex.reserve(vertices.size());
        assert(vertices.size());

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            if (vertices[i].z >= min_distance)
            {
                new_vertices.push_back({vertices[i].x, vertices[i].y, vertices[i].z});
                if (texture)
                {
                    auto it = std::max_element(texture, texture + vertices.size());
                    const float max_val = static_cast<float>(*it);
                    uint8_t texture_val = static_cast<uint8_t>(255 * (texture[i] / max_val));
                    auto color = std::make_tuple(texture_val, texture_val, texture_val);
                    new_tex.push_back(color);
                }
            }
        }

        std::ofstream out(fname);
        out << "ply\n";
        out << "format binary_little_endian 1.0\n";
        out << "comment pointcloud saved from RangeSensorLib\n";
        out << "element vertex " << new_vertices.size() << "\n";
        out << "property float" << sizeof(float) * 8 << " x\n";
        out << "property float" << sizeof(float) * 8 << " y\n";
        out << "property float" << sizeof(float) * 8 << " z\n";
        if (texture)
        {
            out << "property uchar red\n";
            out << "property uchar green\n";
            out << "property uchar blue\n";
        }
        out << "element face 0\n";
        out << "property list uchar int vertex_indices\n";
        out << "end_header\n";
        out.close();

        out.open(fname, std::ios_base::app | std::ios_base::binary);
        if (!out.is_open()) // check for invalid input
        {
            LOG(ERROR) << "Could not open or find the file " << fname; // or data type not supported
            return false;
        }

        for (int i = 0; i < new_vertices.size(); ++i)
        {
            // we assume little endian architecture on your device
            out.write(reinterpret_cast<const char *>(&(new_vertices[i].x)), sizeof(float));
            out.write(reinterpret_cast<const char *>(&(new_vertices[i].y)), sizeof(float));
            out.write(reinterpret_cast<const char *>(&(new_vertices[i].z)), sizeof(float));

            if (texture)
            {
                uint8_t x, y, z;
                std::tie(x, y, z) = new_tex[i];
                out.write(reinterpret_cast<const char *>(&x), sizeof(uint8_t));
                out.write(reinterpret_cast<const char *>(&y), sizeof(uint8_t));
                out.write(reinterpret_cast<const char *>(&z), sizeof(uint8_t));
            }
        }

        return true;
    }
}
