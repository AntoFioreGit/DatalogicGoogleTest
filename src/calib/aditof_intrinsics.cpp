#include "aditof_intrinsics.h"
#include "ccb/ccb.h"

#include <fstream>
#include <vector>
#include <glog/logging.h>

namespace rs
{
        int MapCcbGeometricsCameraInstrinsics(const struct CAL_GEOMETRIC_BLOCK_V3* geometrics,
                                              CameraIntrinsics* camera_intrinsics)
        {
            if (geometrics != NULL && camera_intrinsics != NULL)
            {
                camera_intrinsics->fx = geometrics->Fc1;
                camera_intrinsics->fy = geometrics->Fc2;
                camera_intrinsics->cx = geometrics->cc1;
                camera_intrinsics->cy = geometrics->cc2;
                camera_intrinsics->k1 = geometrics->Kc1;
                camera_intrinsics->k2 = geometrics->Kc2;
                camera_intrinsics->k3 = geometrics->Kc3;
                camera_intrinsics->k4 = geometrics->Kc4;
                camera_intrinsics->k5 = geometrics->Kc5;
                camera_intrinsics->k6 = geometrics->Kc6;
                camera_intrinsics->p2 = geometrics->Tx;
                camera_intrinsics->p1 = geometrics->Ty;
                camera_intrinsics->codx = geometrics->Cx;
                camera_intrinsics->cody = geometrics->Cy;
                return 0;
            }
            else
            {
                return -1;
            }
        }

        int GetCameraIntrinsics(FileData* ccb_data, TofiCCBData* p_ccb_data, uint16_t mode)
        {
            if (p_ccb_data == NULL || ccb_data == NULL)
                return -1;

            int status = 0;

            ccb_mode_data p0_block = ccb_get_mode_block_p0((ccb_data_t*)ccb_data, mode);
            if (p0_block.no_of_blocks == 0 ||
                p0_block.p_block_list_head->block_node == NULL)
            {
                return -1;
            }

            if (p0_block.no_of_blocks != 0 &&
                p0_block.p_block_list_head->block_node != NULL)
            {
                mode_block_list* p_block_node = p0_block.p_block_list_head;
                const struct CAL_P0BLOCK_V4 *p_p0_block =
                    (const struct CAL_P0BLOCK_V4 *)p0_block.p_block_list_head->block_node;

                p_ccb_data->n_freqs = (uint8_t)p0_block.no_of_blocks;
                p_ccb_data->n_rows = p_p0_block->nRows;
                p_ccb_data->n_cols = p_p0_block->nCols;
                p_ccb_data->row_bin_factor = p_p0_block->DigitalBinRows * p_p0_block->AnalogBinRows;
                p_ccb_data->col_bin_factor = p_p0_block->DigitalBinCols * p_p0_block->AnalogBinCols;
                p_ccb_data->n_offset_rows = p_p0_block->OffsetRows;
                p_ccb_data->n_offset_cols = p_p0_block->OffsetCols;

                mode_block_list *temp_node = p_block_node;
                p_block_node = (mode_block_list*)p_block_node->prev;
                if (p_block_node != NULL)
                    p_p0_block = (const struct CAL_P0BLOCK_V4 *)p_block_node->block_node;
                free(temp_node);
                free(p_block_node);

                const struct CAL_HEADER_BLOCK_V3* header_block =
                    ccb_read_header_block((const ccb_data_t*)ccb_data);
                p_ccb_data->n_sensor_rows = header_block ? header_block->nRows : 1024;
                p_ccb_data->n_sensor_cols = header_block ? header_block->nCols : 1024;

                // Read geometric intrinsics
                const struct CAL_GEOMETRIC_BLOCK_V3 *geometric =
                    ccb_get_cal_block_geometric((ccb_data_t*)ccb_data, 0);
                if (geometric == NULL)
                    return -1;

                status = MapCcbGeometricsCameraInstrinsics(geometric, &(p_ccb_data->camera_intrinsics));
                if (status != 0)
                    return status;
            }

            return status;
        }
        
        bool ReadCCB(const std::string& filename, TofiCCBData* p_ccb_data, uint16_t mode)
        {
#if 1
            FILE *ccb_file = fopen(filename.c_str(), "rb");

            size_t size = 0;
            if (0 == fseek(ccb_file, 0L, SEEK_END))
            {
                size = ftell(ccb_file);
                rewind(ccb_file);
            }

            uint8_t *ccb_file_buff = (uint8_t *)malloc(size);
            size_t fret = fread(ccb_file_buff, size, 1, ccb_file);
            if (fret != 1)
            {
                printf("\n Failed to read the ccb file");
                return 1;
            }

            FileData ccb_file_data = {(unsigned char *)ccb_file_buff, size};
#else
            std::ifstream ifs(filename);
            if (!ifs.is_open())
            {
                // file path is empty or does not exist
                LOG(ERROR) << "Failed to open: " << filename;
                return false;
            }

            // get length of file
            ifs.seekg(0, ifs.end);
            size_t size = size_t(ifs.tellg());
            ifs.seekg(0, ifs.beg);

            // read data as a block:
            std::vector<char> ccb_file_buff(size);
            ifs.read((char *)ccb_file_buff.data(), size);

            if (!ifs)
            {
                LOG(ERROR) << "Failed to read the ccb file: only " << ifs.gcount() << " could be read";
                return false;
            }

            //TofiCCBData tofi_ccb_data;
            FileData ccb_file_data = {(unsigned char *)ccb_file_buff.data(), size};
#endif
            if (GetCameraIntrinsics(&ccb_file_data, p_ccb_data, mode) != 0)
            {
                LOG(ERROR) << "Failed to read camera intrinsics from .ccb file.";
                return false;
            }
            
            if (ccb_file_buff != NULL)
                free(ccb_file_buff);

            return true;
        }
}