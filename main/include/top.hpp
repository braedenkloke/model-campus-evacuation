#ifndef TOP_HPP
#define TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "atomics/parking_lot.hpp"
#include "atomics/road_intersection.hpp"
#include "data_structures/intersection_config.hpp"
#include "data_structures/od_datum.hpp"

using namespace cadmium;

struct TopCoupled : public Coupled {

    TopCoupled(const std::string& id, std::vector<int> carDepartureTimes, std::vector<IntersectionConfig> intersectionData,
               std::vector<ODDatum> odData) : Coupled(id) {
        auto parkingLot = addComponent<ParkingLot>("P1", carDepartureTimes);

        std::vector<std::shared_ptr<RoadIntersection>> intersections;
        for (IntersectionConfig config: intersectionData) {
            auto i = addComponent<RoadIntersection>(config.id, odData);
            intersections.push_back(i);
        }

        // Couple output ports to input ports
        addCoupling(parkingLot->exit, intersections[0]->entrance);
        addCoupling(intersections[0]->exit, intersections[1]->entrance);
    }
};

#endif // TOP_HPP
