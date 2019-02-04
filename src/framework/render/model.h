/**
*/

#pragma once

#include <array>

#include "common.h"

#include "Actor.h"
#include "renderer.h"

namespace Grafkit
{
    //class Mesh;
    //typedef Ref<Mesh> MeshRef;

    //class Material;
    //typedef Ref<Material> MaterialRef;

    /**
    Stores extended data for a mesh, that makes it a model
    */

    __declspec(align(16)) class Model : public AlignedNew<Model>, public Entity3D
    {
    public:
        Model();
        explicit Model(MeshRef mesh);
        explicit Model(MeshRef mesh, MaterialRef material);
        //~Model();

        MeshRef GetMesh() const;
        void SetMesh(MeshRef model);

        MaterialRef GetMaterial() const;
        void SetMaterial(MaterialRef material);

        void Calculate(Renderer& deviceContext, const ActorRef &parent = nullptr) override;
        void Render(Renderer& deviceContext, SceneGraph & scene) override;
        void Build(Renderer& deviceContext, SceneGraph & scene) override;

        void SetGeometryShader(ShaderResRef shader);
        ShaderResRef GetGeometryShader() const;

    private:
        MaterialRef m_material;
        MeshRef m_mesh;

        ShaderResRef m_geometryShader;

        SERIALIZE(Grafkit::Model, 1, ar)
        {
            assert(0);
        }

    };

}
