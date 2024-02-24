#include "Menu.h"
#include "Globals.h"
#include <wtypes.h>
#include <iostream>
#include "Config.h"
#include "skStr.h"
#include "KeyAuthInit.h"
#include "TimeUtils.h"
#include "AuraModule.h"
#include <imgui_internal.h>
#include "CheckAuth.h"
using namespace std;

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
            ImGui::TextDisabled("X");
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
                        ImGui::TextDisabled("HANBOT");

                        ImGui::PushItemWidth(260.f);
                        {
                            ImGui::SetCursorPos(ImVec2(22, 50));
                            ImGui::TextDisabled("License");

                            ImGui::SetCursorPos(ImVec2(20, 65));
                            ImGui::InputText(("##License"), globals.license, IM_ARRAYSIZE(globals.license));
                        }
                        ImGui::PopItemWidth();

                        std::string licenseFilePath = skCrypt("C:\\Hanbot.lic").decrypt();
                        std::ifstream licenseFile(licenseFilePath);
                        if (licenseFile.is_open()) {
                            std::string licenseKey;
                            std::getline(licenseFile, licenseKey); 
                            licenseFile.close();
                            strncpy_s(globals.license, sizeof(globals.license), licenseKey.c_str(), _TRUNCATE);
                        }
                        else {
                            std::cout << "Could not open license file." << std::endl;
                        }

                        ImGui::SetCursorPos(ImVec2(22, 230));
                        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);
                        if (ImGui::Button(("Auth me"), ImVec2(260.f, 30.f)))
                        {
                            auto& KeyAuthApp = KeyAuthInitializer.getKeyAuthApp(); 

                            KeyAuthApp.init();

                            KeyAuthApp.license(globals.license); 

                            if (KeyAuthApp.data.success) {


                                for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
                                    auto& sub = KeyAuthApp.data.subscriptions[i]; 
                                    ExpireLabel = TimeUtils::tm_to_readable_time(TimeUtils::timet_to_tm(TimeUtils::string_to_timet(sub.expiry)));
                                }

                                ofstream LicenseFile(skCrypt("C:\\Hanbot.lic").decrypt());
                                if (LicenseFile.is_open()) {
                                    LicenseFile << globals.license;
                                    LicenseFile.close();
                                }
                                else {
                                    std::cerr << skCrypt("Code 100").decrypt() << std::endl;
                                }

                                show_login = false;
                                show_main_menu = true;
                            }
                            else {
                                std::cout << "This license is not authorized." << std::endl;

                                Sleep(3000);
                                exit(0);
                            }
                        }
                        ImGui::PopStyleVar();

                    }
                    ImGui::EndChild();

                }


                if (show_main_menu) {

                    ImGui::SetCursorPos(ImVec2(118, 20));
                    ImGui::TextDisabled("HANBOT - Elevate Your Gaming Experience");

                    ImGui::SetCursorPos(ImVec2(90, 65));
                    ImGui::BeginChild(("##Core"), ImVec2(300, 350), true);
                    {
                        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.054f, 0.054f, 0.054f, 255.0f));
                        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.082f, 0.078f, 0.078f, 255.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.f);

                        ImGui::TextDisabled("Core Function");

                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                        ImGui::RadioButton("Riot Game Server", &SelectServerGame, 1); 
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();

                        ImGui::TextDisabled("Settings");

                        ImGui::Spacing();

                        ImGui::Text("Module");
                        if (ImGui::Combo("##Module", &currentItem, items, itemsCount)) {

                            switch (currentItem) {

                            case 0: 
                                State = "Please use Aura instead - Hanshield is in Maintance";
                                break;

                            case 1:

                                State = "Aura Loaded - Waiting for League of Legends";
                                void VerifyLicense();
                                StartInjectionProcess();
                                break;

                            case 2: 
                                State = "Select module to load before run League of Legends";
                                break;

                            }
                        }
                    }

                    ImGui::EndChild();

                    ImGui::SetCursorPos(ImVec2(400, 65));
                    ImGui::BeginChild(("##Main"), ImVec2(300, 350), true);
                    {
                        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.054f, 0.054f, 0.054f, 255.0f));
                        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.082f, 0.078f, 0.078f, 255.0f));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.f);

                        ImGui::TextDisabled("License");

                        ImGui::Text(" Your license will expire on: ");
                        ImGui::SameLine(0, 1);
                        ImGui::Text(ExpireLabel.data());

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::TextDisabled(" Loader state: ");
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

