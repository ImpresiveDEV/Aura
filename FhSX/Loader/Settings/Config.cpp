#include "config.h"


bool show_main_menu = false;
bool ServerChina = false;
bool ServerRiot = true;
bool doOnce = false;
bool logfree = false;
bool CheckLic = false;
bool AdditionalCheck = true;
bool show_login = true;

int currentItem = 2; 
const char* items[] = { "Hanshield", "Aura", "None" };
const int itemsCount = sizeof(items) / sizeof(items[0]);

int SelectServerGame = 1;

bool AutoInject = false;

std::string State = "Select module to load before run League of Legends";
std::string ExpireLabel;

const std::string filePath = R"(C:\Riot Games\League of Legends\Game\League of Legends.exe)";
const std::string jsonUrl = "https://raw.githubusercontent.com/ImpresiveDEV/Hanbot/main/version.json";