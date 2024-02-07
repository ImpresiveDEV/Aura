#include "TimeUtils.h"
#include "skStr.h"
#include <Windows.h> 

namespace TimeUtils {
    std::string tm_to_readable_time(std::tm ctx) {

        char buffer[80];
        strftime(buffer, sizeof(buffer), skCrypt("%a %m/%d/%y %H:%M").decrypt(), &ctx);
        return std::string(buffer);
    }

    std::time_t string_to_timet(std::string timestamp) {

        auto cv = strtol(timestamp.c_str(), NULL, 10);
        return (time_t)cv;

    }

    std::tm timet_to_tm(std::time_t timestamp) {

        std::tm context;
        localtime_s(&context, &timestamp);
        return context;
    }
}