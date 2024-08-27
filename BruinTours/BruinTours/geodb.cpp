//
//  geodb.cpp
//  BruinTours
//
//  Created by Stefan Chirica on 3/7/24.
//

#include "geodb.h"
#include "geopoint.h"
#include "geotools.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "HashMap.h"
using namespace std;

GeoDatabase::GeoDatabase() {}

GeoDatabase::~GeoDatabase() {}

bool GeoDatabase::load(const string &map_data_file) {
    
    ifstream inf(map_data_file);
    if (!inf)
        return false;
    
    string streetName;
    while (getline(inf, streetName)) {
        string geoPoints;
        getline(inf, geoPoints);
        
        istringstream issPoints(geoPoints);
        string startLat;
        string startLong;
        string endLat;
        string endLong;
        issPoints >> startLat >> startLong >> endLat >> endLong;

        GeoPoint firstPos(startLat, startLong);
        GeoPoint secondPos(endLat, endLong);
        
        connectPoints(firstPos, secondPos, streetName);
        
        int numPois;
        inf >> numPois;
        
        //Needed because I want to get a string after
        inf.ignore(10000, '\n');
        
        if (numPois > 0) {
            GeoPoint mp = midpoint(firstPos, secondPos);
            connectPoints(mp, firstPos, streetName);
            connectPoints(mp, secondPos, streetName);
            
            for (int i = 0; i < numPois; i++) {
                string poiData;
                getline(inf, poiData);
                int dividerPos = poiData.size()-24;
                
                //Splitting this line into two parts
                string poiName = poiData.substr(0, dividerPos);
                string geoPoint = poiData.substr(dividerPos+1);
                
                //Then, splitting the geopoint substring into two parts
                string lat, lng;
                istringstream issPoints(geoPoint);
                issPoints >> lat >> lng;
                
                GeoPoint poi(lat, lng);
                connectPoints(mp, poi, "a path"); 
                
                pois[poiName] = connectedPts[poi.to_string()];
            }
        }
    }
    return true;
}

std::vector<GeoPoint>GeoDatabase::get_connected_points(const GeoPoint &pt) const {
    const GeoPointConnected *cp = connectedPts.find(pt.to_string());
    
    std::vector<GeoPoint> connectedPoints;
    if (cp != nullptr)
        for (const GeoPoint *p : cp->points)
            connectedPoints.push_back(*p);
    
    return connectedPoints;
}

std::string GeoDatabase::get_street_name(const GeoPoint &pt1,
                                         const GeoPoint &pt2) const {
    const std::string *streetName =
    streetNames.find(pt1.to_string() + " " + pt2.to_string());
    
    if (streetName == nullptr) {
        streetName = streetNames.find(pt2.to_string() + " " + pt1.to_string());
        if (streetName == nullptr)
            return "";
    }
    return *streetName;
}

void GeoDatabase::connectPoints(const GeoPoint &p1, const GeoPoint &p2, const std::string &streetName){
    
    //If the points are not already associated with each other, associate them.
    if (get_street_name(p1, p2).empty()) {
        streetNames[p1.to_string() + " " + p2.to_string()] = streetName;
        
        if (connectedPts.find(p1.to_string()) == nullptr)
            connectedPts.insert(p1.to_string(), GeoPointConnected(p1));
        
        if (connectedPts.find(p2.to_string()) == nullptr)
            connectedPts.insert(p2.to_string(), GeoPointConnected(p2));
        
        //Not only can p1/p2 access each other, but because connectedPts contains a GeoPointConnected, they could also gain access to the other's connected points
        connectedPts[p1.to_string()].points.push_back(&connectedPts[p2.to_string()]);
        connectedPts[p2.to_string()].points.push_back(&connectedPts[p1.to_string()]);
    }
}

bool GeoDatabase::get_poi_location(const std::string &poi,
                                   GeoPoint &point) const {
    const GeoPoint *poiPoint = pois.find(poi);
    if (poiPoint == nullptr)
        return false;
    point = *poiPoint;
    return true;
}

