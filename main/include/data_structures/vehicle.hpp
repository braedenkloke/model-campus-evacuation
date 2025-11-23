#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <ostream>

using namespace cadmium;

struct Vehicle {
    int id;

    explicit Vehicle() {
        static int next_id = 1;
        id = next_id++;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Vehicle& v) {
    os << "Vehicle{id=" << v.id << "}";
    return os;
}

#endif // VEHICLE_HPP
