#include "Menu.h"
#include "Globals.h"
#include <wtypes.h>
#include <iostream>
#include "Config.h"

bool show_main_menu = true;
bool ServerChina = false;
bool ServerRiot = true;
bool doOnce = false;
bool logfree = false;
bool CheckLic = false;
bool AdditionalCheck = true;
bool show_login = false;

static int SelectServerGame = 1;

static bool AutoInject = true;

std::string State = ("Waiting");

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

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.054, 0.054, 0.054, 255));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.082, 0.078, 0.078, 255));
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

                        ImGui::SetCursorPos(ImVec2(22, 230));
                        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.f);
                        if (ImGui::Button(("Auth me"), ImVec2(260.f, 30.f)))
                        {
                              //login success or fail
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
                        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.054, 0.054, 0.054, 255));
                        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.082, 0.078, 0.078, 255));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.f);

                        ImGui::TextDisabled("Core Function");

                        ImGui::Text("Select region League of Legends Server ");
                        ImGui::Text(" ");

                        if (ImGui::RadioButton(("Riot Game Server"), SelectServerGame == 1))
                        {
                            SelectServerGame = 1;
                            ServerChina = false;
                            ServerRiot = true;
                        }

                        if (ImGui::RadioButton(("China Server"), SelectServerGame == 2))
                        {
                            ServerChina = true;
                            ServerRiot = false;
                            SelectServerGame = 2;
                        }
                        if (ImGui::RadioButton(("Japan Server"), SelectServerGame == 3))
                        {
                            ServerChina = false;
                            ServerRiot = true;
                            SelectServerGame = 3;
                        }
                        if (ImGui::RadioButton(("Korea Server"), SelectServerGame == 4))
                        {
                            ServerChina = false;
                            ServerRiot = true;
                            SelectServerGame = 4;
                        }

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();

                        ImGui::TextDisabled("Settings");

                        ImGui::Spacing();

                        ImGui::Checkbox(("Ready-Inject Feature"), &AutoInject);
                        if ((AutoInject && AdditionalCheck)) {
                            if (!ServerRiot && !ServerChina)
                            {
                                State = ("Please Select Server");
                                AutoInject = false;
                            }
                            else
                            {
                                //inject success or fail // additional keyauth check + check in call function in injection process
                            }
                        }
                        if (!AutoInject)
                        {
                            AdditionalCheck = true;
                        }

                    }
                    ImGui::EndChild();

                    ImGui::SetCursorPos(ImVec2(400, 65));
                    ImGui::BeginChild(("##Main"), ImVec2(300, 350), true);
                    {
                        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.054, 0.054, 0.054, 255));
                        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.082, 0.078, 0.078, 255));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.f);

                        ImGui::TextDisabled("License");

                        ImGui::Text(" Your license will expire on: ");
                        ImGui::SameLine(0, 1);

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::TextDisabled("HANBOT State");
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