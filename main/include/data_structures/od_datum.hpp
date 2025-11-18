#ifndef OD_DATUM_HPP
#define OD_DATUM_HPP

#include <string>

struct ODDatum{
    int id;
    std::string origin;
    std::string dest;
    int flowRate;

    explicit ODDatum(int id = 0, std::string origin = "", std::string dest = "", int flowRate = 0): 
            id(id), origin(origin), dest(dest), flowRate(flowRate) {};
};

#endif // OD_DATUM_HPP
