#ifndef MES_HPP
#define MES_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"
#include "constants.hpp"
#include "event.hpp"

using namespace cadmium;

struct MESState {
    std::vector<int> orders; // FIFO queue containing order IDs
    bool cellIsIdle;

    explicit MESState(): cellIsIdle(true) {}
};

#ifndef NO_LOGGING
// Formats the state log. This model is considered stateless.
std::ostream& operator<<(std::ostream &out, const MESState& state) {
    return out;
}
#endif

// Atomic DEVS model of a Manufacturing Execution System (MES).
class MES : public Atomic<MESState> {
public:
    Port<Event> placeOrder, enterCell, cellOperationEnd;

    MES(const std::string id) : Atomic<MESState>(id, MESState()) {
        // Input ports
        placeOrder = addInPort<Event>("placeOrder");
        cellOperationEnd = addInPort<Event>("cellOperationEnd");

        // Output ports
        enterCell = addOutPort<Event>("enterCell");
    }

    void internalTransition(MESState& state) const override {
        state.cellIsIdle = false;
    }

    void externalTransition(MESState& state, double e) const override {
        // Handle new orders
        if (!placeOrder->empty()) {
            // Add order to end of queue
            int orderID = placeOrder->getBag().back().orderID;
            state.orders.push_back(orderID);
        }
        
        if (!cellOperationEnd->empty()) {
            state.cellIsIdle = true;
            // Remove order from front of queue
            for (int i = 0; i < state.orders.size() - 1; i++) {
                state.orders[i] = state.orders[i + 1];
            }
            state.orders.pop_back();
        }
    }
    
    void output(const MESState& state) const override {
        enterCell->addMessage(Event(state.orders.front(), this->id, enterCellActivity));
    }

    [[nodiscard]] double timeAdvance(const MESState& state) const override {     
        if (state.cellIsIdle && !state.orders.empty()) {
            return 0;
        } else {
            return infinity;
        }
    }
};

#endif // MES_HPP
