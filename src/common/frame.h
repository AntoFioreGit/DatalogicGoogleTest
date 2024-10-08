#ifndef _FRAME_H
#define _FRAME_H

#include <vector>
#include <iostream>
#include <fstream>
#include <numeric> //std::accumulate
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "common/types.h"

namespace rs
{
    class Frame;

    /**
     * Interface for frame filtering functionality
     * */
    class FilterInterface
    {
    public:
        virtual Frame process(const Frame& frame) const = 0;
        virtual ~FilterInterface() = default;
    };

    /*
    * Define a movable but explicitly noncopyable buffer type to hold our frame data
    */
    class Frame
    {
    public:
        Frame() = default;
        //~Frame() = default;

        /** 
         * \brief  Set frame properties allocate frame data with given params.
         * 
         * \param[in] width the frame width in pixels.
         * \param[in] height the frame height in pixels.
         * \param[in] channels the number of channels in the frame.
         * \param[in] depth the frame depth (in bytes per pixel).
         * \param[in] type the frame type.
         * \param[in] data pointer to the user data to be copied in the internal buffer.
         */
        void prepareData(uint32_t width, uint32_t height, uint32_t channels, uint8_t depth)
        {
            // set frame properties
            width_ = width;
            height_ = height;
            channels_ = channels;
            depth_ = depth;
            // allocate data buffer
            buffer_.resize(height * bytesPerLine()); 
        }

        void copyData(uint8_t* data, size_t resolution)
        {
            if (data)
            {
                if (resolution != buffer_.size())
                    return;
                memcpy(buffer_.data(), data, buffer_.size());
            }
        }

        virtual void prepare(uint32_t width, uint32_t height) = 0;

        Frame(const Frame& other) = delete;
        Frame(Frame&& other)
        {
            swap(other);
        }

        Frame& operator=(const Frame& other) = delete;
        Frame& operator=(Frame&& other)
        {
            swap(other);
            return *this;
        }

        /** 
         * \brief Swap the internal frame with another.
         * 
         * \param[in,out] other frame to swap with.
         */
        void swap(Frame& other)
        {
            std::swap(buffer_, other.buffer_); 
            std::swap(width_, other.width_);
            std::swap(height_, other.height_);
            std::swap(channels_, other.channels_);
            std::swap(depth_, other.depth_);
        }

        /**
         * \brief Parenthesis operator check if internal data buffer is valid.
         * 
         * \return bool - true or false.
         */
        operator bool() const 
        { 
            return data() != nullptr; 
        }

        /**
         * \brief Retrieve the data resolution.
         * 
         * \return the number of bytes in the frame.
         */
        size_t bytes() const
        {
            return buffer_.size();
        }

        /**
         * \brief Retrieve the data resolution per line
         * 
         * \return the number of bytes per row or per width.
         */
        size_t bytesPerLine() const
        {
            return (width_ * channels_ * depth_);
        }

        /**
         * \brief Retrieve data pointer.
         * 
         * \return the pointer to the start of the frame data.
         */
        const uint8_t* data() const
        {
            return buffer_.data();
        }

        /**
         * \brief Retrieve the frame resolution.
         * 
         * \return the number of pixels in the frame.
         */
        size_t resolution() const
        {
            return (width_ * height_);
        }
        
        bool hasData() const
        {
            return (resolution() > 0 && bytes() == height_ * bytesPerLine());
        }

        bool isEmpty() const
        {
            return !hasData();
        }

        virtual void convertToMat(cv::Mat& mat) = 0;
        
        inline bool isSameAs(const Frame& other)
        {
            return (width_ == other.width_ && height_ == other.height_ &&
                    channels_ == other.channels_ &&  depth_ == other.depth_);
        }

        /** 
         * \brief Obtain the pixel given by the (column, row) coordinates.
         * 
         * \param[in] x the column coordinate.
         * \param[in] y the row coordinate.
         */
        template <typename T>
        inline const T& at(std::size_t x, std::size_t y, uint8_t ch = 0) const
        {
            const T* frame_data = reinterpret_cast<const T *>(data());
            const T& pixel = frame_data[y * width_ + x + ch];
            return pixel;
        }

        template <typename T>
        inline T& at(std::size_t x, std::size_t y, uint8_t ch = 0)
        {
            T* frame_data = (T*)reinterpret_cast<const T *>(data());
            T& pixel = frame_data[y * width_ + x + ch];
            return pixel;
        }

        /**
         * \brief Convert a flat index to a pair of row/column indices,
         * assuming row-major order.
         *
         * \param[in] flat_idx flat index.
         * \param[in] num_cols number of columns of referred to matrix.
         * \param[out] row,col row index, column index.
         */
        inline std::pair<size_t, size_t> unravelIndex(size_t flat_idx, size_t num_cols)
        {
            return {flat_idx / num_cols, flat_idx % num_cols};
        }

        /**
         * Template function, checking if current instance is the type of another class.
         * 
         * \return  bool - true or false.
         */
        template <class T>
        bool is() const
        {
            return dynamic_cast<const T*>(this) != nullptr;
        }

        //void loadFromFile(const std::string& filename)
        //{
        //    frame.prepare(frame_height, frame_width, frame_channels, frame_depth);
        //    copyData(frame_data, frame.bytes());
        //}

    public:
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        uint8_t channels_ = 0;
        uint8_t depth_ = 0; // bytes per pixel
        FrameType type_ = FrameType::ANY;
        std::vector<uint8_t> buffer_; // binary data
    };

