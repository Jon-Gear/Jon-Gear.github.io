#pragma once
#include "editor/inspectable.hpp"
#include "tools/diff_util.hpp"

#include "scripting/script_component.hpp"

#include "components/camera.hpp"
#include "components/layer_component.hpp"
#include "components/rigidbody.hpp"
#include "components/voxel_collider.hpp"
#include "components/voxel_renderer.hpp"
#include "components/environment.hpp"
#include "components/ui_component.hpp"
#include "components/sphere_light.hpp"

namespace kudzu {
#ifdef KUDZU_INSPECTOR
class TransformComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Transform"); }
    std::string get_component_icon() const { return std::string(ICON_MD_AXIS_ARROW); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return false; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);
};

class LayerComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Layer"); }
    std::string get_component_icon() const { return std::string(ICON_MD_DATABASE); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return false; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<LayerComponent> diff_util;
};

class ScriptComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Scripts"); }
    std::string get_component_icon() const { return std::string(ICON_MD_CODE_BRACES); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<ScriptComponent> diff_util;
};

class CameraComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Camera"); }
    std::string get_component_icon() const { return std::string(ICON_MD_VIDEO_VINTAGE); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<Camera> diff_util;
};

class RigidbodyComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Rigidbody"); }
    std::string get_component_icon() const { return std::string(ICON_MD_CUBE_SEND); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<RigidBody> diff_util;
};

class VoxelColliderComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Voxel Collider"); }
    std::string get_component_icon() const { return std::string(ICON_MD_CUBE_OUTLINE); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<VoxelCollider> diff_util;
};

class BoxTriggerComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Box Trigger"); }
    std::string get_component_icon() const { return std::string(ICON_MD_SELECT); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);
};

class VoxelRendererComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Voxel Renderer"); }
    std::string get_component_icon() const { return std::string(ICON_MD_CUBE); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<VoxelRenderer> diff_util;
};

class SilhouetteRendererComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Silhouette Renderer"); }
    std::string get_component_icon() const { return std::string(ICON_MD_HUMAN); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity) {}
    void after_edit(entt::entity) {}

   private:
    DiffUtil<VoxelRenderer> diff_util;
};

class ImageRendererComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Image"); }
    std::string get_component_icon() const { return std::string(ICON_MD_IMAGE); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<ImageRenderer> diff_util;
};

class RigModelComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Animated Rig"); }
    std::string get_component_icon() const { return std::string(ICON_MD_CUBE); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<RigModel> diff_util;
};

class EnvironmentComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Environment"); }
    std::string get_component_icon() const { return std::string(ICON_MD_IMAGE_FILTER_HDR); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<Environment> diff_util;
};

class UIComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("UI Component"); }
    std::string get_component_icon() const { return std::string(ICON_MD_VIEW_MODULE); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<UIComponent> diff_util;
};

class RigControllerComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Rig Controller"); }
    std::string get_component_icon() const { return std::string(ICON_MD_CONTROLLER_CLASSIC); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);
};

class ButtonComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Button"); }
    std::string get_component_icon() const { return std::string(ICON_MD_BUTTON_CURSOR); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<Button> diff_util;
};

class PrefabComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Prefab"); }
    std::string get_component_icon() const { return std::string(ICON_MD_CUBE); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);
};

class ParticleSystemComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Particle System"); }
    std::string get_component_icon() const { return std::string(ICON_MD_FLARE); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);
};

class SphereLightComponentInspector : public IComponentInspector {
    std::string get_component_name() const { return std::string("Sphere Light"); }
    std::string get_component_icon() const { return std::string(ICON_MD_LIGHTBULB); }
    bool has_component(entt::entity entity);
    void on_component(entt::entity entity);
    void add_component(entt::entity entity);
    void remove_component(entt::entity entity);
    bool can_remove_component() { return true; }
    nlohmann::ordered_json copy_component(entt::entity entity);
    void paste_component(entt::entity entity, nlohmann::ordered_json json);

    void before_edit(entt::entity entity);
    void after_edit(entt::entity entity);

   private:
    DiffUtil<SphereLight> diff_util;
};

#else
class LayerComponentInspector : public IComponentInspector {};
class TransformComponentInspector : public IComponentInspector {};
class CameraComponentInspector : public IComponentInspector {};
class RigidbodyComponentInspector : public IComponentInspector {};
class BoxTriggerComponentInspector : public IComponentInspector {};
class VoxelColliderComponentInspector : public IComponentInspector {};
class VoxelRendererComponentInspector : public IComponentInspector {};
class SilhouetteRendererComponentInspector : public IComponentInspector {};
class ImageRendererComponentInspector : public IComponentInspector {};
class ScriptComponentInspector : public IComponentInspector {};
class RigModelComponentInspector : public IComponentInspector {};
class EnvironmentComponentInspector : public IComponentInspector {};
class UIComponentInspector : public IComponentInspector {};
class RigControllerComponentInspector : public IComponentInspector {};
class ButtonComponentInspector : public IComponentInspector {};
class PrefabComponentInspector : public IComponentInspector {};
class ParticleSystemComponentInspector : public IComponentInspector {};
class SphereLightComponentInspector : public IComponentInspector {};
#endif
class ComponentInspector {
    // this is also the order in which the components will be displayed
    [[maybe_unused]] PrefabComponentInspector prefab_component_inspector;
    [[maybe_unused]] LayerComponentInspector collision_layer_component_inspector;
    [[maybe_unused]] TransformComponentInspector transform_component_inspector;
    [[maybe_unused]] UIComponentInspector ui_component_inspector;
    [[maybe_unused]] CameraComponentInspector camera_component_inspector;
    [[maybe_unused]] VoxelRendererComponentInspector voxel_renderer_component_inspector;
    [[maybe_unused]] SilhouetteRendererComponentInspector silhouette_renderer_component_inspector;
    [[maybe_unused]] ImageRendererComponentInspector image_renderer_component_inspector;
    [[maybe_unused]] VoxelColliderComponentInspector voxel_collider_component_inspector;
    [[maybe_unused]] RigidbodyComponentInspector rigidbody_component_inspector;
    [[maybe_unused]] BoxTriggerComponentInspector box_trigger_component_inspector;
    [[maybe_unused]] ScriptComponentInspector script_component_inspector;
    [[maybe_unused]] RigModelComponentInspector rig_model_component_inspector;
    [[maybe_unused]] EnvironmentComponentInspector environment_component_inspector;
    [[maybe_unused]] RigControllerComponentInspector rig_controller_component_inspector;
    [[maybe_unused]] ButtonComponentInspector button_component_inspector;
    [[maybe_unused]] ParticleSystemComponentInspector particle_system_component_inspector;
    [[maybe_unused]] SphereLightComponentInspector sphere_light_component_inspector;
};

}  // namespace kudzu
