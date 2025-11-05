#ifndef TOP_HPP
#define TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "customer.hpp"
#include "mes.hpp"
#include "shop_floor.hpp"

using namespace cadmium;

struct TopCoupled : public Coupled {

    TopCoupled(const std::string& id, std::vector<int> orders, double cellAssemblyTime) : Coupled(id) {
        auto customer = addComponent<Customer>("customer", orders);
        auto mes = addComponent<MES>("mes");
        auto shopFloor = addComponent<ShopFloor>("shopFloor", cellAssemblyTime);

        // Couple output ports to input ports
        addCoupling(customer->placeOrder, mes->placeOrder);
        addCoupling(mes->enterCell, shopFloor->enterCell);
        addCoupling(shopFloor->cellOperationEnd, mes->cellOperationEnd);
    }
};

#endif // TOP_HPP
