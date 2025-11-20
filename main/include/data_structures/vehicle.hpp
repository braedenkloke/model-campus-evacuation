#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <ostream>

using namespace cadmium;

struct Vehicle {
    int id;               // Unique vehicle identifier.

    // Full constructor
    Vehicle(int id = -1)
        : id(id) {}
};

inline std::ostream& operator<<(std::ostream& os, const Vehicle& v) {
    os << "Vehicle{id=" << v.id;
    return os;
}

#endif // VEHICLE_HPP