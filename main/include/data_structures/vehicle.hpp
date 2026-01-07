#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <iostream>

using namespace cadmium;

struct Vehicle {
    int id;             // Unique identifer for this Vehicle.
    int routeIndex;     // Index for the route this vehicle is taking. -1 when no route is assigned.
    std::string dest;

    explicit Vehicle(int routeIndex = -1, std::string dest = "") : routeIndex(routeIndex) {
        static int idCounter = 1;
        id = idCounter++;
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const Vehicle& v) {
    out << "Vehicle{id=" << v.id << " routeIndex=" << v.routeIndex << "}";
    return out;
}
#endif

#endif // VEHICLE_HPP
