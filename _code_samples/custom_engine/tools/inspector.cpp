#if defined(BEE_INSPECTOR) && defined(BEE_PLATFORM_PC)
#include "tools/inspector.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>
#include <imgui/implot.h>
#include <glm/gtc/type_ptr.hpp>
#include "core/ecs.hpp"
#include "core/engine.hpp"
#include "core/device.hpp"
#include "core/fileio.hpp"
#include "core/input.hpp"
#include "core/transform.hpp"
#include "tools/log.hpp"
#include "tools/tools.hpp"
#include "tools/serialization_cereal.hpp"
#include <imgui/imgui_internal.h>
#include <imgui/IconsFontAwesome.h>

#include "rendering/render_xsr_components.hpp"
#include "tile_editor/tile_components.hpp"

#include "tools/serialization_cereal.hpp"
#include <fstream>
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/json.hpp>

#define SHOW_IMGUI TRUE

using namespace bee;
using namespace std;

void SetUnityEditorStyle();

bee::Inspector::Inspector()
{
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_Impl_Init();

    ImGuiIO& io = ImGui::GetIO();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    const std::string iniPath = Engine.FileIO().GetPath(FileIO::Directory::SaveFiles, "imgui.ini");
    const char* constStr = iniPath.c_str();
    char* str = new char[iniPath.size() + 1];
    strcpy_s(str, iniPath.size() + 1, constStr);
    io.IniFilename = str;

    const float UIScale = Engine.Device().GetMonitorUIScale();
    const float fontSize = 14.0f;
    const float iconSize = 12.0f;

    ImFontConfig config;
    config.OversampleH = 8;
    config.OversampleV = 8;
    io.Fonts->AddFontFromFileTTF(Engine.FileIO().GetPath(FileIO::Directory::SharedAssets, "/fonts/Roboto.ttf").c_str(),
                                 fontSize * UIScale,
                                 &config);
    static const ImWchar icons_ranges[] = {0xf000, 0xf3ff, 0};  // will not be copied by AddFont* so keep in scope.
    config.MergeMode = true;
    config.OversampleH = 8;
    config.OversampleV = 8;

    string fontpath = Engine.FileIO().GetPath(FileIO::Directory::SharedAssets, "/fonts/FontAwesome5FreeSolid900.otf");
    io.Fonts->AddFontFromFileTTF(fontpath.c_str(), iconSize * UIScale, &config, icons_ranges);

    SetUnityEditorStyle();
    m_openWindows["Configuration"] = false;

    Engine.Serializer().Deserialize(m_openWindows, "Inspector", FileIO::Directory::SaveFiles, "inspector.json");
}

bee::Inspector::~Inspector()
{
    ImGui_Impl_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    Engine.Serializer().Serialize(m_openWindows, "Inspector", FileIO::Directory::SaveFiles, "inspector.json");
}

void Inspector::Inspect(float)
{
#ifdef BEE_INSPECTOR
    if (!m_visible) return;

    ImGui_Impl_NewFrame();
    ImGuizmo::BeginFrame();

    Dockspace();

    Toolbar();

    Hierarchy();

    Viewport();

    Properties();

    SimulationControls();

    Panels();

    ImGui::End();
    ImGui::Render();
    ImGui_Impl_RenderDrawData(ImGui::GetDrawData());
#endif
}

void bee::Inspector::Dockspace()
{
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |=
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole,
    // so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    static bool truethat = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", &truethat, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen) ImGui::PopStyleVar(2);
}

