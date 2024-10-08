#include "profile3d_calculator.h"

#include <numeric> // std::accumulate
#include <atomic>
#include <algorithm>
#include <opencv2/imgproc.hpp>
#include <glog/logging.h>

#include "utils/filesytem_helpers.h"
#include "utils/geometry.h"

#define USE_FIND_ROI

using namespace rs;
using namespace std;

rs::Rect Profile3dCalculator::getRoi(float c[3], float tr[3])
{
    float* intr = camera_params_.camera_matrix_.ptr<float>();
    float* coeffs = camera_params_.distortion_coeffs_.ptr<float>();
    float* orient = camera_params_.rotation_.ptr<float>();
    float tvec[3] = {0, 0, 0};

    cv::Point2f roi_center;
    roi_center = rs::utils::geometry::project3dToPixel(c, orient, tvec, intr, coeffs);

    cv::Point2f roi_bottom_right;
    roi_bottom_right = rs::utils::geometry::project3dToPixel(tr, orient, tvec, intr, coeffs);

    LOG(INFO) << "ROI (center)";
    LOG(INFO) << c[0] << ", " << c[1] << ", " << c[2] << " mm -> "
              << roi_center.x << ", " << roi_center.y << " px";

    float roi_half_width = (roi_bottom_right.x - roi_center.x + 1);
    float roi_half_height = (roi_bottom_right.y - roi_center.y + 1);
    float roi_max_x = camera_params_.image_size_.width;
    float roi_max_y = camera_params_.image_size_.height;

    rs::Rect rect;
    rect.x = int(std::clamp(roi_center.x - roi_half_width, 0.f, roi_max_x));
    rect.y = int(std::clamp(roi_center.y - roi_half_height, 0.f, roi_max_y));
    rect.width = int(std::clamp(2 * roi_half_width, 0.f, roi_max_x));
    rect.height = int(std::clamp(2 * roi_half_height, 0.f, roi_max_y));

    return rect;
}

std::vector<size_t> Profile3dCalculator::getScanlines(float c[3], int anchor_row, size_t n_lines, float spacing)
{
    // Get the scan lines as image rows
    LOG(INFO) << "getScanLines";

    if ((n_lines - 1) % 2 != 0)
    {
        LOG(ERROR) << "Number of scan lines should be odd, n_lines = " << n_lines;
        return {};
    }

    if (spacing < 5) // in mm
    {
        LOG(ERROR) << "line profile spacing cannot be less than xy resolution, spacing = " << spacing;
        return {};
    }

    // Convert line spacing into number of rows
    float* intr = camera_params_.camera_matrix_.ptr<float>();
    float* coeffs = camera_params_.distortion_coeffs_.ptr<float>();
    float* orient = camera_params_.rotation_.ptr<float>();
    float tvec[3] = {0, 0, 0};

    float top[3] = {0, spacing, c[2]};
    cv::Point2f roi_bottom = rs::utils::geometry::project3dToPixel(top, orient, tvec, intr, coeffs);

    size_t k = (n_lines - 1) / 2;
    size_t line_start = anchor_row;
    size_t line_step = size_t(std::clamp(int(roi_bottom.y - anchor_row),
                                         0, camera_params_.image_size_.height));

    LOG(INFO) << "line_step = " << line_step;

    // Get rows from bottom to top, i.e. in the travel direction
    std::vector<size_t> scan_lines;
    size_t n_half_rows = k * line_step;
    size_t row_start = line_start + n_half_rows;
    size_t row_end = line_start - n_half_rows;

    for (int row = row_start; row > int(row_end-1); row -= line_step)
        scan_lines.push_back(row);

    return scan_lines;  
}

