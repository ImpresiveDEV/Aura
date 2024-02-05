#include "KeyAuthInit.h"
#include "skStr.h"

using namespace KeyAuth;

KeyAuthInit::KeyAuthInit() :
    name(skCrypt("Hanbot").decrypt()),
    ownerid(skCrypt("0uZmlfg3fy").decrypt()),
    secret(skCrypt("988413c497760926b11a7243a5459e4b7bc435ed28c1b717196ae84ec4a49f0d").decrypt()),
    version(skCrypt("1.3").decrypt()),
    url(skCrypt("https://keyauth.win/api/1.2/").decrypt()),
    KeyAuthApp(name, ownerid, secret, version, url)
{}

KeyAuthInit::~KeyAuthInit() {
    
}

void KeyAuthInit::init() {

    KeyAuthApp.init();

}


