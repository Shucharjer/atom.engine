#pragma once
#include <memory>
#include <string>
#include <imgui.h>
#include <auxiliary/singleton.hpp>
#include <containers.hpp>
#include <world.hpp>

namespace atom::engine::application {

class Panel {
public:
    Panel(
        const char* name,
        const bool display     = true,
        ImGuiWindowFlags flags = 0,
        const bool open        = true
    );
    Panel(
        const std::string& name,
        const bool display     = true,
        ImGuiWindowFlags flags = 0,
        const bool open        = true
    );
    Panel(
        std::string&& name,
        const bool display     = true,
        ImGuiWindowFlags flags = 0,
        const bool open        = true
    );

    Panel(const Panel&)            = default;
    Panel(Panel&&)                 = default;
    Panel& operator=(const Panel&) = default;
    Panel& operator=(Panel&&)      = default;

    virtual ~Panel() = default;

    void setName(const char* name);
    void setName(const std::string& name);
    void setName(std::string&& name);
    void setDisplay(const bool display);
    void setOpen(const bool oepn);
    void setFlags(const ImGuiWindowFlags flags);

    [[nodiscard]] std::string_view getName() const noexcept;
    [[nodiscard]] bool getDisplay() const noexcept;
    [[nodiscard]] bool* const getPDisplay() noexcept;
    [[nodiscard]] bool getOpen() const noexcept;
    ImGuiWindowFlags& getFlags() noexcept;
    [[nodiscard]] const ImGuiWindowFlags& getFlags() const noexcept;

    void display(
        float deltaTime, atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds
    );

protected:
    virtual void onBegin();
    virtual void afterBegin();

    virtual void layout(
        float deltaTime, atom::map<std::string, std::shared_ptr<::atom::ecs::world>>& worlds
    );

    virtual void onEnd();
    virtual void afterEnd();

private:
    bool m_Display;
    bool m_Open;
    ImGuiWindowFlags m_Flags;
    std::string m_Name;
};

class PanelFactory final : public utils::singleton<PanelFactory> {
    friend class utils::singleton<PanelFactory>;

public:
    PanelFactory(const PanelFactory&)            = delete;
    PanelFactory(PanelFactory&&)                 = delete;
    PanelFactory& operator=(const PanelFactory&) = delete;
    PanelFactory& operator=(PanelFactory&&)      = delete;
    ~PanelFactory()                              = default;

    template <typename PanelType, typename... Args>
    auto make(Args&&... args) -> PanelType* {
        auto panel     = std::make_unique<PanelType>(std::forward<Args>(args)...);
        PanelType* ptr = panel.get();
        m_Panels.emplace_back(std::move(panel));

        return ptr;
    }

private:
    PanelFactory() = default;
    std::vector<std::unique_ptr<Panel>> m_Panels;
};

} // namespace atom::engine::application
