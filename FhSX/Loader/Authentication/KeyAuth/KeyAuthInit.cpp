#include "KeyAuthInit.h"
#include "skStr.h"

using namespace KeyAuth;

keyAuthInit::keyAuthInit() :
    name(skCrypt("Hanbot").decrypt()),
    ownerid(skCrypt("").decrypt()),
    secret(skCrypt("").decrypt()),
    version(skCrypt("").decrypt()),
    url(skCrypt("").decrypt()),
    KeyAuthApp(name, ownerid, secret, version, url) {}

void keyAuthInit::init() {
    KeyAuthApp.init();
}

KeyAuth::api& keyAuthInit::getKeyAuthApp() {
    return KeyAuthApp;
}
