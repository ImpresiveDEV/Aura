#include "Menu.h"
#include "Globals.h"
#include <wtypes.h>
#include <iostream>
#include "Config.h"
#include "skStr.h"
#include "KeyAuthInit.h"
#include "TimeUtils.h"
#include "AuraModule.h"

extern keyAuthInit KeyAuthInitializer;

void Menu::load_styles()
{
    ImVec4* colors = ImGui::GetStyle().Colors;

    // Kolory okna
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.30f);

    // Kolory ramki
    colors[ImGuiCol_FrameBg] = ImColor(11, 11, 11, 255);
    colors[ImGuiCol_FrameBgHovered] = ImColor(11, 11, 11, 255);

    // Kolory przycisku
    colors[ImGuiCol_Button] = ImColor(90, 0, 176, globals.button_opacity);
    colors[ImGuiCol_ButtonActive] = ImColor(90, 0, 176, globals.button_opacity);
    colors[ImGuiCol_ButtonHovered] = ImColor(90, 0, 176, globals.button_opacity);

    // Kolor tekstu wyłączonego
    colors[ImGuiCol_TextDisabled] = ImVec4(0.37f, 0.37f, 0.37f, 1.30f);

    ImGuiStyle* style = &ImGui::GetStyle();

    // Styl okna
    style->WindowPadding = ImVec2(4, 4);
    style->WindowBorderSize = 0.f;

    // Styl ramki
    style->FramePadding = ImVec2(8, 6);
    style->FrameRounding = 3.f;
    style->FrameBorderSize = 1.f;
}

class initWindow {
public:
    const char* window_title = " ";
    ImVec2 window_size{ 740, 460 };

    DWORD window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
} iw;

