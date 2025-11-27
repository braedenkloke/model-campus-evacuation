#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <iostream>
#include <vector>
#include <string>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../data_structures/od_datum.hpp"
#include "../data_structures/vehicle.hpp"
#include "../constants.hpp"

using namespace cadmium;

struct IntersectionState {
    double sigma;
    std::vector<ODDatum> odData; 
    bool hasCar;              // Is there a waiting car 
    int currentCarId;         // Vehicle ID 
    int selectedRouteIndex;   // Chosen route index for the car 

    explicit IntersectionState(): sigma(infinity), hasCar(false), currentCarId(-1), selectedRouteIndex(-1) {} 

};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const IntersectionState& state) {
   return out << "HasCar:"<< state.hasCar << ",RouteIndex:" << state.selectedRouteIndex;
}
#endif

// Atomic DEVS model of a road intersection. Routes cars entering the intersection
// by selecting the destination with the highest flow rate.
class Intersection : public Atomic<IntersectionState> {
public:
    Port<Vehicle> inCar;            // Incoming car
    Port<Vehicle> outCarWithRoute;  // Outgoing car
   
    // ARGUMENTS
    // id - Model name. Equivalent to the origin in the OD data.
    // odData - Origin-destination (OD) data.
    Intersection(const std::string id, const std::vector<ODDatum>& odData): 
                 Atomic<IntersectionState>(id, IntersectionState()) {
        inCar = addInPort<Vehicle>("inCar");
        outCarWithRoute = addOutPort<Vehicle>("outCarWithRoute");
        state.odData = odData;
    }

    void internalTransition(IntersectionState& state) const override {
        // Wait for the next car
        state.hasCar = false; 
        state.currentCarId = -1;
        state.sigma = infinity; 
    }

    void externalTransition(IntersectionState& state, double e) const override {
        // Car enters intersection.
        if (!inCar->getBag().empty()) {
            state.currentCarId = inCar->getBag().back().id;
            state.hasCar = true;
            state.selectedRouteIndex = selectRouteWithMaxFlow(state.odData);
            state.sigma = 0.0; 
        } 
    }

     void output(const IntersectionState& state) const override {
        if (state.hasCar && state.selectedRouteIndex != -1) {
            Vehicle outVehicle(state.currentCarId, state.selectedRouteIndex);
            outCarWithRoute->addMessage(outVehicle);
        }
    }

    [[nodiscard]] double timeAdvance(const IntersectionState& state) const override {     
        return state.sigma;
    }

private:
    // Select the route with the highest flow rate
    int selectRouteWithMaxFlow(const std::vector<ODDatum>& data) const {
        int bestIndex = -1;
        int maxFlow = -1;

        for(size_t i = 0; i < data.size(); i++) {

            // Only checks ids with the same origin
            if(data[i].origin == this->id) {

                if(data[i].flowRate > maxFlow) {
                    maxFlow = data[i].flowRate; 
                    bestIndex = i;        
                }
            }
        }
        return bestIndex; 
    }
};

#endif // INTERSECTION_HPP
