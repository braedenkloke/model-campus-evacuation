#ifndef SHOP_FLOOR_HPP
#define SHOP_FLOOR_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"
#include "constants.hpp"
#include "event.hpp"

using namespace cadmium;

// Prefixed with class name to avoid namespace collisions from models with similarly named phases.
enum ShopFloorPhase {
    // Sorted alphabetically.
    cellBusy,
    cellIdle
};

struct ShopFloorState {
    ShopFloorPhase phase;
    int orderID;
    double assemblyTime;

    explicit ShopFloorState(): phase(cellIdle), orderID(-1) {}
};

#ifndef NO_LOGGING
// Formats the state log.
std::ostream& operator<<(std::ostream &out, const ShopFloorState& state) {
    out << "stateLog,";
    if (state.phase == cellIdle) {
        out << "idle";
    } else if (state.phase == cellBusy) {
        out << "busy";
    }
    return out;
}
#endif

// Atomic DEVS model of a shop floor.
class ShopFloor : public Atomic<ShopFloorState> {
public:
    Port<Event> enterCell, cellOperationEnd;

    // ARGUMENTS
    // id - Model name.
    // assemblyTime - Time in seconds it takes for the cell to assemble a product.
    ShopFloor(const std::string id, double assemblyTime) : Atomic<ShopFloorState>(id, ShopFloorState()) {
        enterCell = addInPort<Event>("enterCell");
        cellOperationEnd = addOutPort<Event>("cellOperationEnd");

        state.assemblyTime = assemblyTime;
    }

    void internalTransition(ShopFloorState& state) const override {
        state.phase = cellIdle;
        state.orderID = -1;
    }

    void externalTransition(ShopFloorState& state, double e) const override {
        if (state.phase == cellIdle && !enterCell->empty()) {
            state.orderID = enterCell->getBag().back().orderID;
            state.phase = cellBusy;
        }
    }
    
    void output(const ShopFloorState& state) const override {
        cellOperationEnd->addMessage(Event(state.orderID, this->id, cellOperationEndActivity));
    }

    [[nodiscard]] double timeAdvance(const ShopFloorState& state) const override {     
        if (state.phase == cellBusy) {
            return state.assemblyTime;
        } else {
            return infinity;
        }
    }
};

#endif // SHOP_FLOOR_HPP
