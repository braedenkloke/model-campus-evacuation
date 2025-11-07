#ifndef TOP_HPP
#define TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "parking_lot.hpp"

using namespace cadmium;

struct TopCoupled : public Coupled {

    TopCoupled(const std::string& id, std::vector<int> carDepartureTimes) : Coupled(id) {
        auto parkingLot = addComponent<ParkingLot>("parking lot", carDepartureTimes);
    }
};

#endif // TOP_HPP
