#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <iostream>

using namespace cadmium;

struct Vehicle {
    std::string src;    // Name of the diffusion element generator this vehicle was created from.
    int id;             // Unique identifer for this Vehicle.
    std::string dest;
    double t;           // Time left travelling on road.

    explicit Vehicle(std::string src = "") : src(src), dest(""), t(0) {
        static int idCounter = 1;
        id = idCounter++;
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const Vehicle& v) {
    out << "Vehicle{src=" << v.src << ",id=" << v.id << ",dest=" << v.dest << "}";
    return out;
}
#endif

#endif // VEHICLE_HPP
