#pragma once

#include "../stdafx.h"

#include "Actor.h"

#include "renderer.h"
#include "shader.h"

///@todo minden cuccot egy namespace-be kellene bedobni; 
///@todo valahogy a materialba kellene belejatszani mindezt
///@todo aligned new-t mindenre
namespace Grafkit
{
    class Light;
    typedef Ref<Light> LightRef;
    //typedef std::shared_ptr<Light> LightRef;

    __declspec(align(16))
    class Light : public Entity3D, public AlignedNew<Light>
    {
    public:
        enum light_type_t
        {
            INVALID = 0,
            LT_point = 1,
            LT_directional = 2,
            LT_spot = 3,
            LT_ambient = 4,
            COUNT
        };

        Light();
        explicit Light(light_type_t t);
        ~Light();

        /// @todo ez csak egy QnD Hack, ki kell majd javitani a jovoben
        float4& Position() { return m_position; }
        void Position(float4 p) { m_position = p; }

        //float4 &Direction() { return m_direction; }
        //void Direction(float4 d) { m_direction = d; }

        ///*float4 &Ambient() { return m_light.ambient; }
        float4& Diffuse() { return m_light.color; }
        void Diffuse(float4 c) { m_light.color = c; }
        //float4 &Specular() { return m_light.specular; }*/

        float& ConstantAttenuation() { return m_light.ca; }
        float& LinearAttenuation() { return m_light.la; }
        float& QuardicAttenuation() { return m_light.qa; }

        /*float& Angle() { return m_light.angle; }
        float& Falloff() { return m_light.falloff; }*/

        /// Setup the corresponding constant buffer inside the shader
        //void SetShaderCB(ShaderRef &rPShader);

        void Build(Renderer& deviceContext, SceneGraph* const & scene) override {
        }

        void Calculate(Renderer& deviceContext, const ActorRef & parent = nullptr) override;
        void Render(Renderer& deviceContext, SceneGraph* const & scene) override;

    protected:

        enum light_type_t m_type;

        struct light_t
        {
            // it does need a ctor 
            // to avoid compile failures for some reason
            light_t() {
            }

            union
            {
                struct
                {
                    int type;
                    int id;
                };

                float4 __p0[4];
            };

            float4 position;
            float4 direction;

            float4 ambient;
            float4 diffuse;
            float4 specular;

            union
            {
                float ca, la, qa; //constant, linear, quadric attenuation
                float4 attenuation;
            };

            union
            {
                float angle, falloff;
                float4 spotlight;
            };

            template <class A>
            void Serialize(A& ar)
            {
                ar & type & id;
                ar & position & direction;
                ar & ambient & diffuse & specular;
                ar & ambient & spotlight;
            }
        };

    public:
        struct light2_t
        {
            light2_t() {
            }

            union
            {
                struct
                {
                    float4 position;
                    float4 color;
                };

                union
                {
                    struct
                    {
                        float ca, la, qa;
                        float intensity;
                    };

                    struct
                    {
                        float4 param1, param2;
                    };
                };

                float4 params[4];
            };

            template <class A>
            void Serialize(A& ar) { ar & params; }
        };

        // QND stuff to get the calculated data 
        // to set it into the cbuffer
        virtual size_t GetInternalData(void*const& p);
        light2_t GetData() const { return m_light; }

    private:

        int m_id;
        struct light2_t m_light;
        float4 m_position;

        SERIALIZE(Grafkit::Light, 1, ar)
        {
            Object::Serialize(ar);
            ar & m_id & m_light & m_position;
        }

        //protected:
        //void Serialize(Archive& ar) override;
    };
}
