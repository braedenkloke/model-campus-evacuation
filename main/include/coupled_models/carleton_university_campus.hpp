#ifndef CARLETON_UNIVERSITY_CAMPUS_HPP
#define CARLETON_UNIVERSITY_CAMPUS_HPP

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

struct CarletonUniversityCampusCoupled : public Coupled {

    CarletonUniversityCampusCoupled(const std::string& id, std::vector<int> carDepartureTimes, 
                                    const std::vector<ODDatum>& odData ): Coupled(id) {
        auto parkingLot = addComponent<ParkingLot>("parking lot", carDepartureTimes);
        // A -> [B,C,D]
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
                auto road = addComponent<Road>(roadName, 100, 30, originId, destId);

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
    }
};
#endif // CARLETON_UNIVERSITY_CAMPUS_HPP
