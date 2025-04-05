#include "application/Panel.hpp"
#include <memory>
#include <imgui.h>

namespace atom::engine::application {
Panel::Panel(const char* name, const bool display, ImGuiWindowFlags flags, const bool open)
    : m_Name(name), m_Display(display), m_Flags(flags), m_Open(open) {}

Panel::Panel(const std::string& name, const bool display, ImGuiWindowFlags flags, const bool open)
    : m_Name(name), m_Display(display), m_Flags(flags), m_Open(open) {}

Panel::Panel(std::string&& name, const bool display, ImGuiWindowFlags flags, const bool open)
    : m_Name(std::move(name)), m_Display(display), m_Flags(flags), m_Open(open) {}

void Panel::setName(const char* name) { m_Name = name; }

void Panel::setName(const std::string& name) { m_Name = name; }

void Panel::setName(std::string&& name) { m_Name = std::move(name); }

void Panel::setDisplay(const bool display) { m_Display = display; }

void Panel::setOpen(const bool open) { m_Open = open; }

void Panel::setFlags(const ImGuiWindowFlags flags) { m_Flags = flags; }

void Panel::display(
    float deltaTime, atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds
) {
    onBegin();
    m_Display = ImGui::Begin(m_Name.c_str(), &m_Open, m_Flags);
    if (m_Display) {
        afterBegin();
        layout(deltaTime, worlds);
        onEnd();
        ImGui::End();
    }
    afterEnd();
}

std::string_view Panel::getName() const noexcept { return m_Name; }
bool Panel::getDisplay() const noexcept { return m_Display; }
bool* const Panel::getPDisplay() noexcept { return &m_Display; }
bool Panel::getOpen() const noexcept { return m_Open; }
ImGuiWindowFlags& Panel::getFlags() noexcept { return m_Flags; }
const ImGuiWindowFlags& Panel::getFlags() const noexcept { return m_Flags; }

void Panel::onBegin() {}
void Panel::afterBegin() {}
void Panel::layout(float deltaTime, atom::map<std::string, std::shared_ptr<ecs::world>>& worlds) {}
void Panel::onEnd() {}
void Panel::afterEnd() {}
} // namespace atom::engine::application
