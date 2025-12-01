#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <iostream>

using namespace cadmium;

struct Vehicle {
    int id;         // Unique identifer for this Vehicle.
    int routeIndex;    // Index for the route this vehicle is taking. -1 when no route is assigned.
    Vehicle() : id(-1), routeIndex(-1) {}
    explicit Vehicle(int rIndex) : routeIndex(rIndex) {
        static int idCounter = 0;
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
