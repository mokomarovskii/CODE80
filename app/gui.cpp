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


