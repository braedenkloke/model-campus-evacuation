#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cassert>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../data_structures/od_datum.hpp"
#include "../data_structures/vehicle.hpp"
#include "../constants.hpp"
#include "../data_structures/vehicle.hpp"

using namespace cadmium;

struct IntersectionState {
    double sigma;
    std::vector<ODDatum> odData;
    bool hasCar;                        // Is there a car waiting in intersection
    Vehicle currentCar;                 // The car currently in the intersection
    std::vector<std::string> outRoads;

    explicit IntersectionState(): sigma(infinity), hasCar(false) {} 

}; 

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const IntersectionState& state) {
    return out << "HasCar: " << state.hasCar;
}
#endif

// Atomic DEVS model of a road intersection. Routes cars entering the intersection
// by selecting the destination with the highest flow rate.
class Intersection : public Atomic<IntersectionState> {
public:
    Port<Vehicle> in;               // Incoming car from a road model.
    Port<Vehicle> out1;            // Car exits through one of 4 out ports
    Port<Vehicle> out2; 
    Port<Vehicle> out3; 
    Port<Vehicle> out4;  
    bool verbose;   // Prints debugging statements when true.
   
    // ARGUMENTS
    // id - Model name. Equivalent to the origin in the OD data.
    // odData - Origin-destination (OD) data.
    // outRoads - Ordered list of road names that have traffic flowing out of the intersection.
    //            Output ports are allocated to each road name in the order given, i.e., outRoads.first() -> out1.
    Intersection(const std::string id, const std::vector<ODDatum>& odData, const std::vector<std::string>& outRoads): 
                 Atomic<IntersectionState>(id, IntersectionState()) {
        verbose = false; 

        in = addInPort<Vehicle>("in");
        
        out1 = addOutPort<Vehicle>("out1");
        out2 = addOutPort<Vehicle>("out2");
        out3 = addOutPort<Vehicle>("out3");
        out4 = addOutPort<Vehicle>("out4");

        // Filter OD data
        for(int i = 0; i < odData.size(); i++){
            if(odData[i].origin == id){
                state.odData.push_back(odData[i]);
            }
        }
        assert(outRoads.size() <= 4);
        state.outRoads = outRoads;
    }

    void internalTransition(IntersectionState& state) const override {
        // Wait for next car to enter intersection.
        state.hasCar = false; 
        state.sigma = infinity; 
    }

    void externalTransition(IntersectionState& state, double e) const override {
        // Car enters intersection.
        if (!in->getBag().empty()) {
            Vehicle v = in->getBag().back();
            state.hasCar = true;
            v.dest = selectDest(state);
            state.currentCar = v;
            state.sigma = 0.0; // Output immediately
        } else {
            state.sigma = infinity;
        }
    }

     void output(const IntersectionState& state) const override {
        int o = getOutputPortID(state);
        if (verbose) {std::cout << this->id << " routing vehicle to out" << o << "\n|";}
        if (o == 1){
            out1->addMessage(state.currentCar);
        } else if (o == 2) {
            out2->addMessage(state.currentCar);
        } else if (o == 3) {
            out3->addMessage(state.currentCar);
        } else if (o == 4) {
            out4->addMessage(state.currentCar);
        }
    }

    [[nodiscard]] double timeAdvance(const IntersectionState& state) const override {     
        return state.sigma;
    }

private:
    std::string selectDest(const IntersectionState& state) const {
        // Temporary solution: Choose random destination
        std::string dest = "";
        if (!state.odData.empty()) {
            int i = std::rand() % state.odData.size();
            if(verbose) {std::cout << "odData size: " << state.odData.size() << "Random i: " << i << "\n";}
            dest = state.odData[i].dest;
        }
        return dest;
    }

    // Returns ID of output port. Returns 0 if no ID found.
    int getOutputPortID(const IntersectionState& state) const {
        std::string target = state.currentCar.dest;
        int counter = 0;
        int id = counter;

        for (std::string r : state.outRoads) {
            if(verbose) {std::cout << "Comparing Target: " << target << " with OutRoad: "  << r << "\n";}
            counter++;
            if (target.compare(r) == 0) {
                id = counter;
            }
        }
        return id;
    }
};

#endif // INTERSECTION_HPP