bool Profile3dCalculator::initialize(const rs::calib::CameraParameters& camera_params, 
                                     const rs::VolumeRange& volume_bound, 
                                     const rs::Rect& image_roi)
{
    LOG(INFO) << "Initialize Profile3dCalculator";

    // Initialize camera parameters
    camera_params_ = camera_params;
    if (!camera_params_.checkIntrinsics())
    {
        return false;
    }

    // Initialize query volume size
    if (!isValidVolume(volume_bound))
    {
        LOG(ERROR) << "Bad query volume (x, y, and z): upper bound must be greater than lower bound";
        return false;
    }

    query_vol_ = volume_bound;

    // Save conveyor position
    float z_belt = camera_params_.translation_.at<float>(0, 2); // camera is atop of conveyor system
    float belt_center[3] = {0, 0, z_belt};

    // Initialize ROI
    if (isValidRoi(image_roi))
    {
        image_roi_ = image_roi;
    }
    else
    {
        LOG(WARNING) << "Roi not set, find it";
        float belt_top_right[3] = {query_vol_.max.x, query_vol_.max.y, z_belt};
        image_roi_ = getRoi(belt_center, belt_top_right);
    }

    // Get the scan lines as image rows
    int anchor_row = int(image_roi_.y + image_roi_.height / 2.0);
    scan_lines_ = getScanlines(belt_center, anchor_row, params_.num_scanlines_, params_.scaline_spacing_);
    if (scan_lines_.empty())
    {
        LOG(ERROR) << "Empty scan line vector, no profile to extract";
        return false;
    }

    return true;
}

int Profile3dCalculator::findObjects(XyzFrame& xyz_frame,
                                     const float min_size[3], const float max_size[3], 
                                     bool find_calib_target,
                                     cv::Mat& obj_mask)
{
    LOG(INFO) << "Find objects";

    chrono::high_resolution_clock::time_point t1, t2;
    chrono::microseconds elapsed_time;

    if (vlog_level_ > 0)
    {
        t1 = chrono::high_resolution_clock::now();

        vector<pair<int16_t, int16_t> > min_max = xyz_frame.computeMinMaxBound();
        //vector<int16_t> max_val = xyz_frame.computeMaxBound();

        VLOG(DEBUG) << "Min X, Max X = " << min_max[0].first << ", " << min_max[0].second;
        VLOG(DEBUG) << "Min Y, Max Y = " << min_max[1].first << ", " << min_max[1].second;
        VLOG(DEBUG) << "Min Z, Max Z = " << min_max[2].first << ", " << min_max[2].second;

        t2 = chrono::high_resolution_clock::now();
        elapsed_time = chrono::duration_cast<chrono::microseconds>(t2 - t1);
        VLOG(DEBUG) << "Execution time (minmax): " << elapsed_time.count() << "us";
    }

#ifdef USE_FIND_ROI
    // Mask out points outside the volume bound
    t1 = chrono::high_resolution_clock::now();

    float vol_lb[3] = {query_vol_.min.x, query_vol_.min.y, query_vol_.min.z};
    float vol_ub[3] = {query_vol_.max.x, query_vol_.max.y, query_vol_.max.z};
    if (!find_calib_target)
    {
        vol_lb[2] = .8f * min_size[2]; // min obj height
        vol_ub[2] = 1.2f * max_size[2]; // max obj height
    }

    xyz_frame.findRoiMask(vol_lb, vol_ub, obj_mask);
    if (params_.static_roi_)
    {
        // Image roi is fixed for all frames, useful for internal testing
        bool valid_roi = !(image_roi_.x == 0 && image_roi_.y == 0 && image_roi_.width == 0 && image_roi_.height == 0);
        if (valid_roi)
        {
            // Select only mask pixels in the ROI
            cv::Mat input_mask = cv::Mat(obj_mask.size(), CV_8U, cv::Scalar::all(0));
            cv::Rect rect(image_roi_.x, image_roi_.y, image_roi_.width, image_roi_.height);
            input_mask(rect) = 255;
            cv::bitwise_and(obj_mask, input_mask, obj_mask);
        }
        else
        {
            // Something went wrong
            LOG(ERROR) << "Invalid number of elements to create a rect.";
            return -1;
        }
    }

    t2 = chrono::high_resolution_clock::now();
    elapsed_time = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    LOG(INFO) << "Execution time (findRoiMask): " << elapsed_time.count() << "us";
#else
    // Extract a single-channel image for each coordinate
    t1 = chrono::high_resolution_clock::now();

    cv::Mat xyz_mat;
    vector<cv::Mat> xyz;
    input_frame_.convertToMat(xyz_mat);
    cv::split(xyz_mat, xyz);

    t2 = chrono::high_resolution_clock::now();
    elapsed_time = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    LOG(INFO) << "Execution time (split): " << elapsed_time.count() << "us";

    // Mask out points outside the vertical ROI
    t1 = chrono::high_resolution_clock::now();

    cv::Mat& x = xyz[0];
    cv::Mat& y = xyz[1];
    cv::Mat& z = xyz[2];

    cv::Mat valid_x = x > query_vol_[0][0] & x < query_vol_[0][1];
    cv::Mat valid_y = y > query_vol_[1][0] & y < query_vol_[1][1];
    cv::Mat roi_mask = (valid_x & valid_y);

    t2 = chrono::high_resolution_clock::now();
    elapsed_time = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    LOG(INFO) << "Execution time (roi_mask): " << elapsed_time.count() << "us";

    // Mask out points below the min object height
    t1 = chrono::high_resolution_clock::now();

    int16_t min_size_z = int16_t(.8f * min_size[2]);
    cv::Mat z_mask = z > min_size_z;

    t2 = chrono::high_resolution_clock::now();
    elapsed_time = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    LOG(INFO) << "Execution time (z_mask): " << elapsed_time.count() << "us";

    obj_mask = z_mask & roi_mask;
#endif
    // Find the largest blob
    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    
    t1 = chrono::high_resolution_clock::now();

    cv::erode(obj_mask, obj_mask, cv::Mat());
    int nb_cc = cv::connectedComponentsWithStats(obj_mask, labels, stats, centroids, 4, CV_16U, cv::CCL_WU);

    int max_idx = -1;
    int max_cc_size = 0;
    for (int i = 1; i < nb_cc; ++i) // 0 is the background label
    {
        int cc_size = stats.at<int>(i, cv::CC_STAT_AREA);
        if (cc_size > max_cc_size)
        {
            max_cc_size = cc_size;
            max_idx = i;
        }
    }
    
    obj_mask = labels == max_idx;
    LOG(INFO) << "nb_cc: " << nb_cc << ", max_idx: " << max_idx << ", max_cc_size: " << max_cc_size;

    t2 = chrono::high_resolution_clock::now();
    elapsed_time = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    LOG(INFO) << "Execution time (connectedComponents): " << elapsed_time.count() << "us";

    return max_idx > 0;
}

