#ifndef PARKING_LOT_HPP
#define PARKING_LOT_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"
#include "constants.hpp"

using namespace cadmium;

struct ParkingLotState {
    double sigma;
    int numCars;                           // Number of cars in the parking lot.
    std::vector<int> carDepartureTimes;    // Sorted internally in descending order 
                                           // and relative to each other.

    explicit ParkingLotState(): sigma(infinity) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const ParkingLotState& state) {
    return out << state.numCars;
}
#endif

// Atomic DEVS model of a ParkingLot which cars depart from.
class ParkingLot : public Atomic<ParkingLotState> {
public:

    // ARGUMENTS
    // id - Model name.
    // carDepartureTimes - Car departure times sorted in ascending order, i.e., the first car to depart is the
    //                     first element and the last car to leave is the last element. 
    ParkingLot(const std::string id, std::vector<int> carDepartureTimes) : Atomic<ParkingLotState>(id, ParkingLotState()) {
        if (!carDepartureTimes.empty()) {

            // Sort orders in descending order, makes working with std::vector<> easier.
            while (!carDepartureTimes.empty()) {
                state.carDepartureTimes.push_back(carDepartureTimes.back());
                carDepartureTimes.pop_back();
            }
			
            // Change carDepartureTimes to be relative to one another.
    	    //
    	    // Eliminates the need to have a 'placeholder' state variable and makes time advance calculations easier.
    	    // 
            // For example, on the left we have lists of 'absolute' order placement times sorted in 
            // descending order, and on the right we have list of 'relative' order placement times 
            // sorted in descending order. 
            //
            // [ 2, 1, 0 ] -> [ 1, 1, 0 ]
            // [ 4, 2, 1 ] -> [ 2, 1, 1 ]
            for (int i = 0; i < state.carDepartureTimes.size() - 1; i++) {
                state.carDepartureTimes[i] = state.carDepartureTimes[i] - state.carDepartureTimes[i + 1];
            }

            state.numCars = state.carDepartureTimes.size();
            state.sigma = state.carDepartureTimes.back();
            state.carDepartureTimes.pop_back();
        } 
    }

    void internalTransition(ParkingLotState& state) const override {
        state.numCars = state.carDepartureTimes.size();
        if (!state.carDepartureTimes.empty()) {
            state.sigma = state.carDepartureTimes.back();
            state.carDepartureTimes.pop_back();
        } else {
            state.sigma = infinity;
        }
    }

	void externalTransition(ParkingLotState& state, double e) const override {}
    
    void output(const ParkingLotState& state) const override {
    }

    [[nodiscard]] double timeAdvance(const ParkingLotState& state) const override {     
        return state.sigma;
    }
};

#endif // PARKING_LOT_HPP
