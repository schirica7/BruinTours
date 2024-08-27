//
//  router.hpp
//  BruinTours
//
//  Created by Stefan Chirica on 3/7/24.
//

#ifndef router_h
#define router_h

#include <stdio.h>
#include <vector>
#include <iostream>
#include "base_classes.h"
#include "geopoint.h"
#include "HashMap.h"
#include <set>
#include <string>
#include "geotools.h"

class Router: public RouterBase {
public:
    Router(const GeoDatabaseBase& geo_db);
    
    virtual ~Router();
    
    virtual std::vector<GeoPoint> route(const GeoPoint& pt1, const GeoPoint& pt2) const;
private:
    struct VisitedGeoPoint : GeoPoint {
        VisitedGeoPoint() {}
        VisitedGeoPoint(const GeoPoint &point, VisitedGeoPoint *lastPoint,
                     const GeoPoint &goal)
        : GeoPoint(point),
        m_distanceFromStart(lastPoint->m_distanceFromStart +
                            distance_earth_miles(*lastPoint, point)),
        m_distanceToFinish(distance_earth_miles(point, goal)),
        m_lastPoint(lastPoint) {}
        
        // for the start point, which has no last point
        VisitedGeoPoint(const GeoPoint &point, const GeoPoint &goal)
        : GeoPoint(point), m_distanceFromStart(0),
        m_distanceToFinish(distance_earth_miles(point, goal)),
        m_lastPoint(nullptr) {}
        
        bool operator>(const VisitedGeoPoint &rhs) const {
            return getDistanceFromStartToFinish() >
            rhs.getDistanceFromStartToFinish();
        }
        
        double getDistanceFromStartToFinish() const {
            return m_distanceToFinish + m_distanceFromStart;
        }
         
        double m_distanceFromStart;
        double m_distanceToFinish;
        VisitedGeoPoint *m_lastPoint;
    };
    
    struct rightPointGreater {
        bool operator()(VisitedGeoPoint *&lhs, VisitedGeoPoint *&rhs) const {
            return *lhs > *rhs;
        }
    };
    
    const GeoDatabaseBase* m_geoDB;
};
#endif /* router_hpp */
