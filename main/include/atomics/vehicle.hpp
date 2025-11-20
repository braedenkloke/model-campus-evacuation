#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <ostream>

using namespace cadmium;

struct Vehicle {
    int id;               // Unique vehicle identifier.
    std::string origin;   // Origin location.
    std::string next;     // Next location.
    double timestamp;     // Time vehicle entered this road segment.

    // Default constructor
    Vehicle(): id(-1), origin(""), next("") {}

    // Full constructor
    Vehicle(int id, const std::string& origin, const std::string& next, double timestamp)
        : id(id), origin(origin), next(next), timestamp(timestamp) {}
};

inline std::ostream& operator<<(std::ostream& os, const Vehicle& v) {
    os << "Vehicle{id=" << v.id
       << ", origin=" << v.origin
       << ", next=" << v.next
       << ", timestamp=" << v.timestamp
       << "}";
    return os;
}

#endif // VEHICLE_HPP