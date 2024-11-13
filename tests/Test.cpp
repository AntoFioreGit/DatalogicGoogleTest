#include "Test.h"

  Test::Test(){
    
  }

    Test::Test(std::string test_id,std::string type,std::string xyz_File, std::string rsAlgoConf_File, std::string calib_File, std::string intrinsic_File,int numberExecution)
      :_test_id(test_id),_type(type),_xyz_File(xyz_File),_rsAlgoConf_File(rsAlgoConf_File),_calib_File(calib_File),_intrinsic_File(intrinsic_File),_numberExecution(numberExecution){
    }
      
    