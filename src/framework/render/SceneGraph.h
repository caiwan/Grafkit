#pragma once

#include <stack>
#include <list>
#include <vector>

#include "common.h"

#include "math/matrix.h"
#include "renderer.h"

namespace Grafkit {
    __declspec(align(16))
        class SceneGraph : public AlignedNew<SceneGraph>, public Object
    {
    public:
        SceneGraph();

        SceneGraph(const SceneGraph& other) = delete;
        SceneGraph& operator=(const SceneGraph& other) = delete;

        //~SceneGraph();

        // Init, shutdown 
        void Initialize();
        void Initialize(ActorRef root); // legacy stuff 
        //void Shutdown();

        void Update();
        void Render(Renderer & render, const CameraRef & camera);

        // Add / Get node
        //void AddNode(const ActorRef& actor);
        ActorRef GetNode(const std::string &name);

        void SetRootNode(const ActorRef &root) { m_root = root; } 
        ActorRef GetRootNode() const { return m_root; } 

        size_t GetNodeCount() const { return m_nodes.size(); } 
        ActorRef GetNode(size_t id) { return m_nodes.at(id).first; } 

        // --- 

        Matrix& GetWorldMatrix() { return this->m_currentWorldMatrix; }

        ShaderRes & GetVShader() { return m_vertexShader; }
        ShaderRes & GetPShader() { return m_pixelShader; }

        void SetVShader(const ShaderRes &VS) { m_vertexShader = VS; }
        void SetPShader(const ShaderRes &FS) { m_pixelShader = FS; }

        void BuildScene(Renderer & render);
        void BuildScene(Renderer & render, const ShaderRes &vs, const ShaderRes &ps); //legacy stuff

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

        ShaderRes m_vertexShader;
        ShaderRes m_pixelShader;

        std::vector<std::pair<ActorRef, ActorRef>> m_nodes; //child, parent
        std::map<std::string, ActorRef> m_nodeMap;

        std::set<Entity3DRef> m_entities;

    private:
        //double m_tAnim;

        Matrix m_currentWorldMatrix;
        std::stack<Matrix> m_worldMatrixStack;

        // -- persistent
    //protected:
    //    void Serialize(Archive& ar) override;
        PERSISTENT_DECL(Grafkit::SceneGraph, 1);
    };
}
