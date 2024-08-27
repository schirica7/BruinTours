//
//  router.cpp
//  BruinTours
//
//  Created by Stefan Chirica on 3/7/24.
//
#include <vector>
#include <queue>
#include <iostream>
#include "HashMap.h"
#include "router.h"
#include "geopoint.h"
#include "geodb.h"
#include "geotools.h"
#include <functional>
using namespace std;

Router::Router(const GeoDatabaseBase& geo_db) : m_geoDB(&geo_db) {}

Router::~Router() {}

vector<GeoPoint> Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const {
    
    priority_queue<VisitedGeoPoint*, vector<VisitedGeoPoint*>, rightPointGreater> openSet;
    HashMap<VisitedGeoPoint> visitedPoints;
    vector<GeoPoint> path;
    
    //This is going from finish to start so I don't have to reverse the path vector afterwards.
    visitedPoints[pt2.to_string()] = VisitedGeoPoint(pt2, pt1);
    openSet.push(&visitedPoints[pt2.to_string()]);

    do {
        VisitedGeoPoint* currentPoint = openSet.top();
        openSet.pop();
        vector<GeoPoint> pts = m_geoDB->get_connected_points(*currentPoint);
        
        for (const GeoPoint& p : pts) {
            VisitedGeoPoint* nextPoint = visitedPoints.find(p.to_string());
            
            if (nextPoint == nullptr || nextPoint->m_distanceFromStart >= distance_earth_miles(*currentPoint, *nextPoint) + currentPoint->m_distanceFromStart) {
                
                //If next point is not visited, or current point has a shorter path we will add this point to the queue.
                visitedPoints[p.to_string()] = VisitedGeoPoint(p, currentPoint, pt1);
                openSet.push(&visitedPoints[p.to_string()]);
            }
            
        }
        
    } while (!openSet.empty() && openSet.top()->m_distanceToFinish != 0);
    
    if (openSet.empty())
        return path;
    
    for (VisitedGeoPoint currPoint = *openSet.top(); currPoint.m_lastPoint != nullptr; currPoint = *(currPoint.m_lastPoint)) {
        path.push_back(currPoint);
    }
    
    path.push_back(pt2);
    
    return path;
}
