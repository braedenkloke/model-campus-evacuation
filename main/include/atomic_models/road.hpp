#ifndef ROAD_HPP
#define ROAD_HPP

#include <iostream>
#include <deque>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../constants.hpp"

using namespace cadmium;

struct RoadState {
    double sigma;
    double lengthInMetres; 
    double speedLimitInKmph;
    std::deque<Vehicle> vehicles; 
    std::deque<double> carDepartureTimesInSeconds;  // Use double for time calculation (length/speed)
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
    Road(const std::string id, int lengthInMetres, int speedLimitInKmph) : Atomic<RoadState>(id, RoadState()) {
        entrance = addInPort<Vehicle>("entrance");
        exit = addOutPort<Vehicle>("exit");

        state.lengthInMetres = lengthInMetres; 
        state.speedLimitInKmph = speedLimitInKmph;
    }

    void internalTransition(RoadState& state) const override {
        // Car exits road.
        // Time at the queue front = elapsed time
        double elapsedTime = state.carDepartureTimesInSeconds.front();
        // Update remaining departure times
        for(int i = 1; i < state.carDepartureTimesInSeconds.size(); i++){
            state.carDepartureTimesInSeconds[i] = state.carDepartureTimesInSeconds[i] - elapsedTime;
        }
        // Remove departed car time
        state.carDepartureTimesInSeconds.pop_front();
        state.vehicles.pop_front();

        // Set sigma to next car's departure time.
        if(!state.carDepartureTimesInSeconds.empty()){
            state.sigma = state.carDepartureTimesInSeconds.front();
        }else {
            state.sigma = infinity;
        }

        /* --- WHY DEQUE? ---
        
        - It can add and remove elements from back and front.
       
        - For road model's departure times it also needs to iterate through 
        the elements in the middle of the list one by one and update them.
       
        - Using deque because needed to remove items from the front, and vector is slow for that.
        
            loop 1 [10, 20, 30]
            [10, 10, 30] -> for first car we wait for 10 sec.
            loop 2 [10, 10, 30]
            [10, 10, 20] -> also for last car 10 sec past.
            
        - Elapsed time going to change with pop_front for next internal transition event.*/ 


    }

	void externalTransition(RoadState& state, double e) const override {
        // Car enters road. 

        // Update all departure times based on elapsed time.
        for (int i = 0; i < state.carDepartureTimesInSeconds.size(); i++) {
            state.carDepartureTimesInSeconds[i] = state.carDepartureTimesInSeconds[i] - e;
            
        }
        // Get car
        Vehicle inCar = entrance->getBag().back();

        // Calculate how long the car that entered takes to travel the road.
        //
        // Assume cars travel the speed limit; ignore accelaration and deceleration.
        double lengthInKm = state.lengthInMetres / 1000;
        double hours = lengthInKm / state.speedLimitInKmph;
        double minutes = hours * 60;
        double carTravelTimeInSeconds = minutes * 60;

        // Add car to queue and schedule its road exit.
        state.vehicles.push_back(inCar);
        state.carDepartureTimesInSeconds.push_back(carTravelTimeInSeconds);
        state.sigma = state.carDepartureTimesInSeconds.front();
    }
    
    void output(const RoadState& state) const override {
        exit->addMessage(state.vehicles.front()); // The car has left the road.
    }

    [[nodiscard]] double timeAdvance(const RoadState& state) const override {     
        return state.sigma;
    }
};

#endif // ROAD_HPP
