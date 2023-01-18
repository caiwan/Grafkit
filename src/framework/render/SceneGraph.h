#pragma once

#include <stack>
#include <list>
#include <vector>

#include "common.h"

#include "math/matrix.h"
#include "renderer.h"

namespace Grafkit {
    __declspec(align(16))
        class SceneGraph : virtual public Referencable, public AlignedNew<SceneGraph>, public Persistent
    {
    public:
        SceneGraph();
        ~SceneGraph();

        // Init, shutdown 
        void Initialize();
        void Initialize(ActorRef root); // legacy stuff 
        void Shutdown();

        void Update();
        void Render(Renderer & render, CameraRef & camera);

        // Add / Get node
        void AddNode(ActorRef& actor);
        ActorRef GetNode(std::string name);

        void SetRootNode(ActorRef &root) { m_root = root; }
        ActorRef GetRootNode() const { return m_root; }

        size_t GetNodeCount() const { return m_nodes.size(); }
        ActorRef GetNode(size_t id) { return m_nodes[id]; }

        std::string GetName() const { return m_name; }
        void SetName(std::string name) { m_name = name; }

        // --- 

        Matrix& GetWorldMatrix() { return this->m_currentWorldMatrix; }

        ShaderResRef GetVShader() const { return this->m_vertexShader; }
        ShaderResRef GetPShader() const { return this->m_pixelShader; }

        void SetVShader(ShaderResRef &VS) { this->m_vertexShader = VS; }
        void SetPShader(ShaderResRef &FS) { this->m_pixelShader = FS; }

        void BuildScene(Renderer & render);
        void BuildScene(Renderer & render, ShaderResRef vs, ShaderResRef ps); //legacy stuff

    protected:

        // we'll see if we need this or not 
        // or if we have to move it into another layer 

        struct WorldMatrices_t {
            matrix worldMatrix;
            matrix viewMatrix;
            matrix projectionMatrix;
        };

        struct WorldMatrices_t GetWorldMatrices() const { return m_worldMatrices; }

        ActorRef m_root;

        struct WorldMatrices_t m_worldMatrices;

        ShaderResRef m_vertexShader;
        ShaderResRef m_pixelShader;

        std::vector<ActorRef> m_nodes;
        std::map<std::string, ActorRef> m_nodeMap;

        std::set<Entity3D*> m_entities;

        std::string m_name;

    private:
        double m_tAnim;

        Matrix m_currentWorldMatrix;
        std::stack<Matrix> m_worldMatrixStack;

        // -- persistent
    public:
        void Serialize(Archive& ar) override;
        PERSISTENT_DECL(Grafkit::SceneGraph, 1);
    };
}
