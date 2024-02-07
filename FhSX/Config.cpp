#include "config.h"

bool show_main_menu = false;
bool ServerChina = false;
bool ServerRiot = true;
bool doOnce = false;
bool logfree = false;
bool CheckLic = false;
bool AdditionalCheck = true;
bool show_login = true;

int currentItem = 0; // Tutaj właściwa inicjalizacja
const char* items[] = { "Hanshield", "Aura" }; // Definicja i inicjalizacja tablicy
const int itemsCount = sizeof(items) / sizeof(items[0]); // Dodaj tę linię

int SelectServerGame = 1;

bool AutoInject = true;

std::string State = "Waiting"; // Niepotrzebne nawiasy
std::string ExpireLabel;
