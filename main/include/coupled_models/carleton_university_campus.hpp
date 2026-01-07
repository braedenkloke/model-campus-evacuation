#ifndef CARLETON_UNIVERSITY_CAMPUS_HPP
#define CARLETON_UNIVERSITY_CAMPUS_HPP

#include <vector>
#include <map>
#include <set>
#include <string>
#include "cadmium/modeling/devs/coupled.hpp"
#include "../atomic_models/parking_lot.hpp"
#include "../atomic_models/road.hpp"
#include "../atomic_models/intersection.hpp"
#include "../data_structures/od_datum.hpp"

using namespace cadmium;

struct CarletonUniversityCampusCoupled : public Coupled {

    CarletonUniversityCampusCoupled(const std::string& id, std::vector<int> carDepartureTimes, 
                                    const std::vector<ODDatum>& odData ): Coupled(id) {
        auto p1 = addComponent<ParkingLot>("P1", carDepartureTimes);

        auto library_rd_and_p1_intersection = addComponent<Intersection>("Library Rd & P1", odData);

        addCoupling(p1->exit, library_rd_and_p1_intersection->inCar);
    }
};
#endif // CARLETON_UNIVERSITY_CAMPUS_HPP
