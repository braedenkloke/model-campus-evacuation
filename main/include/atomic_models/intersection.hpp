#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <iostream>
#include <vector>
#include <string>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../data_structures/od_datum.hpp"
#include "../data_structures/vehicle.hpp"
#include "../constants.hpp"
#include "../data_structures/vehicle.hpp"

using namespace cadmium;

struct IntersectionState {
    double sigma;
    std::vector<ODDatum> odData; 
    bool hasCar;           // Is there a car waiting to be processed in intersection
    Vehicle currentCar;      // The car currently in the intersection
    int selectedRouteId;   // Chosen route for that car (as ID)

    explicit IntersectionState(): sigma(infinity), hasCar(false), currentCar(), selectedRouteId(-1) {} 

};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const IntersectionState& state) {
   return out << "HasCar:"<< state.hasCar << ",RouteIndex:" << state.selectedRouteId;
}
#endif

// Atomic DEVS model of a road intersection. Routes cars entering the intersection
// by selecting the destination with the highest flow rate.
class Intersection : public Atomic<IntersectionState> {
public:
    Port<Vehicle> inCar;               // Incoming car from a road model.
    Port<Vehicle> outSelectedCar;  // Identifier for which route the car took in the OD data.
   
    // ARGUMENTS
    // id - Model name. Equivalent to the origin in the OD data.
    // odData - Origin-destination (OD) data.
    Intersection(const std::string id, const std::vector<ODDatum>& odData): 
                 Atomic<IntersectionState>(id, IntersectionState()) {
        inCar = addInPort<Vehicle>("inCar");
        outSelectedCar = addOutPort<Vehicle>("outForSelectedRoute");
        state.odData = odData;
    }

    void internalTransition(IntersectionState& state) const override {
        // Wait for next car to enter intersection.
        state.hasCar = false; 
        state.selectedRouteId = -1;
        state.sigma = infinity; 
    }

    void externalTransition(IntersectionState& state, double e) const override {
        // Car enters intersection.
        if (!inCar->getBag().empty()) {
            state.currentCar = inCar->getBag().back();
            state.hasCar = true;
            state.selectedRouteId = selectRouteWithMaxFlow(state.odData); 
            state.currentCar.routeId = state.selectedRouteId;
            state.sigma = 0.0; 
        } 
    }

     void output(const IntersectionState& state) const override {
        if (state.hasCar && state.selectedRouteId != -1) {
            outSelectedCar->addMessage(state.currentCar);
        }
    }

    [[nodiscard]] double timeAdvance(const IntersectionState& state) const override {     
        return state.sigma;
    }

private:
    int selectRouteWithMaxFlow(const std::vector<ODDatum>& data) const {
        int bestIndex = -1;
        int maxFlow = -1;

        // Select the route with the highest flow rate for this intersection
        for(size_t i = 0; i < data.size(); i++) {
            
            // Only check entries that match this intersection's origin
            if(data[i].origin == this->id) {
                
                // Choose the highest flow value
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
