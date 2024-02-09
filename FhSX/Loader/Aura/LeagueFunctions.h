#pragma once

#include <Windows.h>

LPVOID GetAsyncKeyStateSingle(int vKey);
LPVOID GetAsyncKeyState();
LPVOID IssueOrder(int unitID, int orderID);
LPVOID IssueOrderWithTarget(int unitID, int orderID, int targetIndex);
LPVOID SendSpellCastPacket(int unitID, int spellID, int targetIndex);
LPVOID UpdateChargedSpell(int unitID, int spellID, int targetIndex);
