#include <iostream>
#ifndef Test_h
#define Test_h
class Test {
    public:
    Test();
     Test(std::string test_id,std::string type,std::string xyz_File, std::string profile3d_File, std::string calib_File, std::string intrinsic_File,int numberExecution);

    std::string testId() const { return _test_id; }
    void setTestId(const std::string &test_id) { _test_id = test_id; }

    std::string xyzFile() const { return _xyz_File; }
    void setXyzFile(const std::string &xyz_File) { _xyz_File = xyz_File; }

    std::string calibFile() const { return _calib_File; }
    void setCalibFile(const std::string &calib_File) { _calib_File = calib_File; }

    std::string intrinsicFile() const { return _intrinsic_File; }
    void setIntrinsicFile(const std::string &intrinsic_File) { _intrinsic_File = intrinsic_File; }

    std::string rsAlgoConfFile() const { return _rsAlgoConf_File; }
    void setRsAlgoConfFile(const std::string &rsAlgoConf_File) { _rsAlgoConf_File = rsAlgoConf_File; }

    std::string type() const { return _type; }
    void setType(const std::string &type) { _type = type; }

    int numberExcution() const { return _numberExecution; }
    void setNumberExcution(int numberExecution) { _numberExecution = numberExecution; }

    private:

    std::string _test_id;
    std::string _type;
    std::string  _xyz_File;
    std::string _rsAlgoConf_File;
    std::string _calib_File;
    std::string _intrinsic_File;
    int _numberExecution;
};
#endif