#pragma once

namespace bee
{

class EntityComponentSystem;
class FileIO;
class ResourceManager;
class Device;
class Input;
class Audio;
class Inspector;
class Serializer;
class Profiler;
class ThreadPool;

class EngineClass
{
public:
    void Initialize();
    void Shutdown();
    void Run();

    void ToggleStep() { m_doSingleStep = !m_doSingleStep; }
    void TogglePlay() { m_isPlaying = !m_isPlaying; }
    bool IsPlaying() { return m_isPlaying; }
    bool IsStepping() { return m_doSingleStep; }

    FileIO& FileIO() { return *m_fileIO; }
    ResourceManager& Resources() { return *m_resources; }
    Device& Device() { return *m_device; }
    Input& Input() { return *m_input; }
    Audio& Audio() { return *m_audio; }
    Inspector& Inspector() { return *m_inspector; }
    Serializer& Serializer() { return *m_serializer; }
    EntityComponentSystem& ECS() { return *m_ECS; }

private:
    bee::FileIO* m_fileIO = nullptr;
    bee::ResourceManager* m_resources = nullptr;
    bee::Device* m_device = nullptr;
    bee::Input* m_input = nullptr;
    bee::Audio* m_audio = nullptr;
    bee::Inspector* m_inspector = nullptr;
    bee::Serializer* m_serializer = nullptr;
    EntityComponentSystem* m_ECS = nullptr;

    bool m_isPlaying = true;
    bool m_doSingleStep = false;
};

extern EngineClass Engine;

}  // namespace bee
