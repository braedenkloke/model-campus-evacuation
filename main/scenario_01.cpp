#include <limits>
#include "include/data_structures/od_datum.hpp"
#include "include/coupled_models/carleton_university_campus.hpp"
#include "include/io/load_data.hpp"
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
    double maxSimulationTime = 10000.0;
    std::string odFile = "input_data/od_data/campus_traffic_flow_01.csv";

    // Load data
    f.open(inputFile);
    int carDeparture;
    while (f >> carDeparture) {
        carDepartureTimes.push_back(carDeparture);
    }
    // Load OD data
    std::vector<ODDatum> odData = loadODData(odFile);

    auto model = std::make_shared<CarletonUniversityCampusCoupled>("Carleton University Campus", carDepartureTimes, odData);
    auto rootCoordinator = cadmium::RootCoordinator(model);

    rootCoordinator.setLogger<STDOUTLogger>(",");
    //rootCoordinator.setLogger<CSVLogger>(outputFile, ",");
    rootCoordinator.start();
    rootCoordinator.simulate(maxSimulationTime);
    rootCoordinator.stop();	
}
