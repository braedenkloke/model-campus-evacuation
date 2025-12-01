#ifndef TOP_HPP
#define TOP_HPP

#include <vector>
#include <string>
#include "cadmium/modeling/devs/coupled.hpp"
#include "../atomic_models/parking_lot.hpp"
#include "../atomic_models/road.hpp"
#include "../atomic_models/intersection.hpp"
#include "../data_structures/od_datum.hpp"

using namespace cadmium;

struct TopCoupled : public Coupled {

    TopCoupled(const std::string& id, std::vector<int> carDepartureTimes, const std::vector<ODDatum>& odData ): Coupled(id) {
        auto parkingLot = addComponent<ParkingLot>("parking lot", carDepartureTimes);
       
        auto road1 = addComponent<Road>("road_1", 100, 30);
        auto road2 = addComponent<Road>("road_2", 100, 30);
        auto road3 = addComponent<Road>("road_3", 100, 30);
        auto road4 = addComponent<Road>("road_4", 100, 30);
        auto intersectionA = addComponent<Intersection>("A", odData);
        auto intersectionB = addComponent<Intersection>("B", odData);
        auto intersectionC = addComponent<Intersection>("C", odData);
        auto intersectionD = addComponent<Intersection>("D", odData);
        auto intersectionE = addComponent<Intersection>("E", odData);
    

        // Couple output ports to input ports
        addCoupling(parkingLot->exit, intersectionA->inCar);

        addCoupling(intersectionA->outRoad1, road1->entrance);
        addCoupling(road1->exit, intersectionB->inCar);

        addCoupling(intersectionA->outRoad2, road2->entrance);
        addCoupling(road2->exit, intersectionC->inCar);

        addCoupling(intersectionA->outRoad3, road3->entrance);
        addCoupling(road3->exit, intersectionD->inCar);

        addCoupling(intersectionA->outRoad4, road4->entrance);
        addCoupling(road4->exit, intersectionE->inCar);
    }
};
#endif // TOP_HPP
