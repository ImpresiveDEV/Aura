#pragma once

#include "auth.h" 
#include <string>

class KeyAuthInit {
public:
    KeyAuthInit();
    ~KeyAuthInit();

    void init();

private:
    std::string name;
    std::string ownerid;
    std::string secret;
    std::string version;
    std::string url;
    KeyAuth::api KeyAuthApp; 
};
