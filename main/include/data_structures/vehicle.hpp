#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <ostream>

using namespace cadmium;

struct Vehicle {
    int id;
    int routeId; // ID of the route the vehicle is taking

    explicit Vehicle() {
        static int next_id = 0;
        id = next_id++;
        routeId = -1; // Default to -1 (no route assigned)
    }
};

inline std::ostream& operator<<(std::ostream& os, const Vehicle& v) {
    os << "Vehicle{id=" << v.id << " routeId=" << v.routeId << "}";
    return os;
}

#endif // VEHICLE_HPP
