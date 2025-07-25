#include "geometry.h"

void setGeometryType (MUSE::GeospatialData &geometry, std::string &GDALtype)
{
    if(GDALtype.compare("POLYGON") == 0)
        geometry.geom_type = POLYGON;

    if(GDALtype.compare("POINT") == 0)
        geometry.geom_type = POINT;

    if(GDALtype.compare("LINESTRING") == 0)
        geometry.geom_type = LINESTRING;
}


// // Update project json
// void updateProjectJson (const MUSE::GeospatialData &geometry, json &metadata)
// {
//     metadata.push_back({geometry.getName(), {
//                             {"Format",          geometry.getFormat()},
//                             {"Geometry Type",   geometry.geom_type},
//                             {"Authority",       geometry.getAuthority()}
//                             //{"N. Domains",      geometry.getDomains()}
//                         }});
// }


// // Update json
// void updateJson (const MUSE::GeospatialData &geometry, json &metadata)
// {
//     metadata.push_back({"File", {
//                             {"Name",            geometry.getName()},
//                             {"Format",          geometry.getFormat()},
//                             {"Geometry Type",   geometry.geom_type},
//                             {"Authority",       geometry.getAuthority()},
//                             {"Unity",           "meters"}
//                             //{"N. Domains",      geometry.getDomains()}
//                         }});
// }
