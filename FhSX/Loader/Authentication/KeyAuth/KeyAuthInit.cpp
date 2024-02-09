#include "KeyAuthInit.h"
#include "skStr.h"

using namespace KeyAuth;

keyAuthInit::keyAuthInit() :
    name(skCrypt("Hanbot").decrypt()),
    ownerid(skCrypt("0uZmlfg3fy").decrypt()),
    secret(skCrypt("988413c497760926b11a7243a5459e4b7bc435ed28c1b717196ae84ec4a49f0d").decrypt()),
    version(skCrypt("1.3").decrypt()),
    url(skCrypt("https://keyauth.win/api/1.2/").decrypt()),
    KeyAuthApp(name, ownerid, secret, version, url) {}

void keyAuthInit::init() {
    KeyAuthApp.init();
}

KeyAuth::api& keyAuthInit::getKeyAuthApp() {
    return KeyAuthApp;
}
