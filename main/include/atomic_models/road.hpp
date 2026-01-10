#ifndef ROAD_HPP
#define ROAD_HPP

#include <iostream>
#include <deque>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../constants.hpp"
#include "../data_structures/vehicle.hpp"

using namespace cadmium;

struct RoadState {
    double sigma;
    double lengthInMetres; 
    double speedLimitInKmph; 
    std::deque<double> carDepartureTimesInSeconds;  // Using double
                                                    // because time = length/speed 
                                                    // can be floating point format
    std::deque<Vehicle> vehiclesOnRoad;        // Stores vehicles currently on the road.                                         
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
        // Car exits road.
        // The time at the front of queue is the elapsed time
        double elapsedTime = state.carDepartureTimesInSeconds.front();
        // Update remaining departure times
        for(int i = 1; i < state.carDepartureTimesInSeconds.size(); i++){
            state.carDepartureTimesInSeconds[i] = state.carDepartureTimesInSeconds[i] - elapsedTime;
        }
        // Romeving departed car's time 
        state.carDepartureTimesInSeconds.pop_front();

        // Set sigma to next car's departure time.
        if(!state.carDepartureTimesInSeconds.empty()){
            state.sigma = state.carDepartureTimesInSeconds.front();
        }else {
            state.sigma = infinity;
        }
        
        if(!state.vehiclesOnRoad.empty()){
            state.vehiclesOnRoad.pop_front(); //remove vechile exiting road
        }
    }

	void externalTransition(RoadState& state, double e) const override {
        // Car enters road. 

        // Update all departure times based on elapsed time.
        for (int i = 0; i < state.carDepartureTimesInSeconds.size(); i++) {
            state.carDepartureTimesInSeconds[i] = state.carDepartureTimesInSeconds[i] - e;
            
        }

        if (!entrance->getBag().empty()) {
            Vehicle v = entrance->getBag().back();
            state.vehiclesOnRoad.push_back(v);   // add vehicle to road queue
        }

        // Schedule car to exit the road.
        double carTravelTimeInSeconds = calcTravelTimeInSeconds(state.lengthInMetres, state.speedLimitInKmph);
        state.carDepartureTimesInSeconds.push_back(carTravelTimeInSeconds);
        state.sigma = state.carDepartureTimesInSeconds.front();
    }
    
    void output(const RoadState& state) const override {
        if(!state.vehiclesOnRoad.empty()){
            exit->addMessage(state.vehiclesOnRoad.front()); //vehicle object leaves road
        }
    }

    [[nodiscard]] double timeAdvance(const RoadState& state) const override {     
        return state.sigma;
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
