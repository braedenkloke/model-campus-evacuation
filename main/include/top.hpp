#ifndef TOP_HPP
#define TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "parking_lot.hpp"
#include "atomics/road_intersection.hpp"
#include "data_structures/od_datum.hpp"

using namespace cadmium;

struct TopCoupled : public Coupled {

    TopCoupled(const std::string& id, std::vector<int> carDepartureTimes, std::vector<ODDatum> odData) : Coupled(id) {
        auto parkingLot = addComponent<ParkingLot>("P1", carDepartureTimes);
        auto intersectionA = addComponent<RoadIntersection>("A", odData);
        auto intersectionB = addComponent<RoadIntersection>("B", odData);

        // Couple output ports to input ports
        addCoupling(parkingLot->exit, intersectionA->entrance);
        addCoupling(intersectionA->exit, intersectionB->entrance);
    }
};

#endif // TOP_HPP