void Menu::render() {

    if (globals.active)
    {
        if (!doOnce)
        {
            load_styles();
            doOnce = true;
        }

        ImGui::SetNextWindowSize(iw.window_size);

        ImGui::Begin(iw.window_title, &globals.active, iw.window_flags);
        {
            ImGui::SetCursorPos(ImVec2(726, 5));
            ImGui::TextDisabled(xorstr("X").crypt_get());
            if (ImGui::IsItemClicked())
            {
                globals.active = false;
            }

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.054f, 0.054f, 0.054f, 255));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.082f, 0.078f, 0.078f, 255));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.f);
            {

                if (show_login) {

                    ImGui::SetCursorPos(ImVec2(222, 83));
                    ImGui::BeginChild(("##Authentication"), ImVec2(300, 276), true);
                    {
                        ImGui::SetCursorPos(ImVec2(118, 20));
                        ImGui::TextDisabled(xorstr("HANBOT").crypt_get());

                        ImGui::PushItemWidth(260.f);
                        {
                            ImGui::SetCursorPos(ImVec2(22, 50));
                            ImGui::TextDisabled(xorstr("License").crypt_get());

                            ImGui::SetCursorPos(ImVec2(20, 65));
                            ImGui::InputText((xorstr("##License").crypt_get()), globals.license, IM_ARRAYSIZE(globals.license));
                        }
                        ImGui::PopItemWidth();

                        ImGui::SetCursorPos(ImVec2(22, 230));
                        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);
                        if (ImGui::Button((xorstr("Auth me").crypt_get()), ImVec2(260.f, 30.f)))
                        {
                            auto& KeyAuthApp = KeyAuthInitializer.getKeyAuthApp(); 

                            KeyAuthApp.init();

                            KeyAuthApp.license(globals.license); 

                            if (KeyAuthApp.data.success) {
                                std::cout << xorstr("Welcome to the Hextech realm, ").crypt_get() << KeyAuthApp.data.username << xorstr("! Your arcane sigil has been recognized. The gates of Hanbot are now open to you, summoner.").crypt_get() << std::endl;


                                for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                                    auto& sub = KeyAuthApp.data.subscriptions[i]; 
                                    ExpireLabel = TimeUtils::tm_to_readable_time(TimeUtils::timet_to_tm(TimeUtils::string_to_timet(sub.expiry)));
                                }

                                show_login = false;
                                show_main_menu = true;
                            }
                            else {
                                std::cout << xorstr("Access denied: ").crypt_get() << KeyAuthApp.data.message << xorstr(". The relic rejects your sigil, a mismatch in the codex of Hanbot. This portal will close for 15s. Seek the correct relic to pass the threshold.").crypt_get() << std::endl;

                                Sleep(15000);
                                exit(0);
                            }

                        }
                        ImGui::PopStyleVar();

                    }
                    ImGui::EndChild();

                }


                if (show_main_menu) {

                    ImGui::SetCursorPos(ImVec2(118, 20));
                    ImGui::TextDisabled(xorstr("HANBOT - Elevate Your Gaming Experience").crypt_get());

                    ImGui::SetCursorPos(ImVec2(90, 65));
                    ImGui::BeginChild((xorstr("##Core").crypt_get()), ImVec2(300, 350), true);
                    {
                        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.054f, 0.054f, 0.054f, 255.0f));
                        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.082f, 0.078f, 0.078f, 255.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.f);

                        ImGui::TextDisabled(xorstr("Core Function").crypt_get());

                        ImGui::Text(xorstr("Select region League of Legends Server ").crypt_get());
                        ImGui::Text(" ");

                        if (ImGui::RadioButton((xorstr("Riot Game Server").crypt_get()), SelectServerGame == 1))
                        {
                            SelectServerGame = 1;
                            ServerChina = false;
                            ServerRiot = true;
                        }

                        if (ImGui::RadioButton((xorstr("China Server").crypt_get()), SelectServerGame == 2))
                        {
                            ServerChina = true;
                            ServerRiot = false;
                            SelectServerGame = 2;
                        }
                        if (ImGui::RadioButton((xorstr("Japan Server").crypt_get()), SelectServerGame == 3))
                        {
                            ServerChina = false;
                            ServerRiot = true;
                            SelectServerGame = 3;
                        }
                        if (ImGui::RadioButton((xorstr("Korea Server").crypt_get()), SelectServerGame == 4))
                        {
                            ServerChina = false;
                            ServerRiot = true;
                            SelectServerGame = 4;
                        }

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();

                        ImGui::TextDisabled(xorstr("Settings").crypt_get());

                        ImGui::Spacing();

                        ImGui::Text(xorstr("Module").crypt_get());
                        if (ImGui::Combo(xorstr("##Module").crypt_get(), &currentItem, items, itemsCount)) {

                            if (currentItem == 0) {
                                State = xorstr("Please use Aura instead - hanshield is in development stage").crypt_get();
                                AutoInject = false; 
                            }

                            else if (currentItem == 1) {
                                State = xorstr("Aura Loaded - Waiting for League of Legends").crypt_get();
                                AutoInject = true;
                            }
                            else if (currentItem == 2) {
                                State = xorstr("Select module to load before run League of Legends").crypt_get();
                                AutoInject = false; 
                            }
                        }

                        ImGui::Checkbox(xorstr("Ready-Inject Feature").crypt_get(), &AutoInject);
                        if (AutoInject && AdditionalCheck) {
                            if (!ServerRiot && !ServerChina) {
                                State = xorstr("Please Select Server").crypt_get();
                                AutoInject = false;
                            }
                            else if (currentItem == 1) { 

                                RunInjectionProcess();

                            }
                        }
                        if (!AutoInject) {
                            AdditionalCheck = true;
                        }

                    }
                    ImGui::EndChild();

                    ImGui::SetCursorPos(ImVec2(400, 65));
                    ImGui::BeginChild((xorstr("##Main").crypt_get()), ImVec2(300, 350), true);
                    {
                        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.054f, 0.054f, 0.054f, 255.0f));
                        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.082f, 0.078f, 0.078f, 255.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.f);

                        ImGui::TextDisabled(xorstr("License").crypt_get());

                        ImGui::Text(xorstr(" Your license will expire on: ").crypt_get());
                        ImGui::SameLine(0, 1);
                        ImGui::Text(ExpireLabel.data());

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::TextDisabled(xorstr("HANBOT State").crypt_get());
                        ImGui::Text(State.data());
                    }
                    ImGui::EndChild();

                }
            }
        }
        ImGui::End();
    }
    else
    {
        exit(0);
    }

}