void Profile3dCalculator::lineProfile(XyzFrame& xyz_frame, 
                                      const cv::Mat& obj_mask, 
                                      int scan_line, int approx_func,
                                      vector<float> &coords)
{
    LOG(INFO) << "Profile of scan line " << scan_line;

    chrono::high_resolution_clock::time_point t1, t2;
    chrono::microseconds elapsed_time;

    t1 = chrono::high_resolution_clock::now();

    auto row_mask_ptr = obj_mask.row(scan_line).ptr<uint8_t>();
    vector<uint8_t> row_mask(row_mask_ptr, row_mask_ptr + obj_mask.cols);
    vector<uint8_t>::iterator it;
    vector<uint8_t>::reverse_iterator rit;

    int col_start = -1;
    it = find(row_mask.begin(), row_mask.end(), 255);
    if (it != row_mask.end())
    {
        col_start = std::distance(row_mask.begin(), it) + 1;
    }

    int col_end = -1;
    rit = find(row_mask.rbegin(), row_mask.rend(), 255);
    if (rit != row_mask.rend())
    {
        col_end = std::distance(rit, row_mask.rend()) - 1;
    }

    LOG(INFO) << "col start, col_end = " << col_start << ", " << col_end;

    int num_control_points;
    string approx_method;
    if (approx_func == 0)
    {
        approx_method = "Rect";
        num_control_points = 2;
    }
    else
    {
        approx_method = "Poly";
        num_control_points = 5;
    }

    coords = vector<float>(3 * num_control_points, 0);
    if (col_start < 0 || col_end < 0 || col_end - col_start < 2)
        return; // return a zero profile vector
    
    if (approx_func == 0) // The profile is approximated using a rect
    {
        // Two 3D points will be computed: the top-left and top-right corners of the
        // rectangle bounding the object surface.
        cv::Mat xyz_mat;
        xyz_frame.convertToMat(xyz_mat);

        // Compute profile height as the median value of non zero heights along the scan line
        cv::Vec3s* row_ptr = xyz_mat.row(scan_line).ptr<cv::Vec3s>();
        vector<int16_t> heights;

        heights.reserve(row_mask.size());
        for (size_t i = col_start; i < col_end; i++)
        {
            int16_t z = row_ptr[i][2];
            if (z != 0)
                heights.push_back(z);
        }
        heights.shrink_to_fit();

        auto m = heights.begin() + heights.size() / 2;
        nth_element(heights.begin(), m, heights.end());
        auto pz = heights[heights.size() / 2];

        // Compute vertical position as the mean value of y coordinates (TODO: interpolate?)
        int16_t py = floor((row_ptr[col_start][1] + row_ptr[col_end][1]) / 2.f);

        // Compute lateral distance from the outer edges of the conveyor
        auto dr2l = query_vol_.max.x - row_ptr[col_start][0]; // from right edge to left corner
        auto dl2r = row_ptr[col_end][0] - query_vol_.min.x; // from left edge to right corner

        LOG(INFO) << "x left, x right = " << row_ptr[col_start][0] << ", " << row_ptr[col_end][0];

        // Top-left rect corner
        coords[0] = dr2l;
        coords[1] = py;
        coords[2] = pz;

        // Top-right rect corner
        coords[3] = dl2r;
        coords[4] = py;
        coords[5] = pz;
    }
    else // The profile is approximated using a polyline
    {
        LOG(ERROR) << "Approximation function not implemented";
    }

    for (size_t i = 0; i < coords.size(); i+=3)
    {
        LOG(INFO) << approx_method << " Point " << (i / 3 + 1) << ": "
                  << coords[i] << ", " << coords[i + 1] << ", " << coords[i + 2];
    }

    t2 = chrono::high_resolution_clock::now();
    elapsed_time = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    LOG(INFO) << "Execution time (lineProfile): " << elapsed_time.count() << "us";
}

