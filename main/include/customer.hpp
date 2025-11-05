#ifndef CUSTOMER_HPP
#define CUSTOMER_HPP

#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"
#include "constants.hpp"
#include "event.hpp"

using namespace cadmium;

struct CustomerState {
    double sigma;
    std::vector<int> orderPlacementTimes;    // Sorted internally in descending order 
                                             // and relative to each other.

    explicit CustomerState(): sigma(infinity) {}
};

#ifndef NO_LOGGING
// Formats the state log. This model is considered stateless.
std::ostream& operator<<(std::ostream &out, const CustomerState& state) {
    return out;
}
#endif

// Atomic DEVS model of a Customer who places orders to the system.
class Customer : public Atomic<CustomerState> {
public:
    Port<Event> placeOrder;

    // ARGUMENTS
    // id - Model name.
    // orderPlacementTimes - Order placement times sorted in ascending order, i.e., the first order to place is the 
    //                       first element and the last order to place is the last element. 
    Customer(const std::string id, std::vector<int> orderPlacementTimes) : Atomic<CustomerState>(id, CustomerState()) {
        placeOrder = addOutPort<Event>("placeOrder");

        if (!orderPlacementTimes.empty()) {
            // Sort orders in descending order, makes working with std::vector<> easier.
            while (!orderPlacementTimes.empty()) {
                state.orderPlacementTimes.push_back(orderPlacementTimes.back());
                orderPlacementTimes.pop_back();
            }
			
            // Change order placement times to be relative to one another, eliminating the need 
            // to have a 'placeholder' state variable and making time advance calculations easier.
            //
            // For example, on the left we have lists of 'absolute' order placement times sorted in 
            // descending order, and on the right we have list of 'relative' order placement times 
            // sorted in descending order. 
            //
            // [ 2, 1, 0 ] -> [ 1, 1, 0 ]
            // [ 4, 2, 1 ] -> [ 2, 1, 1 ]
            for (int i = 0; i < state.orderPlacementTimes.size() - 1; i++) {
                state.orderPlacementTimes[i] = state.orderPlacementTimes[i] - state.orderPlacementTimes[i + 1];
            }

            state.sigma = state.orderPlacementTimes.back();
            state.orderPlacementTimes.pop_back();
        } 
    }

    void internalTransition(CustomerState& state) const override {
        if (!state.orderPlacementTimes.empty()) {
            state.sigma = state.orderPlacementTimes.back();
            state.orderPlacementTimes.pop_back();
        } else {
            state.sigma = infinity;
        }
    }

	void externalTransition(CustomerState& state, double e) const override {}
    
    void output(const CustomerState& state) const override {
        // Create unique order ID
        static int orderID = 0;
        orderID++;

        placeOrder->addMessage(Event(orderID, this->id, placeOrderActivity));
    }

    [[nodiscard]] double timeAdvance(const CustomerState& state) const override {     
        return state.sigma;
    }
};

#endif // CUSTOMER_HPP
