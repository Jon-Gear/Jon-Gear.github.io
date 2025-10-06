#include "core/engine.hpp"

#include <chrono>
#include <iostream>

#include "core/ecs.hpp"
#include "core/device.hpp"
#include "core/fileio.hpp"
#include "core/input.hpp"
#include "core/audio.hpp"
#include "core/resource_manager.hpp"
#include "tools/inspector.hpp"
#include "tools/serialization_cereal.hpp"
#include "tools/log.hpp"

using namespace bee;

// Make the engine a global variable on free store memory.
bee::EngineClass bee::Engine;

void EngineClass::Initialize()
{
    Log::Initialize();
    m_fileIO = new bee::FileIO();
    m_resources = new bee::ResourceManager();
    m_device = new bee::Device();
    m_input = new bee::Input();
    m_audio = new bee::Audio();
    m_inspector = new bee::Inspector();
    m_serializer = new bee::Serializer();
    m_ECS = new EntityComponentSystem();
}

void EngineClass::Shutdown()
{
    delete m_ECS;
    delete m_serializer;
    delete m_inspector;
    delete m_input;
    delete m_audio;
    delete m_device;
    delete m_resources;
    delete m_fileIO;
}

void EngineClass::Run()
{
    using clock = std::chrono::high_resolution_clock;
    auto previous = clock::now();

    while (!m_device->ShouldClose())
    {
        auto current = clock::now();
        std::chrono::duration<float> frameTime = current - previous;
        float dt = frameTime.count();

        // Update subsystems
        m_input->Update();
        m_audio->Update();
        m_ECS->UpdateSystems(dt);
        m_ECS->RemovedDeleted();

        // Rendering phase
        m_device->BeginFrame();
        m_ECS->RenderSystems();
        m_inspector->Inspect(dt);
        m_device->EndFrame();

        // Device update (poll events, swap buffers, etc.)
        m_device->Update();

        previous = current;
    }
}
