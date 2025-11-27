#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#include <string>
#include <iostream>

using namespace cadmium;

struct Vehicle {
public:
    int id;
    int selectedRouteIndex; // Store selected route in vehicle
    
    explicit Vehicle() : id(generate_vehicle()), selectedRouteIndex(-1){}

private:
    static int generate_vehicle(){
        static int idCounter = 1;
        return idCounter++;
    }

};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const Vehicle& v) {
    out << "Vehicle{id=" << v.id << ", routeIndex=" << v.selectedRouteIndex << "}";
    return out;
}
#endif

#endif // VEHICLE_HPP
