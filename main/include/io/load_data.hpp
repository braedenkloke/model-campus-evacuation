#ifndef LOAD_DATA_HPP
#define LOAD_DATA_HPP

#include "../data_structures/od_datum.hpp"
#include "../../lib/rapidcsv.h"

#include <unordered_map>
#include <cmath>

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

std::unordered_map<std::string, int> loadRoadLengths(std::string filepath) {
    rapidcsv::Document doc(filepath);
    std::vector<std::string> roads = doc.GetColumn<std::string>("ROAD");
    std::vector<double> lengths = doc.GetColumn<double>("LENGTH_M");

    std::unordered_map<std::string, int> out;
    for (int i = 0; i < (int)roads.size(); i++) {
        out[roads[i]] = (int)std::lround(lengths[i]);
    }
    return out;
}

std::unordered_map<std::string, int> loadRoadSpeeds(std::string filepath) {
    rapidcsv::Document doc(filepath);
    std::vector<std::string> roads = doc.GetColumn<std::string>("ROAD");
    std::vector<double> speeds = doc.GetColumn<double>("SPEED_KPH");

    std::unordered_map<std::string, int> out;
    for (int i = 0; i < (int)roads.size(); i++) {
        out[roads[i]] = (int)std::lround(speeds[i]); // store as int kph
    }
    return out;
}

#endif // LOAD_DATA_HPP
