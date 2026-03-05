#include "cadmium/simulation/root_coordinator.hpp"
#include "cadmium/simulation/logger/stdout.hpp"
#include "cadmium/modeling/devs/coupled.hpp"

#include "include/atomic_models/parking_lot.hpp"
#include "include/atomic_models/intersection.hpp"
#include "include/data_structures/parking_lot_schedule.hpp"
#include "include/data_structures/od_datum.hpp"
#include "include/io/load_data.hpp"

using namespace cadmium;

struct IntersectionTest: public Coupled {

    IntersectionTest(const std::string& id, std::vector<ParkingLotSchedule> schedules, 
                     std::vector<ODDatum> odData): Coupled(id) {
        std::vector<std::string> outRoads = {"A", "B", "C", "D"};

        auto p = addComponent<ParkingLot>("P1", schedules);
        auto i = addComponent<Intersection>("X", odData, outRoads);
    
        addCoupling(p->exit, i->in);
    }

};

int main(int argc, char* argv[]) {
    std::string parkingLotSchedulesFile = "input_data/parking_lot_schedules/test.csv";
    std::string odFile = "input_data/od_data/abc_test.csv";
    double maxSimulationTime = 10000;

    // Load data
    std::vector<ParkingLotSchedule> schedules = loadParkingLotSchedules(parkingLotSchedulesFile);
    std::vector<ODDatum> odData = loadODData(odFile);

    // Configure model & simulator
    auto model = std::make_shared<IntersectionTest>("IntersectionTest", schedules, odData);
    auto rootCoordinator = cadmium::RootCoordinator(model);
    rootCoordinator.setLogger<STDOUTLogger>(",");

    // Run simulation
    rootCoordinator.start();
    rootCoordinator.simulate(maxSimulationTime);
    rootCoordinator.stop();	
}
