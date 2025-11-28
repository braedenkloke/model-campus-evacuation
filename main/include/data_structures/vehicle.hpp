#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <iostream>

using namespace cadmium;

struct Vehicle {
    int id;         // Unique identifer for this Vehicle.
    int routeId;    // Identifier for the route this vehicle is taking. -1 when no route is assigned.

    explicit Vehicle(int routeId = -1): id(id), routeId(routeId) {
        static int idCounter = 0;
        id = idCounter++;
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const Vehicle& v) {
    out << "Vehicle{id=" << v.id << " routeId=" << v.routeId << "}";
    return out;
}
#endif

#endif // VEHICLE_HPP
