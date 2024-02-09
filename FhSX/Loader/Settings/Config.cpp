#include "config.h"
#include "xorstr.h"

bool show_main_menu = false;
bool ServerChina = false;
bool ServerRiot = true;
bool doOnce = false;
bool logfree = false;
bool CheckLic = false;
bool AdditionalCheck = true;
bool show_login = true;

int currentItem = 2; 
const char* items[] = { xorstr("Hanshield").crypt_get(), xorstr("Aura").crypt_get(), xorstr("None").crypt_get() };
const int itemsCount = sizeof(items) / sizeof(items[0]);

int SelectServerGame = 1;

bool AutoInject = false;

std::string State = xorstr("Select module to load before run League of Legends").crypt_get();
std::string ExpireLabel;

const std::string filePath = xorstr(R"(C:\Riot Games\League of Legends\Game\League of Legends.exe)").crypt_get();
const std::string jsonUrl = xorstr("https://raw.githubusercontent.com/ImpresiveDEV/Hanbot/main/version.json").crypt_get();