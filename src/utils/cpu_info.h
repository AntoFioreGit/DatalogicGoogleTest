#pragma once

#include <memory>

namespace rs 
{
    namespace utils
    {
        /// \brief CPU information.
        class CPUInfo
        {
        public:
            static CPUInfo& GetInstance();

            ~CPUInfo() = default;
            CPUInfo(const CPUInfo&) = delete;
            void operator=(const CPUInfo&) = delete;

            /// Returns the number of physical CPU cores.
            /// This is similar to boost::thread::physical_concurrency().
            int NumCores() const;

            /// Returns the number of logical CPU cores.
            /// This returns the same result as std::thread::hardware_concurrency() or
            /// boost::thread::hardware_concurrency().
            int NumThreads() const;

            /// Prints CPUInfo in the console.
            void Print() const;

        private:
            CPUInfo();
            struct Impl;
            std::unique_ptr<Impl> impl_;
        };
    }
}
