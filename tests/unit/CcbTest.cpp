#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "CommonTest.h"
#include "camera_parameters.h"
#include "ccb.h"
extern int block_id_matches(uint8_t ccb_block_id, block_t block_id);
class CcbTest : public ::testing::Test
{
protected:
     void SetUp() override {}
     void TearDown() override {}
};

TEST_F(CcbTest, ccb_read_header)
{

     LOG(INFO) << "CameraParameterTest test getParameter  begin";
     uint8_t ccb_block_id = 10;
     block_t block_id = nullptr;
     bool result = false;
     bool expected = false;

     CAL_FILE_HEADER_V1 header;
     header.CalibFileHdrSize = 1;
     header.CalibFileSize = 10;
     header.CalibFileVersion = 5;
     header.CalibHdrSize = 13;
     header.CalibVersion = 1;
     header.rsvd1 = 12;
     header.rsvd = 8;

     const unsigned char *const pheader = reinterpret_cast<const unsigned char *const>(&header);
     ccb_data_t ccbData{pheader, 16 + header.CalibFileSize};

     auto ret = ccb_read_header(&ccbData);
     EXPECT_TRUE(ret != nullptr);
     if (ret != nullptr)
     {
          EXPECT_EQ(ret->CalibFileHdrSize, header.CalibFileHdrSize);
          EXPECT_EQ(ret->CalibFileSize, header.CalibFileSize);
          EXPECT_EQ(ret->CalibFileVersion, header.CalibFileVersion);
          EXPECT_EQ(ret->CalibHdrSize, header.CalibHdrSize);
          EXPECT_EQ(ret->rsvd1, header.rsvd1);
          EXPECT_EQ(ret->rsvd, header.rsvd);
     }

     LOG(INFO) << "CcbTest test getParameter  end";
}
