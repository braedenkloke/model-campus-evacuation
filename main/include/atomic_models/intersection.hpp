#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../data_structures/od_datum.hpp"
#include "../data_structures/vehicle.hpp"
#include "../constants.hpp"
#include "../data_structures/vehicle.hpp"

using namespace cadmium;

struct IntersectionState {
    double sigma;
    std::vector<ODDatum> odData;
    std::vector<int> validRouteIndices; // OD route indices for this intersection
    bool hasCar;                        // Is there a car waiting in intersection
    Vehicle currentCar;                 // The car currently in the intersection
    int targetPortIndex;                // Selected port index for this intersection
    int selectedOdIndex;                // Selected route for that car from OD data (as index)

    explicit IntersectionState(): sigma(infinity), hasCar(false), targetPortIndex(-1),selectedOdIndex(-1) {} 

}; 

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const IntersectionState& state) {
    return out << "HasCar: " << state.hasCar 
               << ", IntersectionOutPortIndex: " << state.targetPortIndex
               << ", SelectedOdDataIndex: " << state.selectedOdIndex;

}
#endif

// Atomic DEVS model of a road intersection. Routes cars entering the intersection
// by selecting the destination with the highest flow rate.
class Intersection : public Atomic<IntersectionState> {
public:
    Port<Vehicle> inCar;               // Incoming car from a road model.
    Port<Vehicle> outRoad1;            // Car exits through one of 4 out ports
    Port<Vehicle> outRoad2; 
    Port<Vehicle> outRoad3; 
    Port<Vehicle> outRoad4;  

    std::vector <Port<Vehicle>> outPorts;
   
    // ARGUMENTS
    // id - Model name. Equivalent to the origin in the OD data.
    // odData - Origin-destination (OD) data.
    // opm - Outport port map.
    Intersection(const std::string id, const std::vector<ODDatum>& odData, const std::map<int, std::string>& opm): 
                 Atomic<IntersectionState>(id, IntersectionState()) {
        inCar = addInPort<Vehicle>("inCar");
        
        outRoad1 = addOutPort<Vehicle>("outRoad1");
        outRoad2 = addOutPort<Vehicle>("outRoad2");
        outRoad3 = addOutPort<Vehicle>("outRoad3");
        outRoad4 = addOutPort<Vehicle>("outRoad4");

        outPorts = {outRoad1,outRoad2,outRoad3,outRoad4};
        // Store OD data route indices (max 4) that share the same origin/intersection
        for(int i = 0; i < odData.size(); i++){
            if(odData[i].origin == id){
                if(state.validRouteIndices.size()< 4){
                    state.validRouteIndices.push_back(i);
                }
            }
        }
        state.odData = odData;
    }

    void internalTransition(IntersectionState& state) const override {
        // Wait for next car to enter intersection.
        state.hasCar = false; 
        state.targetPortIndex = -1;
        state.selectedOdIndex = -1;
        state.sigma = infinity; 
    }

    void externalTransition(IntersectionState& state, double e) const override {
        // Car enters intersection.
        if (!inCar->getBag().empty()) {
            state.currentCar = inCar->getBag().back();
            state.hasCar = true;

            if(state.validRouteIndices.empty()){
                state.sigma = infinity;
                return;
            }

            state.targetPortIndex = selectRouteWithMaxFlow(state);
            if (state.targetPortIndex != -1) {
                state.selectedOdIndex = state.validRouteIndices[state.targetPortIndex];
                state.currentCar.routeIndex = state.selectedOdIndex;
                state.sigma = 0.0; // Output immediately
            } else {
                state.sigma = infinity;
            }
        }
    }

     void output(const IntersectionState& state) const override {
        if (state.hasCar && state.targetPortIndex >= 0 && state.targetPortIndex < outPorts.size()){
            outPorts[state.targetPortIndex]->addMessage(state.currentCar);
        }
    }

    [[nodiscard]] double timeAdvance(const IntersectionState& state) const override {     
        return state.sigma;
    }

private:
    int selectRouteWithMaxFlow(const IntersectionState& state) const {
        int bestPortIndex = -1;
        int maxFlow = -1;

        // Select the route with the highest flow rate for this intersection
        for(size_t i = 0; i < state.validRouteIndices.size(); i++) {
            int odIndex = state.validRouteIndices[i];
            int currentFlow = state.odData[odIndex].flowRate;

            if(currentFlow > maxFlow) {
             maxFlow = currentFlow;
             bestPortIndex = i;
            }
        }
        return bestPortIndex; 
    }
};

#endif // INTERSECTION_HPP
