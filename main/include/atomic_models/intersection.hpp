#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <iostream>
#include <vector>
#include <string>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../data_structures/od_datum.hpp"
#include "../constants.hpp"

using namespace cadmium;

struct IntersectionState {
    double sigma;
    std::vector<ODDatum> odData; 
    bool hasCar;           // Is there a car waiting to be processed in intersection
    int currentCarId;      // The ID of the arrived car to the intersection
    int selectedRouteId;   // Chosen route for that car (as ID)

    explicit IntersectionState(): sigma(infinity), hasCar(false), currentCarId(-1), selectedRouteId(-1) {} 

};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const IntersectionState& state) {
   return out << "HasCar:"<< state.hasCar << ",RouteId:" << state.selectedRouteId;
}
#endif


class Intersection : public Atomic<IntersectionState> {
public:
    std::string origin; 
    Port<int> inCar;               // Incoming car from a road model
                                   // Using roads car information for intresection
    Port<int> outSelectedRouteId;  // Select route id for sending to the coupling model (top)
   
    // Constructor
    Intersection(const std::string id, const std::vector<ODDatum>& odData) : Atomic<IntersectionState>(id, IntersectionState()) {
        
        inCar = addInPort<int>("inCar");
        outSelectedRouteId = addOutPort<int>("outForSelectedRoute");
        state.odData = odData;

        // If the OD file isn't empty, use the first row to set the origin (for now)
        if(!odData.empty()) {
            origin = odData[0].origin; 
        } 
    }

    // Goes back to sleeping after sending output
    void internalTransition(IntersectionState& state) const override {
        state.hasCar = false; 
        state.sigma = infinity; 
    }

    // Triggered whenever a car enters to the intersection
    void externalTransition(IntersectionState& state, double e) const override {
        
        if (!inCar->getBag().empty()) {
            state.currentCarId = inCar->getBag().back();
            state.hasCar = true;

            // Pick the best route (based on origin)
            state.selectedRouteId = selectRouteWithMaxFlow(state.odData);

            // To get response immediately
            state.sigma = 0.0; 
        } else {
             if(state.sigma != infinity){
                 state.sigma -= e;
            }
        }
    }

    // Output log
     void output(const IntersectionState& state) const override {
       
        if (state.hasCar && state.selectedRouteId != -1) {
            /*std::cout << "[Intersection " << origin << "] Car #" << state.currentCarId 
                      << " arrived. Selecting Route ID: " << state.selectedRouteId 
                      << " (Max Flow)" << std::endl;*/
            
            outSelectedRouteId->addMessage(state.selectedRouteId);
        }
    }
   

    [[nodiscard]] double timeAdvance(const IntersectionState& state) const override {     
        return state.sigma;
    }

private:
    int selectRouteWithMaxFlow(const std::vector<ODDatum>& data) const {
        int bestID = -1;
        int maxFlow = -1;

        // Select the route with the highest flow rate for this intersection
        for(size_t i = 0; i < data.size(); i++) {
            
            // Only check entries that match this intersection's origin
            if(data[i].origin == origin) {
                
                // Choose the highest flow value
                if(data[i].flowRate > maxFlow) {
                    maxFlow = data[i].flowRate; 
                    bestID = data[i].id;        
                }
            }
        }
        return bestID; 
    }
};

#endif // INTERSECTION_HPP