int Profile3dCalculator::findProfiles(rs::XyzFrame& xyz_frame)
{
    profiles_.clear();

    chrono::high_resolution_clock::time_point t1, t2;
    chrono::microseconds elapsed_time;

    // Set new z "0" on the conveyor plane
    float tvec[3] = {0.f, 0.f, -camera_params_.translation_.at<float>(0,2)};
    // Compensate camera angles
    cv::Mat Rt = camera_params_.rotation_.t();
    float* rmat = Rt.ptr<float>();
    
    // Apply transformation
    t1 = chrono::high_resolution_clock::now();

    xyz_frame.invalidatePoints();
    xyz_frame.transformPoints(rmat, tvec);

    t2 = chrono::high_resolution_clock::now();
    elapsed_time = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    LOG(INFO) << "Execution time (transformPoints): " << elapsed_time.count() << "us";
    
    // Find valid objects
    float min_obj_size[3] = {100, 100, 100};
    float max_obj_size[3] = {500, 500, 1000};
    cv::Mat obj_mask;

    int obj_count = findObjects(xyz_frame, min_obj_size, max_obj_size, false, obj_mask);
    LOG(INFO) << "Number of objects: " << obj_count;

    // Find object profiles
    int profile_count = 0;
    int approx_func = 0; // input param defined by the user, now only rect func is supported

    if (obj_count > 0)
    {
        for (auto row : scan_lines_)
        {
            vector<float> coords;
            lineProfile(xyz_frame, obj_mask, row, approx_func, coords);

            if (coords.empty())
                return -1;

            auto sum = std::accumulate(coords.begin(), coords.end(), 0);
            if (sum > 0)
                profile_count++;

            profiles_.push_back(coords); // always send something
        }
    }

    LOG(INFO) << "Number of profiles: " << profile_count;

    return profile_count;
}