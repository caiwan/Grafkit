/**
*/

#pragma once

#include <array>

#include "common.h"

#include "Actor.h"
#include "renderer.h"

namespace Grafkit
{
    /**
    Stores extended data for a mesh, that makes it a model
    */

    __declspec(align(16)) class Model : public Entity3D, public AlignedNew<Model>
    {
    public:
        Model();
        explicit Model(MeshRef mesh);
        explicit Model(MeshRef mesh, MaterialRef material);
        ~Model();

        MeshRef GetMesh() const;
        void SetMesh(MeshRef model);

        MaterialRef GetMaterial() const;
        void SetMaterial(MaterialRef material);

        void Calculate(Renderer& deviceContext, ActorRef patent = nullptr) override {}
        void Render(Renderer& deviceContext, SceneGraph* const & scene) override;
        void Build(Renderer& deviceContext, SceneGraph* const & scene) override;

        void SetGeometryShader(ShaderResRef shader);
        ShaderResRef GetGeometryShader() const;

    private:
        MaterialRef m_material;
        MeshRef m_mesh;

        ShaderResRef m_geometryShader;

        PERSISTENT_DECL(Grafkit::Model, 1)

    protected:
        void Serialize(Archive& ar) override { _Serialize(ar); }
        void _Serialize(Archive& ar);

    };

}
