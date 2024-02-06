#pragma once

#include "auth.h" 
#include <string>

class keyAuthInit {
public:
    keyAuthInit();
    void init();
    KeyAuth::api& getKeyAuthApp(); 

private:
    std::string name;
    std::string ownerid;
    std::string secret;
    std::string version;
    std::string url;
    KeyAuth::api KeyAuthApp;
};
