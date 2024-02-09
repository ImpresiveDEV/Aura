#include "LeagueFunctions.h"
#include "xorstr.h"

LPVOID GetAsyncKeyStateSingle(int vKey) {
    return (LPVOID)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("GetAsyncKeyState").crypt_get());
}

LPVOID GetAsyncKeyState() {
    return (LPVOID)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("GetAsyncKeyState").crypt_get());
}

LPVOID IssueOrder(int unitID, int orderID) {
    return (LPVOID)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("IssueOrder").crypt_get());
}

LPVOID IssueOrderWithTarget(int unitID, int orderID, int targetIndex) {
    return (LPVOID)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("IssueOrder").crypt_get());
}

LPVOID SendSpellCastPacket(int unitID, int spellID, int targetIndex) {
    return (LPVOID)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("SendSpellCastPacket").crypt_get());
}

LPVOID UpdateChargedSpell(int unitID, int spellID, int targetIndex) {
    return (LPVOID)GetProcAddress(GetModuleHandle(xorstr(L"stub.dll").crypt_get()), xorstr("UpdateChargedSpell").crypt_get());
}
