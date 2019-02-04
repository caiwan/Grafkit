#pragma once

#include <vector>
#include <map>

#include "common.h"
#include "core/memory.h"

#include "dxtypes.h"
#include "renderer.h"
#include "shader.h"

namespace Grafkit
{
    __declspec(align(16))
    class MaterialParameters
        //: public Grafkit::IResource, public AlignedNew<MaterialParameter>, public Persistent
    {
        /*
        Todo:
        Shader parameter framework

        store varoius parameter pointers for ea. node
        struct ParamRecord{
            std::string name;
            int id;
            size_t len;
            void *p;
        }

        - store them separately

        map<> params
        map<> resources
        map<> samplers - if needed

        - works as a facade for SetParam, SetResource and SetSampler

        */
    public:
        //MaterialParameter();
        //~MaterialParameter();
    };

    // =========================================================================================================

    __declspec(align(16))
    class Material : public AlignedNew<Material>, public Object
    {
    public:

        struct material_color_t
        {
            // it does need a ctor 
            // to avoid compile failures for some reason
            material_color_t() {}

            float4 diffuse, specular, emission;

            template <class A>
            void Serialize(A& ar) { ar & diffuse & specular & emission; }
        };

        struct material_params_t
        {
            // it does need a ctor 
            // to avoid compile failures for some reason
            material_params_t() {}

            union
            {
                struct
                {
                    float intensity; // p0.x or specular level, 
                    float hardness; // p0.y or shininess
                    float refraction; // p0.z
                    float roughness; // p0.w
                    float slope; // p1.x
                    float emission; // p1.y emission level of diffuse color
                    float layer; // p1.z layer or material id multiple shaders are used
                    // p1.w
                    // p2.xyzw
                    // p3.xyzw
                };

                struct
                {
                    float4 param0, param1, param2, param3;
                };

                float param[16];
            };

            template <class A>
            void Serialize(A& ar) { ar & param; }
        };

        enum texture_type_e
        {
            // regular types	
            TT_diffuse, ///< 1st map diffuse = albedo

            TT_metallic,
            TT_roughness,
            TT_emission,

            TT_alpha, ///< alpha map
            TT_normal, ///< normal map
            TT_bump, ///< bump map

            TT_aux0, ///< aux texture, used for pretty much everything else
            TT_aux1, ///< aux texture, used for pretty much everything else
            TT_aux2, ///< aux texture, used for pretty much everything else
            TT_aux3, ///< aux texture, used for pretty much everything else

            TT_COUNT // count
        };

        Material();
        //~Material();

        // qnd thingy
        material_color_t& Colors() { return m_colors; }
        material_params_t& Params() { return m_params; }

        TextureResRef GetTexture(std::string bindName) const;

        void SetTexture(TextureResRef texture, std::string bindName);
        void AddTexture(TextureResRef texture, std::string bindName);
        void RemoveTexture(TextureResRef texture, std::string bindName);

        void SetTexture(TextureResRef texture, texture_type_e slot);
        void AddTexture(TextureResRef texture, texture_type_e slot);
        void RemoveTexture(TextureResRef texture, texture_type_e slot);

        virtual void Render(Renderer& render, ShaderRes& vertexShader, ShaderRes& pixelShader);

        // --- 
        // persisthez kell
        bool GetTextureMap(std::map<std::string, TextureResRef>& textureMap) const;

    protected:

        struct material_color_t m_colors;
        struct material_params_t m_params;

        std::map<std::string, TextureResRef> m_textures;

        SERIALIZE(Grafkit::Material, 1, ar)
        {
            ar & m_colors & m_params;
        }

    private:
        int m_colors_id;
        int m_params_id;
    };

    typedef Ref<Material> MaterialRef;
}
