#ifndef PARKING_LOT_HPP
#define PARKING_LOT_HPP

#include <iostream>
#include <queue>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../constants.hpp"

using namespace cadmium;

struct ParkingLotState {
    double sigma;
    int numCars;                           // Number of cars in the parking lot.
    std::queue<int> carDepartureTimes;     // Relative wait times between car departures.

    explicit ParkingLotState(): sigma(infinity), numCars(0) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const ParkingLotState& state) {
    return out << state.numCars;
}
#endif

// Atomic DEVS model of a ParkingLot which cars depart from.
class ParkingLot : public Atomic<ParkingLotState> {
public:
    Port<int> exit;

    // ARGUMENTS
    // id - Model name.
    // carDepartureTimes - The times that cars leave the parking lot, sorted in ascending order.
    ParkingLot(const std::string id, std::vector<int> carDepartureTimes): Atomic<ParkingLotState>(id, ParkingLotState()) {
        exit = addOutPort<int>("exit");

        if (!carDepartureTimes.empty()) {
            // Convert departure times to be relative to the previous car's departure time.
            // Simplifies time advance calculations.
            //
            // For example:
            // 
            // [ 0, 1, 2 ] --> [ 0, 1, 1 ]
            // [ 1, 2, 3 ] --> [ 1, 1, 1 ]
            int prevDepartureTime = 0; 
            for(int i = 0; i < carDepartureTimes.size(); i++){
                int relativeDepartureTime = carDepartureTimes[i] - prevDepartureTime;
                state.carDepartureTimes.push(relativeDepartureTime);
                prevDepartureTime = carDepartureTimes[i];
            }
           
            state.numCars = state.carDepartureTimes.size();
			state.sigma = state.carDepartureTimes.front();
            state.carDepartureTimes.pop();
        } 
    }

    void internalTransition(ParkingLotState& state) const override {
        state.numCars = state.carDepartureTimes.size();
        if(!state.carDepartureTimes.empty()){
            state.sigma = state.carDepartureTimes.front();
            state.carDepartureTimes.pop();
        } else {
            state.sigma = infinity;
        }
    }

	void externalTransition(ParkingLotState& state, double e) const override {}
    
    void output(const ParkingLotState& state) const override {
        static int carID = 0;
        carID++;
        exit->addMessage(carID); 
    }

    [[nodiscard]] double timeAdvance(const ParkingLotState& state) const override {     
        return state.sigma;
    }
};

#endif // PARKING_LOT_HPP
