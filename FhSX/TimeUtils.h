#pragma once

#include <string>
#include <ctime>

namespace TimeUtils {
    std::string tm_to_readable_time(std::tm ctx);
    std::time_t string_to_timet(std::string timestamp);
    std::tm timet_to_tm(std::time_t timestamp);
}