void bee::Inspector::Toolbar()
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    const auto systems = Engine.ECS().GetSystems<System>();

    // DockSpace
    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                            {
                                8.0f,
                                7.0f,
                            });
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            {
                                8.0f,
                                7.0f,
                            });
        ImGui::BeginMainMenuBar();

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load Scene", nullptr, nullptr))
            {
                auto result = Engine.FileIO().OpenDialogue();
                Engine.Serializer().LoadScene(result.first, result.second);
            }

            if (ImGui::MenuItem("Save Scene", nullptr, nullptr))
            {
                auto result = Engine.FileIO().SaveDialogue();
                Engine.Serializer().SaveScene(result.first, result.second);
            }

            if (ImGui::MenuItem("Load Game Object", nullptr, nullptr))
            {
                auto result = Engine.FileIO().OpenDialogue();
                Engine.Serializer().LoadGameObject(result.first, result.second);
            }

            if (ImGui::MenuItem("Save Game Object", nullptr, nullptr))
            {
                auto result = Engine.FileIO().SaveDialogue();
                Engine.Serializer().SaveGameObject(SelectedEntity(), result.first, result.second);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Systems"))
        {
            for (auto* const s : systems)
            {
                auto title = s->Title;
                if (!title.empty())
                {
                    ImGui::MenuItem(title.c_str(), nullptr, &m_openWindows[s->Title]);
                }
                else
                {
                    title = typeid(*s).name();
                    title = StringReplace(title, "class ", "");
                    title = "[" + title + "]";
                    ImGui::MenuItem(title.c_str(), nullptr, nullptr);
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    }
}

void bee::Inspector::Hierarchy()
{
    ImGui::Begin("Scene");

    std::set<Entity> inspected;
    Engine.ECS()
        .Registry.view<Transform>(entt::exclude<HideTransform>)
        .each(
            [this, &inspected](auto entity, Transform& transform)
            {
                if (!transform.HasParent()) this->InspectHierarchy(entity, transform, inspected);
            });
    ImGui::End();
}

void bee::Inspector::Properties()
{
    const auto systems = Engine.ECS().GetSystems<System>();

    ImGui::Begin("Properties");
    if (Engine.ECS().Registry.valid(m_selectedEntity))
    {
        ImGui::LabelText("ID:", "%s", to_string(static_cast<int>(m_selectedEntity)).c_str());

        if (ImGui::Button("Delete")) Engine.ECS().DeleteEntity(m_selectedEntity);

        ImGui::PushID((void*)m_selectedEntity);

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (Engine.ECS().Registry.try_get<Transform>(m_selectedEntity))
            {
                Transform& t = Engine.ECS().Registry.get<Transform>(m_selectedEntity);
                glm::vec3 translation(t.GetTranslation());
                if (ImGui::DragFloat3("Position", value_ptr(translation), 0.01f))
                {
                    t.SetTranslation(translation);
                }
                glm::vec3 scale(t.GetScale());
                if (ImGui::DragFloat3("Scale", value_ptr(scale), 0.01f))
                {
                    t.SetScale(scale);
                }
                glm::vec3 rotationEuler(glm::eulerAngles(t.GetRotation()));
                if (ImGui::DragFloat3("Rotation (Euler)", value_ptr(rotationEuler), 0.01f))
                {
                    t.SetRotation(glm::quat(rotationEuler));
                }
            }
        }

        for (auto* s : systems)
        {
            s->Inspect(m_selectedEntity);
        }

        ImGui::PopID();
    }

    ImGui::End();
}

void bee::Inspector::Viewport()
{
    static bool truethat = true;
    // Actual game
    if (InspectorColorbuffer != 0)
    {
        ImVec4* colors = ImGui::GetStyle().Colors;
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
        ImGui::Begin(u8"\U0000f11b Game", &truethat, ImGuiWindowFlags_NoScrollbar);

        float width = ImGui::GetWindowWidth();
        float height = ImGui::GetWindowHeight();
        auto screenAspectRatio = (float)Engine.Device().GetHeight() / (float)Engine.Device().GetWidth();
        const auto lm = static_cast<unsigned long long>(InspectorColorbuffer);
        auto* const id = reinterpret_cast<ImTextureID>(lm);
        if (height / width < screenAspectRatio)
            width = height * 1.0f / screenAspectRatio;
        else
            height = width * screenAspectRatio;
        ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
        ImGui::Image(id, ImVec2(width, height), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

        auto btnColor = colors[ImGuiCol_Button];
        btnColor.w *= 0.4f;
        const float UIScale = 1.0f;  // Game.Device().GetMonitorUIScale();
        const auto s = ImGui::GetIO().FontGlobalScale * UIScale;
        const ImVec2 btnSize(24.0f * s, 24.0f * s);
        ImGui::SetCursorPos(ImVec2(6.0f, 6.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, btnColor);

        ImGui::PopStyleColor();

        // store the current viewport bounds
        ImVec2 pos(ImGui::GetWindowPos());
        m_gameViewportBounds = geometry2d::AABB({pos.x, pos.y}, {pos.x + width, pos.y + height});

        Gizmo();

        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();
    }
}

void bee::Inspector::Panels()
{
    const auto systems = Engine.ECS().GetSystems<System>();
    for (auto* const s : systems)
    {
        auto title = s->Title;
        if (!title.empty() && m_openWindows[title]) s->Inspect();
    }
}

void bee::Inspector::Gizmo()
{
    auto cameras = Engine.ECS().Registry.view<Camera>();
    if (cameras.empty()) return;
    auto cameraEntity = *cameras.begin();
    auto& cameraTransform = Engine.ECS().Registry.get<Transform>(cameraEntity);
    auto& camera = Engine.ECS().Registry.get<Camera>(*cameras.begin());

    auto view = glm::inverse(cameraTransform.World());
    auto projection = camera.Projection;

    if (m_selectedEntity == entt::null) return;

    auto* tile = Engine.ECS().Registry.try_get<Tile>(m_selectedEntity);

    if (tile) return;

    auto* transform = Engine.ECS().Registry.try_get<Transform>(m_selectedEntity);

    if (!transform) return;

    auto model = transform->World();

    ImGuizmo::Enable(true);

    ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);
    ImGuizmo::SetRect(m_gameViewportBounds.GetMin().x,
                      m_gameViewportBounds.GetMin().y,
                      m_gameViewportBounds.GetMax().x,
                      m_gameViewportBounds.GetMax().y);

    ImGuizmo::Manipulate(glm::value_ptr(view),
                         glm::value_ptr(projection),
                         m_gizmoOperation,
                         m_gizmoMode,
                         glm::value_ptr(model));

    if (ImGuizmo::IsUsing())
    {
        if (transform->HasParent())  // transform to local space
        {
            auto& parentTransform = Engine.ECS().Registry.get<Transform>(transform->GetParent());
            model = glm::inverse(parentTransform.World()) * model;
        }
        transform->SetFromMatrix(model);
    }
}

void bee::Inspector::SimulationControls()
{
    ImGui::Begin("Simulation Control");
    {
        if (Engine.IsPlaying())
        {
            ImGui::Text("The simulation is playing.");
            if (ImGui::Button("Pause"))
            {
                Engine.TogglePlay();
            }
        }
        else
        {
            ImGui::Text("The simulation is paused.");
            if (ImGui::Button("Play"))
            {
                Engine.TogglePlay();
            }
            ImGui::SameLine();
            if (ImGui::Button("Step"))
            {
                Engine.ToggleStep();
            }
        }
    }
    ImGui::End();
}

void bee::Inspector::InspectHierarchy(Entity entity, Transform& transform, std::set<Entity>& inspected)
{
    if (inspected.find(entity) != inspected.end()) return;
    inspected.insert(entity);

    string name = transform.Name.empty() ? "Entity-" + std::to_string(static_cast<std::uint32_t>(entity)) : transform.Name;

    static ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (transform.HasChildren())
    {
        const bool nodeOpen =
            ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<long long>(entity)), nodeFlags, "%s", name.c_str());
        if (ImGui::IsItemClicked()) m_selectedEntity = entity;

        if (nodeOpen)
        {
            for (auto child : transform)
            {
                if (Engine.ECS().Registry.valid(child))
                {
                    auto& childTransform = Engine.ECS().Registry.get<Transform>(child);
                    InspectHierarchy(child, childTransform, inspected);
                }
            }
            ImGui::TreePop();
        }
        else
        {
            for (auto child : transform)
            {
                AddToInspected(child, inspected);
            }
        }
    }
    else
    {
        ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<long long>(entity)),
                          nodeFlags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen,
                          "%s",
                          name.c_str());
        if (ImGui::IsItemClicked()) m_selectedEntity = entity;
    }
}

