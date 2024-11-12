#include "Test.h"

  Test::Test(){
    
  }

    Test::Test(std::string test_id,std::string xyz_File, std::string profile3d_File, std::string calib_File, std::string intrinsic_File,int numberExwcution)
      :_test_id(test_id),_xyz_File(xyz_File),_profile3d_File(profile3d_File),_intrinsic_File(intrinsic_File),_numberExwcution(numberExwcution){
    }
      
    