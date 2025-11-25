#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <iostream>

using namespace cadmium;

struct Vehicle {
    int id;

    explicit Vehicle() {
        static int idCounter = 1;
        id = idCounter++;
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const Vehicle& v) {
    out << "Vehicle{id=" << v.id;
    return out;
}
#endif

#endif // VEHICLE_HPP
