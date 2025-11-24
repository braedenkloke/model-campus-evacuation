#ifndef OD_DATUM_HPP
#define OD_DATUM_HPP

#include <string>

struct ODDatum{
    std::string origin;
    std::string dest;
    int flowRate;

    explicit ODDatum(std::string origin = "", std::string dest = "", int flowRate = 0): 
            origin(origin), dest(dest), flowRate(flowRate) {};
};

#endif // OD_DATUM_HPP
