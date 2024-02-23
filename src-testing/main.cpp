/**********************************************************************
 * This file is used for testing random stuff without running the
 * whole of SatDump, which comes in handy for debugging individual
 * elements before putting them all together in modules...
 *
 * If you are an user, ignore this file which will not be built by
 * default, and if you're a developper in need of doing stuff here...
 * Go ahead!
 *
 * Don't judge the code you might see in there! :)
 **********************************************************************/

#include "logger.h"
#include "common/projection/projs2/proj.h"

#include "common/image/image.h"

#include "common/map/map_drawer.h"

#include "common/image/image_meta.h"
#include "common/projection/projs2/proj_json.h"

int main(int argc, char *argv[])
{
    initLogger();
    completeLoggerInit();

#if 0
    proj::projection_t p;

    p.type = proj::ProjType_Stereographic;
    p.proj_offset_x = 418962.397137703;
    p.proj_offset_y = -101148.834767705;
    p.proj_scalar_x = 60.5849500687633;  // 1;
    p.proj_scalar_y = -60.5849500687633; // 1;
    p.lam0 = 2 * DEG2RAD;
    p.phi0 = 48 * DEG2RAD;

    proj::projection_setup(&p);

    double x = 0; // 15101; // 678108.04;
    double y = 0; // 16495; // 5496954.89;
    double lon, lat;
    proj::projection_perform_inv(&p, x, y, &lon, &lat);

    logger->info("X %f - Y %f", x, y);
    logger->info("Lon %f - Lat %f", lon, lat);

    proj::projection_perform_fwd(&p, lon, lat, &x, &y);

    logger->info("X %f - Y %f", x, y);

    // return 0;

    image::Image<uint16_t> image_test;
    image_test.load_tiff(argv[1]);
    image_test.normalize();
    image_test.mirror(false, true);

    {
        unsigned short color[4] = {0, 65535, 0, 65535};
        map::drawProjectedMapShapefile({"resources/maps/ne_10m_admin_0_countries.shp"},
                                       image_test,
                                       color,
                                       [&p](float lat, float lon, int, int) -> std::pair<int, int>
                                       {
                                           double x, y;
                                           proj::projection_perform_fwd(&p, lon, lat, &x, &y);
                                           return {(int)x, (int)y};
                                       });
    }

    image_test.save_jpeg(argv[2]);
#else

#if 0
    proj::projection_t p;

    p.type = proj::ProjType_UniversalTransverseMercator;
    p.proj_offset_x = 5529681.06869509;
    p.proj_offset_y = 8988883.99412302;
    p.proj_scalar_x = 86.6310171354189;  // 1;
    p.proj_scalar_y = -86.6310171354189; // 1;
    // p.lam0 = 15 * DEG2RAD;  // Zone 33
    //  p.phi0 = 48 * DEG2RAD;

    proj::projection_setup_t params;
    params.zone = 20; // 33;
    proj::projection_setup(&p, &params);

    double x = 0; // 15101; // 678108.04;
    double y = 0; // 16495; // 5496954.89;
    double lon, lat;
    proj::projection_perform_inv(&p, x, y, &lon, &lat);

    logger->info("X %f - Y %f", x, y);
    logger->info("Lon %f - Lat %f", lon, lat);

    proj::projection_perform_fwd(&p, lon, lat, &x, &y);

    logger->info("X %f - Y %f", x, y);

    // return 0;
#endif

    image::Image<uint16_t> image_test;
    image_test.load_tiff(argv[1]);
    image_test.equalize();
    image_test.normalize();
    image_test.mirror(false, true);

    auto jsonp = image::get_metadata(image_test);
    logger->debug("\n%s", jsonp.dump(4).c_str());
    proj::projection_t p = jsonp["proj_cfg"];
    bool v = proj::projection_setup(&p);

    {
        double x = 0; // 15101; // 678108.04;
        double y = 0; // 16495; // 5496954.89;
        double lon, lat;
        proj::projection_perform_inv(&p, x, y, &lon, &lat);

        logger->info("X %f - Y %f", x, y);
        logger->info("Lon %f - Lat %f", lon, lat);

        proj::projection_perform_fwd(&p, lon, lat, &x, &y);

        logger->info("X %f - Y %f", x, y);
    }

    if (v)
    {
        logger->error("Proj error!");
        return 0;
    }

    {
        unsigned short color[4] = {0, 65535, 0, 65535};
        map::drawProjectedMapShapefile({"resources/maps/ne_10m_admin_0_countries.shp"},
                                       image_test,
                                       color,
                                       [&p](float lat, float lon, int, int) -> std::pair<int, int>
                                       {
                                           double x, y;
                                           proj::projection_perform_fwd(&p, lon, lat, &x, &y);
                                           return {(int)x, (int)y};
                                       });
    }

    image_test.save_jpeg(argv[2]);
#endif
}