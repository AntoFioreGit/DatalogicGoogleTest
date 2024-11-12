#include <iostream>
class Test {
    public:
    Test();
     Test(std::string test_id,std::string xyz_File, std::string profile3d_File, std::string calib_File, std::string intrinsic_File,int numberExwcution);

    std::string testId() const { return _test_id; }
    void setTestId(const std::string &test_id) { _test_id = test_id; }

    std::string xyzFile() const { return _xyz_File; }
    void setXyzFile(const std::string &xyz_File) { _xyz_File = xyz_File; }

    std::string profile3dFile() const { return _profile3d_File; }
    void setProfile3dFile(const std::string &profile3d_File) { _profile3d_File = profile3d_File; }

    std::string calibFile() const { return _calib_File; }
    void setCalibFile(const std::string &calib_File) { _calib_File = calib_File; }

    std::string intrinsicFile() const { return _intrinsic_File; }
    void setIntrinsicFile(const std::string &intrinsic_File) { _intrinsic_File = intrinsic_File; }

    int numberExwcution() const { return _numberExwcution; }
    void setNumberExwcution(int numberExwcution) { _numberExwcution = numberExwcution; }

    private:

    std::string _test_id;
    std::string  _xyz_File;
    std::string _profile3d_File;
    std::string _calib_File;
    std::string _intrinsic_File;
    int _numberExwcution;
};