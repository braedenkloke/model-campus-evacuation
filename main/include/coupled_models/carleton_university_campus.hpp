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
        // Road names, as per documentation in the final report.
        std::string r01Name = "Library Rd & P1 to Library Rd & University Dr";
        std::string r02Name = "Library Rd & P1 to Campus Ave & Library Rd";
        std::string r03Name = "Library Rd & University Dr to Library Rd & P1";
        std::string r04Name = "Campus Ave & Library Rd to Library Rd & P1";
        std::string r05Name = "Campus Ave & Library Rd to Campus Ave & P2";
        std::string r06Name = "Campus Ave & P2 to Campus Ave & University Dr";
        std::string r07Name = "Campus Ave & University Dr to Library Rd & University Dr";
        std::string r08Name = "Library Rd & University Dr to Campus Ave & University Dr";
        std::string r09Name = "Campus Ave & University Dr to Raven Rd & University Dr";
        std::string r10Name = "Raven Rd & University Dr to Campus Ave & University Dr";
        std::string r11Name = "Raven Rd & University Dr to P3 & Raven Rd";
        std::string r12Name = "P3 & Raven Rd to Raven Rd & University Dr";

        // Out roads
        std::vector<std::string> x01OutRoads = {r01Name, r02Name};
        std::vector<std::string> x02OutRoads = {r03Name, r08Name};
        std::vector<std::string> x03OutRoads = {r04Name, r05Name};
        std::vector<std::string> x04OutRoads = {r06Name};
        std::vector<std::string> x05OutRoads = {r07Name, r09Name};
        std::vector<std::string> x06OutRoads = {r10Name, r11Name};
        std::vector<std::string> x07OutRoads = {r12Name};

        // Create models
        auto p1 = addComponent<ParkingLot>("P1", carDepartureTimes);
        auto p2 = addComponent<ParkingLot>("P2", carDepartureTimes);
        //auto p3 = addComponent<ParkingLot>("P3", carDepartureTimes);
        auto x01 = addComponent<Intersection>("Library Rd & P1", odData, x01OutRoads);
        auto x02 = addComponent<Intersection>("Library Rd & University Dr", odData, x02OutRoads);
        auto x03 = addComponent<Intersection>("Campus Ave & Library Rd", odData, x03OutRoads);
        auto x04 = addComponent<Intersection>("Campus Ave & P2", odData, x04OutRoads);
        auto x05 = addComponent<Intersection>("Campus Ave & University Dr", odData, x05OutRoads);
        auto x06 = addComponent<Intersection>("Raven Rd & University Dr", odData, x06OutRoads);
        auto x07 = addComponent<Intersection>("P3 & Raven Rd", odData, x07OutRoads);
        auto r01 = addComponent<Road>(r01Name);
        auto r02 = addComponent<Road>(r02Name);
        auto r03 = addComponent<Road>(r03Name);
        auto r04 = addComponent<Road>(r04Name);
        auto r05 = addComponent<Road>(r05Name);
        auto r06 = addComponent<Road>(r06Name);
        auto r07 = addComponent<Road>(r07Name);
        auto r08 = addComponent<Road>(r08Name);
        auto r09 = addComponent<Road>(r09Name);
        auto r10 = addComponent<Road>(r10Name);
        auto r11 = addComponent<Road>(r11Name);
        auto r12 = addComponent<Road>(r12Name);

        // Couple intersection inputs
        addCoupling(p1->exit, x01->in);
        addCoupling(r03->exit, x01->in);
        addCoupling(r04->exit, x01->in);
        addCoupling(r01->exit, x02->in);
        addCoupling(r07->exit, x02->in);
        addCoupling(r02->exit, x03->in);
        addCoupling(p2->exit, x04->in);
        addCoupling(r05->exit, x04->in);
        addCoupling(r06->exit, x05->in);
        addCoupling(r08->exit, x05->in);
        addCoupling(r10->exit, x05->in);
        addCoupling(r09->exit, x06->in);
        addCoupling(r12->exit, x06->in);
        addCoupling(r11->exit, x07->in);

        // Couple intersection outputs
        addCoupling(x01->out1, r01->entrance);
        addCoupling(x01->out2, r02->entrance);
        addCoupling(x02->out1, r03->entrance);
        addCoupling(x03->out1, r04->entrance);
        addCoupling(x03->out2, r05->entrance);
        addCoupling(x04->out1, r06->entrance);
        addCoupling(x05->out1, r07->entrance);
        addCoupling(x05->out2, r09->entrance);
        addCoupling(x06->out1, r10->entrance);
        addCoupling(x06->out2, r11->entrance);
        addCoupling(x07->out1, r12->entrance);
    }
};
#endif // CARLETON_UNIVERSITY_CAMPUS_HPP
