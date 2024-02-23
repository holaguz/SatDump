#pragma once

#include "proj.h"
#include "nlohmann/json.hpp"

namespace proj
{
    void to_json(nlohmann::json &j, const projection_t &p)
    {
        if (p.type == ProjType_Equirectangular)
            j["type"] = "equirectangular";
        else if (p.type == ProjType_Stereographic)
            j["type"] = "stereographic";
        else if (p.type == ProjType_UniversalTransverseMercator)
            j["type"] = "universal_transverse_mercator";

        if (p.type == ProjType_UniversalTransverseMercator)
        {
            j["zone"] = p.params.zone;
            j["south"] = p.params.south;
        }

        if (p.proj_offset_x != 0)
            j["offset_x"] = p.proj_offset_x;
        if (p.proj_offset_y != 0)
            j["offset_y"] = p.proj_offset_y;
        if (p.proj_scalar_x != 1)
            j["scalar_x"] = p.proj_scalar_x;
        if (p.proj_scalar_y != 1)
            j["scalar_y"] = p.proj_scalar_y;

        if (p.lam0 != 0)
            j["lon0"] = p.lam0 * RAD2DEG;
        if (p.phi0 != 0)
            j["lat0"] = p.phi0 * RAD2DEG;
    }

    void from_json(const nlohmann::json &j, projection_t &p)
    {
        if (j["type"].get<std::string>() == "equirectangular")
            p.type = ProjType_Equirectangular;
        else if (j["type"].get<std::string>() == "stereographic")
            p.type = ProjType_Stereographic;
        else if (j["type"].get<std::string>() == "universal_transverse_mercator")
            p.type = ProjType_UniversalTransverseMercator;

        if (p.type == ProjType_UniversalTransverseMercator)
        {
            if (j.contains("zone"))
                p.params.zone = j["zone"];
            if (j.contains("south"))
                p.params.south = j["south"];
        }

        if (j.contains("offset_x"))
            p.proj_offset_x = j["offset_x"];
        if (j.contains("offset_y"))
            p.proj_offset_y = j["offset_y"];
        if (j.contains("scalar_x"))
            p.proj_scalar_x = j["scalar_x"];
        if (j.contains("scalar_y"))
            p.proj_scalar_y = j["scalar_y"];

        if (j.contains("lon0"))
            p.lam0 = j["lon0"].get<double>() * DEG2RAD;
        if (j.contains("lat0"))
            p.phi0 = j["lat0"].get<double>() * DEG2RAD;
    }
}