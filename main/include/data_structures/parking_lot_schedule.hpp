#ifndef PARKING_LOT_SCHEDULE_HPP
#define PARKING_LOT_SCHEDULE_HPP

#include <string>

struct ParkingLotSchedule{
    std::string id;
    double initEventInSec;
    double periodInSec;
    int totalEvents;

    explicit ParkingLotSchedule(std::string id, double initEventInSec, double periodInSec, int totalEvents): 
            id(id), initEventInSec(initEventInSec), periodInSec(periodInSec), totalEvents(totalEvents) {};
};

#endif // PARKING_LOT_SCHEDULE_HPP
