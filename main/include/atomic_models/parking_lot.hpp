#ifndef PARKING_LOT_HPP
#define PARKING_LOT_HPP

#include <iostream>
#include <queue>
#include "cadmium/modeling/devs/atomic.hpp"
#include "../constants.hpp"
#include "../data_structures/vehicle.hpp"

using namespace cadmium;

struct ParkingLotState {
    double initEventInSec;
    double periodInSec;
    int count;
    int totalEvents;

    explicit ParkingLotState(): count(0) {}
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const ParkingLotState& state) {
    return out << state.totalEvents - state.count; // Vehicles left in lot
}
#endif

// Atomic DEVS model of a ParkingLot which cars leave from periodically.
class ParkingLot : public Atomic<ParkingLotState> {
public:
    Port<Vehicle> exit;

    // ARGUMENTS
    // id - Model name.
    // schedules - Parking lot schedules.
    ParkingLot(const std::string id, std::vector<ParkingLotSchedule>& schedules): 
               Atomic<ParkingLotState>(id, ParkingLotState()) {
            exit = addOutPort<Vehicle>("exit");

            // Default
            state.initEventInSec = infinity;
            state.periodInSec = infinity;
            state.totalEvents = 0;

            // Filter
            for (ParkingLotSchedule s: schedules) {
                if (id.compare(s.id) == 0) {
                    state.initEventInSec = s.initEventInSec;
                    state.periodInSec = s.periodInSec;
                    state.totalEvents = s.totalEvents;
                }
            }
    }

    void internalTransition(ParkingLotState& state) const override {
        state.count++;
    }

	void externalTransition(ParkingLotState& state, double e) const override {}
    
    void output(const ParkingLotState& state) const override {
        exit->addMessage(Vehicle(this->id));
    }

    [[nodiscard]] double timeAdvance(const ParkingLotState& state) const override {     
        if (state.count < state.totalEvents) {
            if (state.count == 0) {
                return state.initEventInSec;
            } else {
                return state.periodInSec;
            }
        } else {
            return infinity;
        }
    }
};

#endif // PARKING_LOT_HPP
