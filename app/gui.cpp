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

void copyEventToForm(const Event& event, FormState& form) {
    std::snprintf(form.theme, sizeof(form.theme), "%s", event.theme.c_str());
    std::snprintf(form.title, sizeof(form.title), "%s", event.title.c_str());
    form.month = event.date.month;
    form.year = event.date.year;
    std::snprintf(form.place, sizeof(form.place), "%s", event.place.c_str());
    std::snprintf(form.leader, sizeof(form.leader), "%s", event.leader.c_str());
    std::snprintf(form.participants, sizeof(form.participants), "%s", event.participants.c_str());
    std::snprintf(form.result, sizeof(form.result), "%s", event.result.c_str());
}

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
void loadBulgarianWinsPreset(Database& db) {
    if (!db.loadAllEvents().empty()) {
        return;
    }

    struct PresetEvent {
        const char* theme;
        const char* title;
        int month;
        int year;
        const char* place;
        const char* leader;
        const char* participants;
        const char* result;
    };

    const PresetEvent preset[] = {
        {"Statehood", "Founding of Danubian Bulgaria", 7, 681, "Balkans", "Khan Asparuh", "Proto-Bulgarians and Slavs", "Bulgarian statehood internationally acknowledged by Byzantium."},
        {"Diplomacy", "Peace treaty with Byzantium", 10, 716, "Constantinople", "Khan Tervel", "First Bulgarian Empire and Byzantine Empire", "Long-term peace and tribute favorable to Bulgaria."},
        {"Religion", "Christianization of Bulgaria", 3, 864, "Pliska", "Knyaz Boris I", "Bulgarian state and church", "Adoption of Christianity unified and stabilized the country."},
        {"Culture", "Disciples of Cyril and Methodius received", 5, 886, "Pliska", "Knyaz Boris I", "Clement, Naum, Angelarius and Bulgarian clergy", "Foundation of major Slavic literary and educational centers."},
        {"Culture", "Preslav Literary School flourishes", 1, 893, "Preslav", "Tsar Simeon I", "Bulgarian scholars and clergy", "Old Bulgarian became a leading literary language in Orthodox Europe."},
        {"Statehood", "Simeon I proclaimed Tsar", 8, 913, "Constantinople", "Tsar Simeon I", "Bulgaria and Byzantium", "Bulgarian ruler elevated to imperial status."},
        {"Military", "Battle of Achelous", 8, 917, "Near Anchialos", "Tsar Simeon I", "Bulgaria vs Byzantine Empire", "Major Bulgarian victory securing regional dominance."},
        {"Statehood", "Samuil crowned Tsar", 6, 997, "Prespa", "Tsar Samuil", "Bulgarian nobility and clergy", "Continuity of Bulgarian state institutions affirmed."},
        {"Statehood", "Uprising of Asen and Peter", 10, 1185, "Tarnovo", "Theodore-Peter and Ivan Asen I", "Bulgarians and Vlachs", "Restoration of Bulgarian statehood (Second Bulgarian Empire)."},
        {"Military", "Battle of Adrianople", 4, 1205, "Adrianople", "Tsar Kaloyan", "Bulgaria and Cumans vs Latin Empire", "Crushing victory over the Latin Empire."},
        {"Military", "Battle of Klokotnitsa", 3, 1230, "Klokotnitsa", "Tsar Ivan Asen II", "Bulgaria vs Epirus", "Territorial and political peak of the Second Bulgarian Empire."},
        {"Defense", "Defense of Vidin Tsardom autonomy", 5, 1369, "Vidin", "Tsar Ivan Sratsimir", "Vidin Tsardom", "Temporary preservation of Bulgarian rule in the northwest."},
        {"National Revival", "Paisii writes Slavo-Bulgarian History", 1, 1762, "Hilandar", "Paisii Hilendarski", "Bulgarian clergy and intellectuals", "Catalyzed modern Bulgarian national consciousness."},
        {"Education", "First modern secular school in Gabrovo", 1, 1835, "Gabrovo", "Vasil Aprilov", "Bulgarian educators and students", "Start of modern Bulgarian secular education."},
        {"Church", "Bulgarian Exarchate established", 2, 1870, "Constantinople", "Bulgarian church leaders", "Ottoman authorities and Bulgarian Orthodox community", "Restoration of autonomous Bulgarian church governance."},
        {"Revolution", "April Uprising", 4, 1876, "Ottoman Bulgaria", "Georgi Benkovski and local committees", "Bulgarian revolutionaries", "Drew international attention to Bulgarian liberation cause."},
        {"Liberation", "Treaty of San Stefano", 3, 1878, "San Stefano", "Russian and Ottoman negotiators", "Great Powers, Ottoman Empire, Bulgarians", "Recognized a large autonomous Bulgarian entity."},
        {"Statehood", "Constitution of Tarnovo adopted", 4, 1879, "Tarnovo", "Constituent Assembly", "Bulgarian political elite", "Established liberal constitutional order."},
        {"Statehood", "Unification of Bulgaria", 9, 1885, "Plovdiv", "Alexander I of Battenberg", "Principality of Bulgaria and Eastern Rumelia", "National unification achieved despite external pressure."},
        {"Military", "Battle of Slivnitsa", 11, 1885, "Slivnitsa", "Prince Alexander I", "Bulgaria vs Kingdom of Serbia", "Successful defense of unification."},
        {"Diplomacy", "International recognition of Unification", 4, 1886, "Istanbul", "Bulgarian diplomacy", "Great Powers and Ottoman Empire", "Political settlement accepted de facto unification."},
        {"Independence", "Proclamation of Bulgarian Independence", 9, 1908, "Veliko Tarnovo", "Tsar Ferdinand I", "Bulgarian state and people", "Full sovereignty of Bulgaria proclaimed."},
        {"Military", "Capture of Edirne", 3, 1913, "Edirne", "Gen. Nikola Ivanov", "Bulgaria and allies vs Ottoman Empire", "Major strategic success in the First Balkan War."},
        {"Military", "Battle of Tutrakan", 9, 1916, "Tutrakan", "Gen. Panteley Kiselov", "Bulgaria and Germany vs Romania", "Significant Bulgarian success on the Romanian front."},
        {"Military", "Battle of Doiran", 9, 1918, "Doiran", "Gen. Vladimir Vazov", "Bulgaria vs British and Greek forces", "Successful defense with high international recognition."},
        {"Diplomacy", "Treaty of Craiova", 9, 1940, "Craiova", "Bulgarian and Romanian delegations", "Bulgaria and Romania", "Southern Dobrudja returned to Bulgaria peacefully."},
        {"Rescue", "Rescue of Bulgarian Jews", 3, 1943, "Kingdom of Bulgaria", "Dimitar Peshev and Bulgarian society", "Bulgarian institutions, church and citizens", "Deportation of most Bulgarian Jews was prevented."},
        {"International", "Bulgaria joins the United Nations", 12, 1955, "New York", "Bulgarian diplomacy", "UN member states", "Integration into global post-war institutions."},
        {"International", "Bulgaria joins NATO", 3, 2004, "Washington", "Bulgarian government", "NATO member states", "Security and strategic integration with Euro-Atlantic allies."},
        {"International", "Bulgaria joins the European Union", 1, 2007, "Brussels", "Bulgarian government", "EU member states", "Economic and political integration in the EU."}
    };

    for (const PresetEvent& item : preset) {
        Event e;
        e.theme = item.theme;
        e.title = item.title;
        e.date.month = item.month;
        e.date.year = item.year;
        e.place = item.place;
        e.leader = item.leader;
        e.participants = item.participants;
        e.result = std::string(item.result) + " Bulgaria victor in positive outcome.";
        db.insertEvent(e);
    }
}

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
    loadBulgarianWinsPreset(db);
    FormState form;
    FormState editForm;
    FilterState filter;
    int selectedId = -1;
    int loadedForEditId = -1;
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

        if (ImGui::BeginTable("event_table", 8,
                              ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable |
                                  ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX,
                              ImVec2(0.0f, 0.0f))) {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, 90.0f);
            ImGui::TableSetupColumn("Title");
            ImGui::TableSetupColumn("Theme");
            ImGui::TableSetupColumn("Place");
            ImGui::TableSetupColumn("Leader");
            ImGui::TableSetupColumn("Participants");
            ImGui::TableSetupColumn("Result");
            

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
                ImGui::TableSetColumnIndex(5);
                ImGui::TextUnformatted(event.leader.c_str());
                ImGui::TableSetColumnIndex(6);
                ImGui::TextUnformatted(event.participants.c_str());
                ImGui::TableSetColumnIndex(7);
                ImGui::TextUnformatted(event.result.c_str());
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
            if (ImGui::BeginTabItem("Edit selected")) {
                if (selectedId <= 0) {
                    ImGui::TextWrapped("Select a row in the table first.");
                } else {
                    Event* selectedEvent = nullptr;
                    for (Event& event : allEvents) {
                        if (event.id == selectedId) {
                            selectedEvent = &event;
                            break;
                        }
                    }

                    if (!selectedEvent) {
                        ImGui::TextWrapped("Selected row is no longer available.");
                    } else {
                        if (loadedForEditId != selectedId) {
                            copyEventToForm(*selectedEvent, editForm);
                            loadedForEditId = selectedId;
                        }

                        ImGui::Text("Editing ID: %d", selectedId);
                        ImGui::InputText("Theme##edit", editForm.theme, sizeof(editForm.theme));
                        ImGui::InputText("Title##edit", editForm.title, sizeof(editForm.title));
                        ImGui::InputInt("Month##edit", &editForm.month);
                        ImGui::InputInt("Year##edit", &editForm.year);
                        ImGui::InputText("Place##edit", editForm.place, sizeof(editForm.place));
                        ImGui::InputText("Leader##edit", editForm.leader, sizeof(editForm.leader));
                        ImGui::InputText("Participants##edit", editForm.participants, sizeof(editForm.participants));
                        ImGui::InputTextMultiline("Result##edit", editForm.result, sizeof(editForm.result),
                                                  ImVec2(-FLT_MIN, 90.0f));

                        if (ImGui::Button("Update selected", ImVec2(200.0f, 0.0f))) {
                            Event updated{};
                            updated.id = selectedId;
                            updated.theme = editForm.theme;
                            updated.title = editForm.title;
                            updated.date.month = editForm.month;
                            updated.date.year = editForm.year;
                            updated.place = editForm.place;
                            updated.leader = editForm.leader;
                            updated.participants = editForm.participants;
                            updated.result = editForm.result;

                            if (!validateForm(editForm, status)) {
                                // status already set
                            } else if (db.updateEvent(updated)) {
                                status = "Selected event updated.";
                            } else {
                                status = "Update failed.";
                            }
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Reload from DB", ImVec2(150.0f, 0.0f))) {
                            copyEventToForm(*selectedEvent, editForm);
                            status = "Edit form reloaded.";
                        }
                    }
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
