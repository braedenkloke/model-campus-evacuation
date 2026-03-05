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
            if(odData[i].origin == id and odData[i].flowRate != 0){
                state.odData.push_back(odData[i]);
            }
        }
        assert(outRoads.size() <= 4);
        state.outRoads = outRoads;
        //assert(state.outRoads.size() == state.odData.size());
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
    // Returns a random destination based on the probability distribution of the OD data.
    std::string selectDest(const IntersectionState& state) const {
        std::string dest = "";
        if (state.odData.empty()) {
            // Do nothing
        } else if (state.odData.size() == 1) {
            dest = state.odData.front().dest;
        } else {
            std::vector<ODDatum> data = state.odData;

            // Make flow rates relative to one another: [1, 1, 1, 1] -> [1, 2, 3, 4]
            for (int i = 1; i < data.size(); i++) {
                assert((data[i].flowRate != 0) && "Assume OD data does not have flow rates equal to zero");
                data[i].flowRate = data[i - 1].flowRate + data[i].flowRate;
            }

            // Choose random number in between flow rates (0 .. max + 1)
            int r = std::rand() % data.back().flowRate + 1;

            // Select destination
            int i = 0;
            while (dest.empty() and i < data.size()) {
                if (r <= data[i].flowRate) {
                    dest = data[i].dest;
                }
                i++;
            }
            assert(!dest.empty());
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
