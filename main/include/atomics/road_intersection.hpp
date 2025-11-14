#ifndef ROAD_INTERSECTION_HPP
#define ROAD_INTERSECTION_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../constants.hpp"
#include "../data_structures/od_datum.hpp"

using namespace cadmium;

struct RoadIntersectionState {
    double sigma;
    std::vector<ODDatum> odData;
    std::vector<int> carDepartureTimesInSeconds;    

    explicit RoadIntersectionState(): sigma(infinity) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const RoadIntersectionState& state) {
    return out << state.carDepartureTimesInSeconds.size(); 
}
#endif

// Atomic DEVS model of road intersection that routes incoming traffic based on  
// origin-destination (OD) data.
class RoadIntersection : public Atomic<RoadIntersectionState> {
public:
    Port<int> entrance, exit;

    // ARGUMENTS
    // id - Model name.
    // odData - Origin-destination data.
    RoadIntersection(const std::string id, std::vector<ODDatum> odData): 
                     Atomic<RoadIntersectionState>(id, RoadIntersectionState()) {
        entrance = addInPort<int>("entrance");
        exit = addOutPort<int>("exit");

        // Filter OD data for origins that match this model's name.
        std::vector<ODDatum> tmp;
        for (ODDatum od: odData) {
            if (id.compare(od.origin) == 0) {
                tmp.push_back(od);
            }
        }
        state.odData = tmp;
    }

    void internalTransition(RoadIntersectionState& state) const override {
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

	void externalTransition(RoadIntersectionState& state, double e) const override {
        // Vehicle enters road. 

        // Update all departure times based on elapsed time.
        for (int i = 0; i < state.carDepartureTimesInSeconds.size(); i++) {
            state.carDepartureTimesInSeconds[i] = state.carDepartureTimesInSeconds[i] - e;
            
        }

        // Route vehicle.
        if (!state.odData.empty()) {
            // Calculate travel time.
            double carTravelTimeInSeconds = 10; // Placeholder. Takes a vehicle X seconds to travel anywhere.

            // Schedule vehicle to exit the road.
            state.carDepartureTimesInSeconds.push_back(carTravelTimeInSeconds);
            state.sigma = state.carDepartureTimesInSeconds.front();
        }
    }
    
    void output(const RoadIntersectionState& state) const override {
        exit->addMessage(1); // Indicates that one vehicle has left the intersection 
                             // and arrived at another intersection after some amount of travel time.
    }

    [[nodiscard]] double timeAdvance(const RoadIntersectionState& state) const override {     
        return state.sigma;
    }
};

#endif // ROAD_INTERSECTION_HPP
