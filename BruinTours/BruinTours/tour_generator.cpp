//
//  tour_generator.cpp
//  BruinTours
//
//  Created by Stefan Chirica on 3/7/24.
//

#include "tour_generator.h"
#include "geotools.h"
using namespace std;

TourGenerator::TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router) : m_geoDB(geodb), m_router(router) {}

TourGenerator::~TourGenerator() {}

vector<TourCommand> TourGenerator::generate_tour(const Stops &stops) const {
    vector<TourCommand> c;
    
    for (int i = 0; i < stops.size(); i++) {
        TourCommand commentary;
        
        string poiName;
        string talkingPoints;
        stops.get_poi_data(i, poiName, talkingPoints);
        commentary.init_commentary(poiName, talkingPoints);
        c.push_back(commentary);
        
        vector<GeoPoint> p;
        if (i != stops.size()-1) {
            GeoPoint poi;
            m_geoDB.get_poi_location(poiName, poi);
            
            string poiName1;
            string talkingPoints1;
            stops.get_poi_data(i+1, poiName1, talkingPoints1);
            GeoPoint poi1;
            m_geoDB.get_poi_location(poiName1, poi1);
            
            p = m_router.route(poi, poi1);
        }
        
        for (int i = 0; i < p.size(); i++) {
            if (i < p.size()-1)
                m_geoDB.get_street_name(p[i], p[i+1]);
        }
        
        for (int i = 0; i < p.size(); i++) {
            if (i < p.size()-1) {
                TourCommand proceed;
                string direction;
                
                double angle = angle_of_line(p[i], p[i+1]);
                
                    if (angle < 22.5) {
                        direction = "east";
                    } else if (angle < 67.5) {
                        direction = "northeast";
                    } else if (angle < 112.5) {
                        direction = "north";
                    } else if (angle < 157.5) {
                        direction = "northwest";
                    } else if (angle < 202.5) {
                        direction = "west";
                    } else if (angle < 247.5) {
                        direction = "southwest";
                    } else if (angle < 292.5) {
                        direction = "south";
                    } else if (angle < 337.5) {
                        direction = "southeast";
                    } else {
                        direction = "east";
                    }

                proceed.init_proceed(direction, m_geoDB.get_street_name(p[i], p[i+1]), distance_earth_miles(p[i], p[i+1]), p[i], p[i+1]);
                c.push_back(proceed);
            }
            
            if (i < p.size() - 2) {
                if (m_geoDB.get_street_name(p[i], p[i+1]) != m_geoDB.get_street_name(p[i+1], p[i+2])) {
                    
                    double a = angle_of_turn(p[i], p[i+1], p[i+2]);
                    if (a > 1 && a < 359 ) {
                        TourCommand turn;
                        string dir;
                        
                        if (angle_of_turn(p[i], p[i+1], p[i+2]) < 180)
                            dir = "left";
                        else
                            dir = "right";
                        
                        turn.init_turn(dir, m_geoDB.get_street_name(p[i+1], p[i+2]));
                        c.push_back(turn);
                    }
                }
            }
        }
    }
    
    return c;
}
