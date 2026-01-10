#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../data_structures/od_datum.hpp"
#include "../data_structures/vehicle.hpp"
#include "../constants.hpp"
#include "../data_structures/vehicle.hpp"

using namespace cadmium;

struct IntersectionState {
    std::vector<ODDatum> odData;
    std::vector<Vehicle> vehicles;
    std::vector<std::string> outRoads;

    explicit IntersectionState() {} 

}; 

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const IntersectionState& state) {
    return out << "Number of vehicles in intersection: " << state.vehicles.size();
}
#endif

// Atomic DEVS model of a road intersection. Routes cars entering the intersection
// by selecting the destination with the highest flow rate.
class Intersection : public Atomic<IntersectionState> {
public:
    Port<Vehicle> in;
    Port<Vehicle> out1;
    Port<Vehicle> out2; 
    Port<Vehicle> out3; 
    Port<Vehicle> out4;  
   
    // ARGUMENTS
    // id - Model name. Equivalent to the origin in the OD data.
    // odData - Origin-destination (OD) data.
    // outRoads - Ordered list of road names that have traffic flowing out of the intersection.
    //            Output ports are allocated to each road name in the order given, i.e., outRoads.first() -> out1.
    Intersection(const std::string id, const std::vector<ODDatum>& odData, const std::vector<std::string>& outRoads): 
                 Atomic<IntersectionState>(id, IntersectionState()) {
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
        // Wait for next set of vehicles to enter intersection.
        state.vehicles.clear();
    }

    void externalTransition(IntersectionState& state, double e) const override {
        // Vehicles enters intersection.
        if (!in->getBag().empty()) {
            for (Vehicle v : in->getBag()) {
                v.dest = selectDest(state);
                state.vehicles.push_back(v);
            }
        } 
    }

     void output(const IntersectionState& state) const override {
        for (Vehicle v : state.vehicles) {
            int o = getOutputPortID(v, state.outRoads);
            if (o == 1){
                out1->addMessage(v);
            } else if (o == 2) {
                out2->addMessage(v);
            } else if (o == 3) {
                out3->addMessage(v);
            } else if (o == 4) {
                out4->addMessage(v);
            }
        }
    }

    [[nodiscard]] double timeAdvance(const IntersectionState& state) const override {     
        if (!state.vehicles.empty()) {
            return 0.0;
        } else {
            return infinity;
        }
    }

private:
    std::string selectDest(const IntersectionState& state) const {
        // Temporary solution: Choose random destination
        std::string dest = "";
        if (!state.odData.empty()) {
            int i = std::rand() % state.odData.size();
            dest = state.odData[i].dest;
        }
        return dest;
    }

    // Returns ID of output port. Returns 0 if no ID found.
    int getOutputPortID(const Vehicle v, const std::vector<std::string>& outRoads) const {
        std::string target = v.dest;
        int counter = 0;
        int id = counter;

        for (std::string r : outRoads) {
            counter++;
            if (target.compare(r) == 0) {
                id = counter;
            }
        }
        return id;
    }
};

#endif // INTERSECTION_HPP
