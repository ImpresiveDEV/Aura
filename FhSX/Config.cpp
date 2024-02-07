#include "config.h"

bool show_main_menu = false;
bool ServerChina = false;
bool ServerRiot = true;
bool doOnce = false;
bool logfree = false;
bool CheckLic = false;
bool AdditionalCheck = true;
bool show_login = true;

int currentItem = 0; 
const char* items[] = { "Hanshield", "Aura" };
const int itemsCount = sizeof(items) / sizeof(items[0]);

int SelectServerGame = 1;

bool AutoInject = true;

std::string State = "Waiting";
std::string ExpireLabel;