    /**
     * Extends the frame class with additional grayscale image related attributes and functions.
     */
    class ImageFrame : public Frame
    {
    public:
        ImageFrame() = default;
        ImageFrame(const Frame& frame) { }

        void prepare(uint32_t width, uint32_t height) override
        {
            //type_ = FrameType::AB;
            prepareData(width, height, 1, sizeof(uint16_t));
        }

        void copyPixels(uint16_t* data, size_t count)
        {
            copyData(reinterpret_cast<uint8_t*>(data), count * sizeof(uint16_t));
        }
        
        const uint16_t* getPixels() const
        {
            auto pixels = reinterpret_cast<const uint16_t*>(data());
            return pixels;
        }

        virtual void convertToMat(cv::Mat& mat) override
        {
            mat = cv::Mat(height_, width_, CV_16U, (uint16_t*)getPixels());
        }

        /**
         * \brief Export the image into PNG format.
         * 
         * \param[in] fname - file name of the PNG to be saved.
         * \param[in] roi - draw the rectangle outline corresponding to the frame ROI.
         * \param[in] scan_lines - draw the scan lines used for measuring the object profile.
         * \return returns true on success, false on failure
         */
        bool exportToPng(const std::string& fname, 
                         const rs::Rect& roi,
                         const std::vector<size_t>& scan_lines,
                         float gamma = 0.5);
    };

    /**
     * Extends the frame class with additional 3d image (pointcloud) related attributes and functions.
     */
    class XyzFrame : public Frame
    {
    public:
        XyzFrame() = default;

        XyzFrame(const Frame& frame) { }

        void prepare(uint32_t width, uint32_t height) override
        {
            //type_ = FrameType::XYZ;
            prepareData(width, height, 3, sizeof(int16_t));
        }
        
        void copyPoints(int16_t* data, size_t count)
        {
            copyData(reinterpret_cast<uint8_t *>(data), count * sizeof(uint16_t) * 3);
        }

        const int16_t* getPoints() const
        {
            return reinterpret_cast<const int16_t*>(data());
        }

        virtual void convertToMat(cv::Mat& mat) override
        {
            int16_t *xyz = (int16_t *)getPoints();
            assert(xyz != nullptr);
            mat = cv::Mat(height_, width_, CV_16SC3, xyz);
        }

        std::vector<std::pair<int16_t,int16_t> > computeMinMaxBound() const;
        //std::vector<int16_t> computeMaxBound() const;
        std::vector<float> computeCentroid() const;

        /**
         * \brief Check if the given point is invalid.
        */
        inline bool pointIsValid(int16_t point[3]) const
        {
            return (point[0] != std::numeric_limits<int16_t>::min() &&
                    point[1] != std::numeric_limits<int16_t>::min() &&
                    point[2] != std::numeric_limits<int16_t>::min());
        }

        /**
         * \brief Invalidate points if the z coordinate is equal to the given invalid z value.
         * If a point is invalid, its coordinates will be set to the smallest finite value of the 
         * type usingned short.
         *
         * \param[in] invalid_z The invalid z value, in millimeters.
         */
        size_t invalidatePoints(int16_t invalid_z = 0);

        /**
         * \brief Translate points on x, y, and z axis.
         *
         * \param[in] translation A 3x1 translation vector, in millimeters.
         * \param[in] relative If `true`, the \p translation is directly applied to the
         *  point cloud. Otherwise, the point cloud center is moved to the \p translation.
         */
        XyzFrame& translatePoints(const float translation[3], bool relative);

        /**
         * \brief Rotate points around x, y, and z axis. \p R.
         * 
         * \param[in] rotation A 3x3 rotation matrix.
         * \param[in] center Rotation center that is used for the rotation, in millimeters..
         */
        XyzFrame& rotatePoints(const float rotation[9], const float center[3] = {0});

        /**
         * \brief Transform 3D coordinates relative to one sensor to 3D coordinates relative
         * to another viewpoint.
         *
         * \param[in] rotation A 3x3 rotation matrix.
         * \param[in] translation A 3x1 translation vector, in millimeters.
         */
        XyzFrame& transformPoints(const float rotation[9], const float translation[3]);

        /**
         * \brief Find roi mask using only the points inside the query crop.
         *
         * \param[in] vol_lb A 3x1 array for the lower bound of the volume to be processed.
         * \param[in] vol_ub A 3x1 array for the upper bound of the volume to be processed.
         * \param[out] roi_mask A binary mask of invalid (0) and valid (255) data points.
         */
        void findRoiMask(const float vol_lb[3], const float vol_ub[3], cv::Mat& roi_mask);

        /**
         * \brief Convert the organized point cloud data to a single stream
         * of 3D coordinates.
         *
         * \param[out] coords - list of (x,y,z) coordinates.
         */
        void getCoordinates(std::vector<float3>& coords) const;
        void getCoordinates(std::vector<float>& coords, int idx) const;
        void getX(std::vector<float>& x) const;
        void getY(std::vector<float>& y) const;
        void getZ(std::vector<float>& z) const;

        /**
         * \brief Export the point cloud to a PLY file.
         * 
         * \param[in] fname - file name of the PLY to be saved.
         * \param[in] texture - the optional texture data for the PLY.
         * \return returns true on success, false on failure
         */
        bool exportToPly(const std::string& fname, const uint16_t* texture = nullptr, 
                         float min_distance = 10.f);
    };
}
#endif // _FRAME_H