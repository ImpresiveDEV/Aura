#include "LeagueFunctions.h"


LPVOID GetAsyncKeyStateSingle(int vKey) {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "GetAsyncKeyState");
}

LPVOID GetAsyncKeyState() {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "GetAsyncKeyState");
}

LPVOID IssueOrder(int unitID, int orderID) {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "IssueOrder");
}

LPVOID IssueOrderWithTarget(int unitID, int orderID, int targetIndex) {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "IssueOrder");
}

LPVOID SendSpellCastPacket(int unitID, int spellID, int targetIndex) {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "SendSpellCastPacket");
}

LPVOID UpdateChargedSpell(int unitID, int spellID, int targetIndex) {
    return (LPVOID)GetProcAddress(GetModuleHandle(L"stub.dll"), "UpdateChargedSpell");
}
