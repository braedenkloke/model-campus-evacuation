#ifndef TOP_HPP
#define TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "../atomic_models/parking_lot.hpp"
#include "../atomic_models/road.hpp"
#include "../atomic_models/intersection.hpp"
#include "../data_structures/od_datum.hpp"

using namespace cadmium;

struct TopCoupled : public Coupled {

    TopCoupled(const std::string& id, std::vector<int> carDepartureTimes, const std::vector<ODDatum>& odData ): Coupled(id) {
        auto parkingLot = addComponent<ParkingLot>("parking lot", carDepartureTimes);
        auto road = addComponent<Road>("road", 100, 30);
        auto intersection = addComponent<Intersection>("intersection", odData);
        // Couple output ports to input ports
        addCoupling(parkingLot->exit, intersection->inCar);
        addCoupling(intersection->outSelectedRouteId, road->entrance);
    }
};
#endif // TOP_HPP
