#ifndef TOP_HPP
#define TOP_HPP

#include <vector>
#include <map>
#include <set>
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
       
        std::map<std::string, std::vector<std::string>> topology;
        std::set<std::string> allIntersections;

        // Build topology map from OD data and collect all intersection ids
        for(const auto& data : odData){
            topology[data.origin].push_back(data.dest);
           
            allIntersections.insert(data.origin);
            allIntersections.insert(data.dest);
            
        }
        
        // Create intersection components for all unique intersections
        std::map<std::string, std::shared_ptr<Intersection>> intersectionModels;
        for (const auto& intersectId : allIntersections) {
            intersectionModels[intersectId] = addComponent<Intersection>(intersectId, odData);
        }
        int roadCounter = 0;

        // Create road components and couple intersections based on topology
        for (auto const& [originId, destinations] : topology) {
            auto originModel = intersectionModels[originId];
          
            for (size_t i = 0; i < destinations.size(); ++i) {
                std::string destId = destinations[i];
                auto destModel = intersectionModels[destId];

                // Road from origin to destination
                std::string roadName = "road_" + originId + "_to_" + destId; 
                auto road = addComponent<Road>(roadName, 100, 30);

                // Connect origin intersection output port to road entrance
                if (i == 0) addCoupling(originModel->outRoad1, road->entrance);
                else if (i == 1) addCoupling(originModel->outRoad2, road->entrance);
                else if (i == 2) addCoupling(originModel->outRoad3, road->entrance);
                else if (i == 3) addCoupling(originModel->outRoad4, road->entrance);
                else std::cerr << "Error" << std::endl;
                
                // Connect road exit to destination intersection input
                addCoupling(road->exit, destModel->inCar);
            }
        }
        if (intersectionModels.count("A")) {
            addCoupling(parkingLot->exit, intersectionModels["A"]->inCar);
        }

        /* IT WILL DELETED AFTER REVIEWING
        CREATED A MAP FOR THIS LOGIC
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
        */
    }
};
#endif // TOP_HPP
