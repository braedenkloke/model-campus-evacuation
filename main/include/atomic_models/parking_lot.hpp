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
    std::queue<int> carDepartureWaitTimes; // Relative wait times between car departures 
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
    // carDepartureTimes - A std::vector<int> of absolute departure times.
    //                     This vector must be in ascending order. (taken from input file)
    ParkingLot(const std::string id, std::vector<int> carDepartureTimes) : Atomic<ParkingLotState>(id, ParkingLotState()) {
        exit = addOutPort<int>("exit");
        // Total car counter from the size of the input vector.
        state.numCars = carDepartureTimes.size();

        if (!carDepartureTimes.empty()) {
            int lastTime = 0; // Tracks the time of previous departure
           
            // Relative Time Calculation //
            // This loop converting absolute time (vector) to relative wait time (queue)
            for(int i = 0; i < carDepartureTimes.size(); i++){
                int absoluteTime = carDepartureTimes[i];
                // Calculate the time between ith car and the last car.
                int waitTime = absoluteTime - lastTime;
                state.carDepartureWaitTimes.push(waitTime);
                lastTime = absoluteTime;
            }
           
			state.sigma = state.carDepartureWaitTimes.front();
            state.carDepartureWaitTimes.pop();

            /* --- WHY QUEUE? ---
            - Using queue in the parking lot simplify reversing the list with its FIFO nature.
           
            - It takes the simple input list [0, 1, 2] then it calculates the relative wait times 
            forward to create a queue [0, 1, 1].
           
            - Afer that it reads events from the front of the queue and deletes them with pop.
           
            - The parking lot model's job is static. It only needs to process a time schedule.*/ 
        } 
    }

    void internalTransition(ParkingLotState& state) const override {
        state.numCars--;
        // After decreasing if there are still cars in the parking lot
        // it takes next wait time and set to sigma
        if(!state.carDepartureWaitTimes.empty()){
            state.sigma = state.carDepartureWaitTimes.front();
            state.carDepartureWaitTimes.pop();
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
