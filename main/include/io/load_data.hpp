#ifndef LOAD_DATA_HPP
#define LOAD_DATA_HPP

#include "../data_structures/intersection_config.hpp"
#include "../data_structures/od_datum.hpp"
#include "../../lib/rapidcsv.h"
#include "../../lib/nlohmann/json.hpp"

using json = nlohmann::json;

std::vector<IntersectionConfig> loadIntersectionGeoJSONData(std::string filepath) {
    std::ifstream f(filepath);
    json data = json::parse(f);

    std::vector<IntersectionConfig> intersections;
    for (auto f: data["features"]) {
        intersections.push_back(IntersectionConfig(f["id"]));
    }
    
    return intersections;
}

std::vector<ODDatum> loadODData(std::string filepath) {
    rapidcsv::Document doc(filepath);
    std::vector<std::string> origins = doc.GetColumn<std::string>("ORIGIN");
    std::vector<std::string> destinations = doc.GetColumn<std::string>("DEST");
    std::vector<int> flowRates = doc.GetColumn<int>("FLOW");

    std::vector<ODDatum> odData;
    for (int i = 0; i < origins.size(); i++) {
        odData.push_back(ODDatum(origins[i], destinations[i], flowRates[i]));
    }
    return odData;
}

#endif // LOAD_DATA_HPP
