//
//  tour_generator.h
//  BruinTours
//
//  Created by Stefan Chirica on 3/7/24.
//

#ifndef tour_generator_h
#define tour_generator_h

#include <stdio.h>
#include "base_classes.h"
#include <vector>
#include "stops.h"
#include "tourcmd.h"

class TourGenerator: public TourGeneratorBase {
public:
    TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router); 
    virtual ~TourGenerator();
    virtual std::vector<TourCommand> generate_tour(const Stops &stops) const;
private:
    const GeoDatabaseBase& m_geoDB;
    const RouterBase& m_router;
};

#endif /* tour_generator_hpp */
