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
        // Out roads
        std::vector<std::string> x01OutRoads{"Library Rd & P1 to Library Rd & University Dr"};

        // Create models
        auto p1 = addComponent<ParkingLot>("P1", carDepartureTimes);
        auto x01 = addComponent<Intersection>("Library Rd & P1", odData, x01OutRoads);
        auto r01 = addComponent<Road>("Library Rd & P1 to Library Rd & University Dr");

        // Couple intersection inputs
        addCoupling(p1->exit, x01->in);

        // Couple intersection outputs
        addCoupling(x01->out1, r01->entrance);
    }
};
#endif // CARLETON_UNIVERSITY_CAMPUS_HPP
