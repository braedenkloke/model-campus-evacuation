#include <limits>
#include "include/top.hpp"
#include "cadmium/simulation/root_coordinator.hpp"
#include "cadmium/simulation/logger/stdout.hpp"
#include "cadmium/simulation/logger/csv.hpp"

using namespace cadmium;

int main(int argc, char* argv[]) {
    std::ifstream f;
    std::vector<int> carDepartureTimes;

    // Configure simulation
    std::string inputFile = "input_data/parking_lot_schedules/parking_lot_schedule_03_three_cars.txt";
    std::string outputFile = "output_data/raw/manufacturing_system_log.csv";
    double maxSimulationTime = 30.0;

    // Parse command line arguments
    if (argc == 2) {
        inputFile = argv[1];
    } else if (argc > 2) {
        std::cout << "Invalid number of arguments ... aborting gracefully.";
        return 1;
    }

    // Load orders
    f.open(inputFile);
    int carDeparture;
    while (f >> carDeparture) {
        carDepartureTimes.push_back(carDeparture);
    }

    auto model = std::make_shared<TopCoupled>("top", carDepartureTimes);
    auto rootCoordinator = cadmium::RootCoordinator(model);

    rootCoordinator.setLogger<STDOUTLogger>(",");
    //rootCoordinator.setLogger<CSVLogger>(outputFile, ",");
    rootCoordinator.start();
    rootCoordinator.simulate(maxSimulationTime);
    rootCoordinator.stop();	
}
