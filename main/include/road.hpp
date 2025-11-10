#ifndef ROAD_HPP
#define ROAD_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"
#include "constants.hpp"

using namespace cadmium;

struct RoadState {
    double sigma;
    double lengthInMetres; 
    double speedLimitInKmph; 
    std::vector<int> carDepartureTimesInSeconds;    

    explicit RoadState(): sigma(infinity) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const RoadState& state) {
    return out << state.carDepartureTimesInSeconds.size(); 
}
#endif

// Atomic DEVS model of a road which cars enter and exit from.
class Road : public Atomic<RoadState> {
public:
    Port<int> entrance, exit;

    // ARGUMENTS
    // id - Model name.
    // lengthInMetres - Length of road in metres.
    // speedLimitInKmph - Speed limit of road in kilometres per hour.
    Road(const std::string id, int lengthInMetres, int speedLimitInKmph) : Atomic<RoadState>(id, RoadState()) {
        entrance = addInPort<int>("entrance");
        exit = addOutPort<int>("exit");

        state.lengthInMetres = lengthInMetres; 
        state.speedLimitInKmph = speedLimitInKmph;
    }

    void internalTransition(RoadState& state) const override {
        // Car exits road.

        // Update car departure times based on elapsed time of the car that's exiting.
        for (int i = 1; i < state.carDepartureTimesInSeconds.size(); i++) {
            state.carDepartureTimesInSeconds[i] = state.carDepartureTimesInSeconds[i] - state.carDepartureTimesInSeconds.front();
        }
    
        // Remove car that's exiting from departure times.
        for (int i = 0; i < state.carDepartureTimesInSeconds.size() - 1; i++) {
            state.carDepartureTimesInSeconds[i] = state.carDepartureTimesInSeconds[i + 1];
        }
        state.carDepartureTimesInSeconds.pop_back();

        // Set time advance to be the next time a car is exiting.
        if (!state.carDepartureTimesInSeconds.empty()) {
            state.sigma = state.carDepartureTimesInSeconds.front();
        } else {
            state.sigma = infinity;
        }
    }

	void externalTransition(RoadState& state, double e) const override {
        // Car enters road. 

        // Update all departure times based on elapsed time.
        for (int i = 0; i < state.carDepartureTimesInSeconds.size(); i++) {
            state.carDepartureTimesInSeconds[i] = state.carDepartureTimesInSeconds[i] - e;
            
        }

        // Calculate how long the car that entered takes to travel the road.
        //
        // Assume cars travel the speed limit; ignore accelaration and deceleration.
        double lengthInKm = state.lengthInMetres / 1000;
        double hours = lengthInKm / state.speedLimitInKmph;
        double minutes = hours * 60;
        double carTravelTimeInSeconds = minutes * 60;

        // Schedule car to exit the road.
        state.carDepartureTimesInSeconds.push_back(carTravelTimeInSeconds);
        state.sigma = state.carDepartureTimesInSeconds.front();
    }
    
    void output(const RoadState& state) const override {
        exit->addMessage(1); // Placeholder. Indicates that one car has left the road.
    }

    [[nodiscard]] double timeAdvance(const RoadState& state) const override {     
        return state.sigma;
    }
};

#endif // ROAD_HPP
