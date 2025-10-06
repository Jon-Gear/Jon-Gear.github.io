#pragma once
#include "core/resource.hpp"
#include <xsr/include/xsr.hpp>

namespace bee
{
class ModelResource;

class MeshResource : public Resource
{
    friend class ResourceManager;

public:
    MeshResource(const ModelResource& model, int index);

    const xsr::mesh_handle* GetHandleXSR() const { return &m_mesh; }

protected:
    static std::string GetPath(const ModelResource& model, int index);

private:
    xsr::mesh_handle m_mesh;
};
}  // namespace bee
