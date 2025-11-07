#ifndef TOP_HPP
#define TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "parking_lot.hpp"
#include "road.hpp"

using namespace cadmium;

struct TopCoupled : public Coupled {

    TopCoupled(const std::string& id, std::vector<int> carDepartureTimes) : Coupled(id) {
        auto parkingLot = addComponent<ParkingLot>("parking lot", carDepartureTimes);
        auto road = addComponent<Road>("road", 100, 30);

        // Couple output ports to input ports
        addCoupling(parkingLot->exit, road->entrance);
    }
};

#endif // TOP_HPP
