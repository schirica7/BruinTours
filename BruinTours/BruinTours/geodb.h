//
//  geodb.hpp
//  BruinTours
//
//  Created by Stefan Chirica on 3/7/24.
//

#ifndef geodb_h
#define geodb_h

#include <stdio.h>
#include <vector>
#include <string>
#include "geopoint.h"
#include "base_classes.h"
#include "HashMap.h"
#include <map>

class GeoDatabase: public GeoDatabaseBase {
public:
    GeoDatabase();
    virtual ~GeoDatabase();
    virtual bool load(const std::string& map_data_file); 
    virtual bool get_poi_location(const std::string& poi, GeoPoint& point) const;
    virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;
    virtual std::string get_street_name(const GeoPoint& pt1,
                                        const GeoPoint& pt2) const;
private:
    struct GeoPointConnected : GeoPoint {
        GeoPointConnected() {}
      GeoPointConnected(const GeoPoint &pt)
          : GeoPoint(pt.sLatitude, pt.sLongitude) {}

      std::vector<GeoPoint*> points;
    };
    
    void connectPoints(const GeoPoint& p1, const GeoPoint& p2, const std::string& streetName);

    HashMap<std::string> streetNames;
    HashMap<GeoPointConnected> connectedPts;
    HashMap<GeoPoint> pois;
};


#endif /* geodb_hpp */

