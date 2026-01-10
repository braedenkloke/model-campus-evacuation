#ifndef ROAD_HPP
#define ROAD_HPP

#include <iostream>
#include <deque>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../constants.hpp"
#include "../data_structures/vehicle.hpp"

using namespace cadmium;

struct RoadState {
    double lengthInMetres; 
    double speedLimitInKmph; 
    std::deque<Vehicle> vehicles;        

    explicit RoadState() {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const RoadState& state) {
    return out << "Number of cars on road: " << state.vehicles.size(); 
}
#endif

// Atomic DEVS model of a road which cars enter and exit from.
class Road : public Atomic<RoadState> {
public:
    Port<Vehicle> entrance, exit;

    // ARGUMENTS
    // id - Model name.
    // lengthInMetres - Length of road in metres.
    // speedLimitInKmph - Speed limit of road in kilometres per hour.
    Road(const std::string id, int lengthInMetres = 100, int speedLimitInKmph = 40): 
         Atomic<RoadState>(id, RoadState()) {
        entrance = addInPort<Vehicle>("entrance");
        exit = addOutPort<Vehicle>("exit");

        state.lengthInMetres = lengthInMetres; 
        state.speedLimitInKmph = speedLimitInKmph;
    }

    void internalTransition(RoadState& state) const override {
        Vehicle x = state.vehicles.front();
        state.vehicles.pop_front();
        for (Vehicle v: state.vehicles) {
            v.t = v.t - x.t;
        }
    }

	void externalTransition(RoadState& state, double e) const override {
        // Vehicles enter road. 

        // Update all travel times based on elapsed time.
        for (Vehicle v : state.vehicles) {
            v.t = v.t - e;
        }

        if (!entrance->getBag().empty()) {
            for (Vehicle v : entrance->getBag()) {
                v.t = calcTravelTimeInSeconds(state.lengthInMetres, state.speedLimitInKmph);
                state.vehicles.push_back(v);
            }
        }
    }
    
    void output(const RoadState& state) const override {
        if(!state.vehicles.empty()){
            exit->addMessage(state.vehicles.front());
        }
    }

    [[nodiscard]] double timeAdvance(const RoadState& state) const override {     
        if (state.vehicles.empty()) {
            return infinity;
        } else {
            return state.vehicles.front().t;
        }
    }

private:
    double calcTravelTimeInSeconds(const double lengthInMetres, const double speedLimitInKmph) const {
        // Assume cars travel the speed limit; ignore accelaration and deceleration.
        double lengthInKm = state.lengthInMetres / 1000;
        double hours = lengthInKm / state.speedLimitInKmph;
        double minutes = hours * 60;
        double seconds = minutes * 60;
        return seconds;
    }
};

#endif // ROAD_HPP