void bee::Inspector::AddToInspected(Entity entity, std::set<Entity>& inspected)
{
    inspected.insert(entity);
    if (auto* transform = Engine.ECS().Registry.try_get<Transform>(entity))
    {
        for (auto child : *transform)
        {
            AddToInspected(child, inspected);
        }
    }
}

void SetUnityEditorStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // ImGui::StyleColorsDark();  // Base for dark theme

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.192f, 0.192f, 0.192f, 1.00f);  // Main window
    colors[ImGuiCol_PopupBg] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

    // Tabs (e.g., Inspector, Scene, Project)
    colors[ImGuiCol_Tab] = ImVec4(0.176f, 0.176f, 0.176f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);  // Darker hover
    colors[ImGuiCol_TabActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);  // Lighter for inactive tabs
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);

    // Frames & Inputs (e.g., text fields, buttons)
    colors[ImGuiCol_FrameBg] = ImVec4(0.216f, 0.216f, 0.216f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);

    colors[ImGuiCol_Button] = ImVec4(0.247f, 0.247f, 0.247f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);

    colors[ImGuiCol_SliderGrab] = ImVec4(0.5f, 0.6f, 0.7f, 1.00f);        // Slider color
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.6f, 0.7f, 0.8f, 1.00f);  // Active slider color
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);  // Scrollbar color
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);

    // Disabled elements (grayed out)
    colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.1f, 0.1f, 0.1f, 0.00f);  // Subtle shadows

    // Misc elements like checkboxes, scrollbars
    colors[ImGuiCol_CheckMark] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);

    // Window styling
    colors[ImGuiCol_TitleBg] = ImVec4(0.157f, 0.157f, 0.157f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.137f, 0.137f, 0.137f, 1.00f);

    // Text colors (labels, values, etc.)
    colors[ImGuiCol_Text] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);         // White text
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);  // Selected text background

    // Selection and focus
    colors[ImGuiCol_Separator] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

    // Collapsing Headers (no blue highlight)
    colors[ImGuiCol_Header] = ImVec4(0.259f, 0.259f, 0.259f, 1.00f);      // Default header color
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);  // Darker hover color
    colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);   // Slightly darker active header

    // Optional: tighter layout like Unity
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.WindowPadding = ImVec2(8, 8);  // Window padding to give breathing space
    style.WindowRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.ScrollbarRounding = 2.0f;
}

#else

#include "tools/inspector.hpp"

using namespace std;
using namespace bee;

Inspector::Inspector() {}
Inspector::~Inspector() {}
void Inspector::Inspect(float dt) {}
void Inspector::Inspect(Entity entity, Transform& transform, std::set<Entity>& inspected) {}
void Inspector::Inspect(const char* name, float& f) {}
void Inspector::Inspect(const char* name, int& i) {}
void Inspector::Inspect(const char* name, bool& b) {}
void Inspector::Inspect(const char* name, glm::vec2& v) {}
void Inspector::Inspect(const char* name, glm::vec3& v) {}
void Inspector::Inspect(const char* name, glm::vec4& v) {}

#endif  // BEE_INSPECTOR
