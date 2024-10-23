#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include "CommonTest.h"
#include "camera_parameters.h"
#include "ccb.h"
extern int block_id_matches(uint8_t ccb_block_id, block_t block_id);
enum index_kind_t
{
     ALL,
     BLOCK_ID
};
struct block__t
{
     enum index_kind_t kind;
     uint8_t block_id;
};
typedef const struct block__t *const block_t;
class CcbTest : public ::testing::Test
{
protected:
     void SetUp() override {}
     void TearDown() override {}
};
void setHeaderBlk3(CAL_HEADER_BLOCK_V3 &header)
{
     header.BlockInfo.BlockID = 1;
     header.BlockInfo.BlockVersion = 2;
     header.BlockInfo.BlockCalibVer = 3;
     header.BlockInfo.BlockSize = 10;
     header.BlockInfo.CheckSum = 34;
     header.BlockInfo.CalibrationYear = 15;
     header.BlockInfo.CalibrationMonth = 45;
     header.BlockInfo.CalibrationDay = 12;
     header.BlockInfo.CalibrationHour = 6;
     header.BlockInfo.CalibrationMinute = 56;
     header.BlockInfo.CalibrationSecond = 22;
     header.BlockInfo.CalibrationPass = 67;
     header.nBlocks = 45;
     header.ChipID = 100;
     header.nRows = 321;
     header.nCols = 123;
     memset(header.ConfigVersionStr, 0, sizeof(header.ConfigVersionStr));
     memset(header.SerialNumber, 0, sizeof(header.SerialNumber));
     memset(header.ChipUniqueID, 0, sizeof(header.ChipUniqueID));

     strcpy(header.ConfigVersionStr, "Version 1.0");
     strcpy(header.SerialNumber, "Serial 123");
     strcpy(header.ChipUniqueID, "IMX 8.0");
     header.ControlAPIVersion = 6677;
     header.FirmwareVersion = 33;
     header.CalibrationVersion = 99;
     header.SequenceVersion = 1011;
}
TEST_F(CcbTest, ccb_read_header)
{

     LOG(INFO) << "CameraParameterTest test getParameter  begin";

     CAL_FILE_HEADER_V1 header;
     header.CalibFileHdrSize = 1;
     header.CalibFileSize = 10;
     header.CalibFileVersion = 5;
     header.CalibHdrSize = 13;
     header.CalibVersion = 1;
     header.rsvd1 = 12;
     header.rsvd = 8;

     const unsigned char *const pheader = reinterpret_cast<const unsigned char *const>(&header);
     ccb_data_t ccbData{pheader, sizeof(struct CAL_FILE_HEADER_V1) + header.CalibFileSize};

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
     ccb_data_t ccbData1{pheader, 0};
     ret = ccb_read_header(&ccbData1);
     EXPECT_TRUE(ret == nullptr);

     LOG(INFO) << "CcbTest test getParameter  end";
}
TEST_F(CcbTest, ccb_read_header_block)
{

     LOG(INFO) << "CameraParameterTest test ccb_read_header_block  begin";

     CAL_HEADER_BLOCK_V3 header;
     setHeaderBlk3(header);

     // header.BlockInfo.BlockID = 1;
     // header.BlockInfo.BlockVersion = 2;
     // header.BlockInfo.BlockCalibVer = 3;
     // header.BlockInfo.BlockSize = 10;
     // header.BlockInfo.CheckSum = 34;
     // header.BlockInfo.CalibrationYear = 15;
     // header.BlockInfo.CalibrationMonth = 45;
     // header.BlockInfo.CalibrationDay = 12;
     // header.BlockInfo.CalibrationHour = 6;
     // header.BlockInfo.CalibrationMinute = 56;
     // header.BlockInfo.CalibrationSecond = 22;
     // header.BlockInfo.CalibrationPass = 67;
     // header.nBlocks = 45;
     // header.ChipID = 100;
     // header.nRows = 321;
     // header.nCols = 123;
     // memset(header.ConfigVersionStr, 0, sizeof(header.ConfigVersionStr));
     // memset(header.SerialNumber, 0, sizeof(header.SerialNumber));
     // memset(header.ChipUniqueID, 0, sizeof(header.ChipUniqueID));

     // strcpy(header.ConfigVersionStr, "Version 1.0");
     // strcpy(header.SerialNumber, "Serial 123");
     // strcpy(header.ChipUniqueID, "IMX 8.0");
     // header.ControlAPIVersion = 6677;
     // header.FirmwareVersion = 33;
     // header.CalibrationVersion = 99;
     // header.SequenceVersion = 1011;
     char data[sizeof(CAL_FILE_HEADER_V1) + sizeof(CAL_HEADER_BLOCK_V3)];
     memcpy(data + sizeof(CAL_FILE_HEADER_V1), &header, sizeof(CAL_HEADER_BLOCK_V3));

     const unsigned char *const nwedata = reinterpret_cast<const unsigned char *const>(data);
     ccb_data_t ccbData{nwedata, sizeof(struct CAL_HEADER_BLOCK_V3) + (header.nRows * header.nCols)};

     auto ret = ccb_read_header_block(&ccbData);
     EXPECT_TRUE(ret != nullptr);
     bool expected = true;
     bool result = true;
     if (ret != nullptr)
     {
          EXPECT_EQ(ret->BlockInfo.BlockID, header.BlockInfo.BlockID);
          EXPECT_EQ(ret->BlockInfo.BlockVersion, header.BlockInfo.BlockVersion);
          EXPECT_EQ(ret->BlockInfo.BlockCalibVer, header.BlockInfo.BlockCalibVer);
          EXPECT_EQ(ret->BlockInfo.BlockSize, header.BlockInfo.BlockSize);
          EXPECT_EQ(ret->BlockInfo.CheckSum, header.BlockInfo.CheckSum);
          EXPECT_EQ(ret->BlockInfo.CalibrationYear, header.BlockInfo.CalibrationYear);
          EXPECT_EQ(ret->BlockInfo.CalibrationMonth, header.BlockInfo.CalibrationMonth);
          EXPECT_EQ(ret->BlockInfo.CalibrationDay, header.BlockInfo.CalibrationDay);
          EXPECT_EQ(ret->BlockInfo.CalibrationHour, header.BlockInfo.CalibrationHour);
          EXPECT_EQ(ret->BlockInfo.CalibrationMinute, header.BlockInfo.CalibrationMinute);
          EXPECT_EQ(ret->BlockInfo.CalibrationSecond, header.BlockInfo.CalibrationSecond);
          EXPECT_EQ(ret->BlockInfo.CalibrationPass, header.BlockInfo.CalibrationPass);
          EXPECT_EQ(ret->nBlocks, header.nBlocks);
          EXPECT_EQ(ret->ChipID, header.ChipID);
          EXPECT_EQ(ret->nRows, header.nRows);
          EXPECT_EQ(ret->nCols, header.nCols);
          result = strcmp(ret->ConfigVersionStr, header.ConfigVersionStr) == 0 ? true : false;
          EXPECT_EQ(result, expected);
          result = strcmp(ret->SerialNumber, header.SerialNumber) == 0 ? true : false;
          EXPECT_EQ(result, expected);
          result = strcmp(ret->ChipUniqueID, header.ChipUniqueID) == 0 ? true : false;
          EXPECT_EQ(result, expected);

          expected = false;
          strcpy(header.ConfigVersionStr, "Version 1.0YY");
          strcpy(header.SerialNumber, "Serial 123ZZ");
          strcpy(header.ChipUniqueID, "IMX 8.0XX");

          result = strcmp(ret->ConfigVersionStr, header.ConfigVersionStr) == 0 ? true : false;
          EXPECT_EQ(result, expected);
          result = strcmp(ret->SerialNumber, header.SerialNumber) == 0 ? true : false;
          EXPECT_EQ(result, expected);
          result = strcmp(ret->ChipUniqueID, header.ChipUniqueID) == 0 ? true : false;
          EXPECT_EQ(result, expected);

          EXPECT_EQ(ret->ControlAPIVersion, header.ControlAPIVersion);
          EXPECT_EQ(ret->FirmwareVersion, header.FirmwareVersion);
          EXPECT_EQ(ret->CalibrationVersion, header.CalibrationVersion);
          EXPECT_EQ(ret->SequenceVersion, header.SequenceVersion);
     }
     ccb_data_t ccbData1{nwedata, 0};
     ret = ccb_read_header_block(&ccbData1);
     EXPECT_TRUE(ret == nullptr);

     LOG(INFO) << "CcbTest test ccb_read_header_block  end";
}
TEST_F(CcbTest, ccb_get_cal_block_info)
{

     LOG(INFO) << "CameraParameterTest test ccb_read_header_block  begin";

     CAL_HEADER_BLOCK_V3 header;
     setHeaderBlk3(header);

     char data[sizeof(CAL_FILE_HEADER_V1) +
               sizeof(CAL_HEADER_BLOCK_V3) +
               sizeof(CAL_BLOCK_INFO_V1)];
     memset(data, 0, sizeof(data));

     block_t block_id = new block__t{ALL, '0'};
     CAL_BLOCK_INFO_V1 cal_blk_info_v1;
     cal_blk_info_v1.BlockID = ALL;
     cal_blk_info_v1.BlockSize = 100;
     cal_blk_info_v1.BlockCalibVer = 1;
     cal_blk_info_v1.BlockVersion = 12;
     cal_blk_info_v1.CalibrationDay = 1;
     cal_blk_info_v1.CalibrationHour = 2;
     cal_blk_info_v1.CalibrationMinute = 3;
     cal_blk_info_v1.CalibrationMonth = 12;
     cal_blk_info_v1.CalibrationPass = 0;
     cal_blk_info_v1.CalibrationSecond = 56;
     cal_blk_info_v1.CalibrationYear = 24;
     cal_blk_info_v1.CheckSum = 88;

     int offset = sizeof(CAL_FILE_HEADER_V1);
     memcpy(data + offset, &header, sizeof(CAL_HEADER_BLOCK_V3));
     offset += sizeof(CAL_HEADER_BLOCK_V3);

     memcpy(data + offset, &cal_blk_info_v1, sizeof(CAL_BLOCK_INFO_V1));

     const unsigned char *const nwedata = reinterpret_cast<const unsigned char *const>(data);
     ccb_data_t ccbData{nwedata, sizeof(struct CAL_HEADER_BLOCK_V3) + (header.nRows * header.nCols)};
     size_t index = 0;

     auto ret = ccb_get_cal_block_info(&ccbData, block_id, index);
     EXPECT_TRUE(ret != nullptr);
     bool expected = true;
     bool result = true;
     if (ret != nullptr)
     {
          EXPECT_EQ(ret->BlockSize, cal_blk_info_v1.BlockSize);
          EXPECT_EQ(ret->BlockCalibVer, cal_blk_info_v1.BlockCalibVer);
          EXPECT_EQ(ret->BlockID, cal_blk_info_v1.BlockID);
          EXPECT_EQ(ret->BlockID, cal_blk_info_v1.BlockID);
          EXPECT_EQ(ret->BlockVersion, cal_blk_info_v1.BlockVersion);
          EXPECT_EQ(ret->CalibrationDay, cal_blk_info_v1.CalibrationDay);
          EXPECT_EQ(ret->CalibrationHour, cal_blk_info_v1.CalibrationHour);
          EXPECT_EQ(ret->CalibrationMinute, cal_blk_info_v1.CalibrationMinute);
          EXPECT_EQ(ret->CalibrationMonth, cal_blk_info_v1.CalibrationMonth);
          EXPECT_EQ(ret->CalibrationPass, cal_blk_info_v1.CalibrationPass);
          EXPECT_EQ(ret->CalibrationSecond, cal_blk_info_v1.CalibrationSecond);
          EXPECT_EQ(ret->CalibrationYear, cal_blk_info_v1.CalibrationYear);
          EXPECT_EQ(ret->CheckSum, cal_blk_info_v1.CheckSum);
     }
     index = 10;
     ret = ccb_get_cal_block_info(&ccbData, block_id, index);
     EXPECT_TRUE(ret == nullptr);
     index = 0;
     ccb_data_t ccbData1{nwedata, 0};
     ret = ccb_get_cal_block_info(&ccbData1, block_id, index);
     EXPECT_TRUE(ret == nullptr);

     LOG(INFO) << "CcbTest test ccb_read_header_block  end";
}
