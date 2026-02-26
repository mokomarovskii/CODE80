#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <string>
#include <vector>

#include "database.h"

#if defined(TIMELINE_IMGUI_GUI)
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace {
struct FormState {
    char theme[128] = "";
    char title[160] = "";
    int month = 1;
    int year = 1945;
    char place[128] = "";
    char leader[128] = "";
    char participants[256] = "";
    char result[256] = "";

    void clear() {
        theme[0] = '\0';
        title[0] = '\0';
        month = 1;
        year = 1945;
        place[0] = '\0';
        leader[0] = '\0';
        participants[0] = '\0';
        result[0] = '\0';
    }
};

struct FilterState {
    char theme[128] = "";
    char place[128] = "";
    int year = 0;
};

bool containsSubstring(const std::string& value, const char* filter) {
    return filter[0] == '\0' || value.find(filter) != std::string::npos;
}

bool passesFilter(const Event& event, const FilterState& filter) {
    return containsSubstring(event.theme, filter.theme) && containsSubstring(event.place, filter.place) &&
           (filter.year <= 0 || event.date.year == filter.year);
}

void applyModernStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.FrameRounding = 8.0f;
    style.GrabRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.TabRounding = 8.0f;
    style.FramePadding = ImVec2(10.0f, 6.0f);
    style.ItemSpacing = ImVec2(10.0f, 8.0f);

    ImVec4* c = style.Colors;
    c[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.12f, 1.0f);
    c[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.12f, 0.16f, 1.0f);
    c[ImGuiCol_Header] = ImVec4(0.24f, 0.36f, 0.62f, 0.50f);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.33f, 0.49f, 0.82f, 0.80f);
    c[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.58f, 0.95f, 0.95f);
    c[ImGuiCol_Button] = ImVec4(0.22f, 0.33f, 0.56f, 0.75f);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.33f, 0.49f, 0.82f, 0.90f);
    c[ImGuiCol_ButtonActive] = ImVec4(0.36f, 0.58f, 0.95f, 1.00f);
    c[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.17f, 0.24f, 0.90f);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.22f, 0.30f, 1.00f);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.22f, 0.25f, 0.36f, 1.00f);
    c[ImGuiCol_Border] = ImVec4(0.35f, 0.40f, 0.54f, 0.40f);
}

bool validateForm(const FormState& form, std::string& message) {
    if (form.theme[0] == '\0' || form.title[0] == '\0' || form.place[0] == '\0' || form.leader[0] == '\0' ||
        form.participants[0] == '\0' || form.result[0] == '\0') {
        message = "Fill all fields before saving.";
        return false;
    }
    if (form.month < 1 || form.month > 12) {
        message = "Month must be between 1 and 12.";
        return false;
    }
    if (form.year < 1 || form.year > 3000) {
        message = "Year must be between 1 and 3000.";
        return false;
    }
    return true;
}
}  // namespace

int main() {
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 760, "CODE80", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    applyModernStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    Database db;
    if (!db.open("events.db") || !db.createTable()) {
        return 1;
    }

    FormState form;
    FilterState filter;
    int selectedId = -1;
    std::string status = "Ready";

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        std::vector<Event> allEvents = db.loadAllEvents();
        std::vector<Event> visibleEvents;
        for (const Event& event : allEvents) {
            if (passesFilter(event, filter)) {
                visibleEvents.push_back(event);
            }
        }

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGui::Begin("Timeline UI", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoTitleBar);

        ImGui::TextColored(ImVec4(0.40f, 0.70f, 1.00f, 1.0f), "CODE80");
        ImGui::Separator();

        ImGui::BeginChild("left", ImVec2(io.DisplaySize.x * 0.60f, -50.0f), true);
        ImGui::Text("Event board");

        if (ImGui::BeginTable("event_table", 5,
                              ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
                                  ImGuiTableFlags_ScrollY,
                              ImVec2(0.0f, 0.0f))) {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableSetupColumn("Title");
            ImGui::TableSetupColumn("Theme");
            ImGui::TableSetupColumn("Place");
            ImGui::TableHeadersRow();

            for (const Event& event : visibleEvents) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                char idBuffer[32];
                std::snprintf(idBuffer, sizeof(idBuffer), "%d", event.id);
                if (ImGui::Selectable(idBuffer, selectedId == event.id,
                                      ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
                    selectedId = event.id;
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%02d.%d", event.date.month, event.date.year);
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted(event.title.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::TextUnformatted(event.theme.c_str());
                ImGui::TableSetColumnIndex(4);
                ImGui::TextUnformatted(event.place.c_str());
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("right", ImVec2(0.0f, -50.0f), true);

        if (ImGui::BeginTabBar("actions")) {
            if (ImGui::BeginTabItem("Add event")) {
                ImGui::InputText("Theme", form.theme, sizeof(form.theme));
                ImGui::InputText("Title", form.title, sizeof(form.title));
                ImGui::InputInt("Month", &form.month);
                ImGui::InputInt("Year", &form.year);
                ImGui::InputText("Place", form.place, sizeof(form.place));
                ImGui::InputText("Leader", form.leader, sizeof(form.leader));
                ImGui::InputText("Participants", form.participants, sizeof(form.participants));
                ImGui::InputTextMultiline("Result", form.result, sizeof(form.result), ImVec2(-FLT_MIN, 90.0f));

                if (ImGui::Button("Save", ImVec2(120.0f, 0.0f))) {
                    Event event{};
                    event.theme = form.theme;
                    event.title = form.title;
                    event.date.month = form.month;
                    event.date.year = form.year;
                    event.place = form.place;
                    event.leader = form.leader;
                    event.participants = form.participants;
                    event.result = form.result;

                    if (!validateForm(form, status)) {
                        // status already set
                    } else if (db.insertEvent(event)) {
                        status = "Event saved.";
                        form.clear();
                    } else {
                        status = "Failed to save event.";
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear", ImVec2(120.0f, 0.0f))) {
                    form.clear();
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Filter")) {
                ImGui::InputText("Theme filter", filter.theme, sizeof(filter.theme));
                ImGui::InputText("Place filter", filter.place, sizeof(filter.place));
                ImGui::InputInt("Year filter", &filter.year);
                if (ImGui::Button("Reset filters")) {
                    filter = FilterState{};
                }
                ImGui::Text("Visible rows: %d", static_cast<int>(visibleEvents.size()));
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Actions")) {
                if (ImGui::Button("Delete selected", ImVec2(200.0f, 0.0f))) {
                    if (selectedId <= 0) {
                        status = "Select a row first.";
                    } else if (db.deleteEvent(selectedId)) {
                        status = "Selected event deleted.";
                        selectedId = -1;
                    } else {
                        status = "Delete failed.";
                    }
                }
                ImGui::TextWrapped("Tip: use keyboard arrows + Enter in the table to select rows quickly.");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Text("Status: %s", status.c_str());
        ImGui::End();

        ImGui::Render();
        int displayW = 0;
        int displayH = 0;
        glfwGetFramebufferSize(window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        glClearColor(0.07f, 0.08f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
#else
#include <iostream>

int main() {
    std::cout << "GUI backend is disabled for this build.\n";
    std::cout << "Enable TIMELINE_IMGUI_GUI and install dependencies via vcpkg.\n";
    return 0;
}
#endif
