#ifndef EVENT_HPP 
#define EVENT_HPP

#include <iostream>
#include <constants.hpp>

struct Event {
    int orderID;
    int eventID;
    std::string resource;
    std::string activity;

    explicit Event(int orderID, std::string resource = "tmp", std::string activity = "tmp"): 
                   orderID(orderID), resource(resource), activity(activity) {
        static int eventIDCounter = 1;
        eventID = eventIDCounter++;
    };
};

#ifndef NO_LOGGING
// Formats the event log.
std::ostream& operator<<(std::ostream &out, const Event& event) {
    if (event.activity != placeOrderActivity) {
        out << "eventLog,";
        out << event.orderID << ",";
        out << event.eventID << ",";
        out << event.resource << ",";
        out << event.activity << ",";
    }
    return out;
}
#endif

#endif // EVENT_HPP
