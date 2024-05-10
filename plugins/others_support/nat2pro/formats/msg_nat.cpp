#include "common/projection/projs2/proj_json.h"
#include "formats.h"
#include "products/image_products.h"
#include "common/repack.h"
#include "logger.h"
#include "common/image/io.h"

namespace nat2pro
{
    inline int get_i4(uint8_t *buff)
    {
        // if (isbig) return *((int_4 *) buff);
        uint8_t i4[4];
        i4[0] = buff[3];
        i4[1] = buff[2];
        i4[2] = buff[1];
        i4[3] = buff[0];
        return *((int *)i4);
    }

    void decodeMSGNat(std::vector<uint8_t> msg_file, std::string pro_output_file)
    {
        uint8_t *buf = msg_file.data();

        // Parse header
        std::string mh_strs[48];
        mh_strs[0] = (char *)buf;
        mh_strs[1] = (char *)buf + 80;
        mh_strs[2] = (char *)buf + 160;
        mh_strs[3] = (char *)buf + 240;
        mh_strs[4] = (char *)buf + 320;
        mh_strs[5] = (char *)buf + 400;
        mh_strs[6] = (char *)buf + 480;
        mh_strs[6] += (char *)buf + 526;
        mh_strs[7] = (char *)buf + 542;
        mh_strs[7] += (char *)buf + 588;
        mh_strs[8] = (char *)buf + 604;
        mh_strs[8] += (char *)buf + 650;
        mh_strs[9] = (char *)buf + 666;
        mh_strs[9] += (char *)buf + 712;
        mh_strs[10] = (char *)buf + 728;
        mh_strs[10] += (char *)buf + 774;
        mh_strs[11] = (char *)buf + 2154;
        mh_strs[12] = (char *)buf + 2234;
        mh_strs[13] = (char *)buf + 2314;
        mh_strs[14] = (char *)buf + 2394;
        mh_strs[15] = (char *)buf + 2474;
        mh_strs[16] = (char *)buf + 2554;
        mh_strs[17] = (char *)buf + 2634;
        mh_strs[18] = (char *)buf + 2714;
        mh_strs[19] = (char *)buf + 2794;
        mh_strs[20] = (char *)buf + 2874;
        mh_strs[21] = (char *)buf + 2954;
        mh_strs[22] = (char *)buf + 3034;
        mh_strs[23] = (char *)buf + 3114;
        mh_strs[24] = (char *)buf + 3194;
        mh_strs[25] = (char *)buf + 3274;
        mh_strs[26] = (char *)buf + 3354;
        mh_strs[27] = (char *)buf + 3434;
        mh_strs[28] = (char *)buf + 3514;
        mh_strs[29] = (char *)buf + 3594;
        mh_strs[30] = (char *)buf + 3674;
        mh_strs[31] = (char *)buf + 3754;
        mh_strs[32] = (char *)buf + 3834;
        mh_strs[33] = (char *)buf + 3914;
        mh_strs[34] = (char *)buf + 3994;
        mh_strs[35] = (char *)buf + 4074;
        mh_strs[36] = (char *)buf + 4154;
        mh_strs[37] = (char *)buf + 4234;
        mh_strs[38] = (char *)buf + 4314;
        mh_strs[39] = (char *)buf + 4394;
        mh_strs[40] = (char *)buf + 4474;
        mh_strs[41] = (char *)buf + 4554;
        mh_strs[42] = (char *)buf + 4634;
        mh_strs[43] = (char *)buf + 4714;
        mh_strs[44] = (char *)buf + 4794;
        mh_strs[45] = (char *)buf + 4874;
        mh_strs[46] = (char *)buf + 4954;
        mh_strs[47] = (char *)buf + 5034;

        for (int i = 0; i < 48; i++)
            logger->debug("L%d %s", i, mh_strs[i].substr(0, mh_strs[i].size() - 1).c_str());

        // IMG Size
        int vis_ir_x_size, vis_ir_y_size;
        int hrv_x_size, hrv_y_size;
        float longitude;
        sscanf(mh_strs[44].c_str(), "NumberLinesVISIR            : %d", &vis_ir_y_size);
        sscanf(mh_strs[45].c_str(), "NumberColumnsVISIR          : %d", &vis_ir_x_size);
        sscanf(mh_strs[46].c_str(), "NumberLinesHRV              : %d", &hrv_y_size);
        sscanf(mh_strs[47].c_str(), "NumberColumnsHRV            : %d", &hrv_x_size);
        sscanf(mh_strs[14].c_str(), "LLOS                        : %f", &longitude);

        logger->warn("VIS/IR Size : %dx%d", vis_ir_x_size, vis_ir_y_size);
        logger->warn("HRV    Size : %dx%d", hrv_x_size, hrv_y_size);
        logger->warn("Longitude   : %f", longitude);

        // Other data
        long int headerpos, datapos, trailerpos;
        sscanf(mh_strs[8].c_str(), "%*s : %*d %ld\n", &headerpos);
        sscanf(mh_strs[9].c_str(), "%*s : %*d %ld\n", &datapos);
        sscanf(mh_strs[10].c_str(), "%*s : %*d %ld\n", &trailerpos);

        char bandsel[16];
        sscanf(mh_strs[39].c_str(), "%*s : %s", bandsel);

        image::Image vis_ir_imgs[12];
        for (int channel = 0; channel < 12; channel++)
        {
            if (bandsel[channel] != 'X')
                continue;

            vis_ir_imgs[channel].init(16, (channel == 11 ? hrv_x_size : vis_ir_x_size), (channel == 11 ? hrv_y_size : vis_ir_y_size), 1);
        }

        // Read Trailer
        //{
        uint8_t *trailer_ptr = buf + trailerpos;

        // int trailer_version = trailer_ptr[0];

        int offset = 38 + 1 + 2 +
                     14 +
                     12 +
                     192 +
                     6 * 12 +
                     16;

        int LowerSouthLineActual = get_i4(&trailer_ptr[offset + 0]);
        int LowerNorthLineActual = get_i4(&trailer_ptr[offset + 4]);
        int LowerEastColumnActual = get_i4(&trailer_ptr[offset + 8]);
        int LowerWestColumnActual = get_i4(&trailer_ptr[offset + 12]);

        int UpperSouthLineActual = get_i4(&trailer_ptr[offset + 16]);
        // int UpperNorthLineActual = get_i4(&trailer_ptr[offset + 20]);
        int UpperEastColumnActual = get_i4(&trailer_ptr[offset + 24]);
        // int UpperWestColumnActual = get_i4(&trailer_ptr[offset + 28]);

        logger->critical("LowerSouthLineActual : %d, LowerNorthLineActual : %d, LowerEastColumnActual : %d, LowerWestColumnActual : %d",
                         LowerSouthLineActual, LowerNorthLineActual, LowerEastColumnActual, LowerWestColumnActual);
        // }

        uint16_t repacked_line[81920];

        // Read line data
        // uint8_t pkt_hdr[48];
        uint8_t *data_ptr = buf + datapos;
        for (int line = 0; line < vis_ir_y_size; line++)
        {
            for (int channel = 0; channel < 12; channel++)
            {
                if (bandsel[channel] != 'X')
                    continue;

                for (int i = 0; i < (channel == 11 ? 3 : 1); i++)
                {
                    // uint16_t seq_cnt = (data_ptr + 16)[0] << 8 | (data_ptr + 16)[1];
                    uint32_t pkt_len = (data_ptr + 18)[0] << 24 | (data_ptr + 18)[1] << 16 | (data_ptr + 18)[2] << 8 | (data_ptr + 18)[3];

                    // logger->info("PKT %d CHANNEL %d SEQ %d LEN %d", line, channel, seq_cnt, pkt_len);

                    int datasize = pkt_len - 15 - 27;

                    repackBytesTo10bits(data_ptr + 38 + 27, datasize, repacked_line);

                    if (channel < 11)
                    {
                        for (int x = 0; x < vis_ir_x_size; x++)
                            vis_ir_imgs[channel].set(line * vis_ir_x_size + x, repacked_line[x] << 6);
                    }
                    else
                    {
                        for (int x = 0; x < hrv_x_size; x++)
                        {
                            if (line * 3 + 4 > UpperSouthLineActual)
                            {
                                int xpos = x + UpperEastColumnActual;
                                if (xpos >= 0 && xpos < hrv_x_size)
                                    vis_ir_imgs[channel].set((line * 3 + i) * hrv_x_size + xpos, repacked_line[x] << 6);
                            }
                            else
                            {
                                int xpos = x + LowerEastColumnActual;
                                if (xpos >= 0 && xpos < hrv_x_size)
                                    vis_ir_imgs[channel].set((line * 3 + i) * hrv_x_size + xpos, repacked_line[x] << 6);
                            }
                        }
                    }

                    data_ptr += 38 + 27 + datasize;
                }
            }
        }

        // Saving
        satdump::ImageProducts seviri_products;
        seviri_products.instrument_name = "seviri";
        seviri_products.has_timestamps = false;
        seviri_products.bit_depth = 10;

        proj::projection_t proj;
        proj.type = proj::ProjType_Geos;
        proj.params.sweep_x = false;
        proj.params.altitude = 35785831.00;
        proj.proj_offset_x = -5568748.275756353; // 6378169;
        proj.proj_offset_y = 5568748.275756353;  // 6356583.80;
        proj.proj_scalar_x = 3000.403165817;
        proj.proj_scalar_y = -3000.403165817;
        proj.lam0 = longitude * DEG2RAD;
        nlohmann::json proj_cfg = proj;
        proj_cfg["width"] = 3712;
        proj_cfg["height"] = 3712;
        //        proj_cfg["type"] = "geos";
        //        proj_cfg["lon"] = longitude;
        //        proj_cfg["alt"] = 35792.74;
        //        proj_cfg["scale_x"] = 1.145;
        //        proj_cfg["scale_y"] = 1.145;
        //        proj_cfg["offset_x"] = -0.0005;
        //        proj_cfg["offset_y"] = 0.0005;
        //        proj_cfg["sweep_x"] = false;
        seviri_products.set_proj_cfg(proj_cfg);

        for (int channel = 0; channel < 12; channel++)
        {
            if (bandsel[channel] != 'X')
                continue;
            vis_ir_imgs[channel].mirror(true, true);
            seviri_products.images.push_back({"SEVIRI-" + std::to_string(channel + 1), std::to_string(channel + 1), vis_ir_imgs[channel]});
        }

        if (!std::filesystem::exists(pro_output_file))
            std::filesystem::create_directories(pro_output_file);
        seviri_products.save(pro_output_file);

        // Write composite
        std::string path = "SEVIRI_321";
        image::Image compo_321(16, vis_ir_x_size, vis_ir_y_size, 3);

        compo_321.draw_image(0, vis_ir_imgs[2]);
        compo_321.draw_image(1, vis_ir_imgs[1]);
        compo_321.draw_image(2, vis_ir_imgs[0]);

        logger->info("Saving " + path);
        image::save_png(compo_321, path + ".png");
    }
}
