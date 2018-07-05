#pragma once

#include "common.h"

#include "render/renderer.h"
#include "utils/Event.h"
#include "utils/Command.h"

#include "utils/ViewModule.h"

#include "EditorView.h"

namespace Grafkit
{
    class IResourceManager;
}

namespace GkDemo
{
    class Context;
    class Demo;
}

namespace Idogep
{
    class Editor;
    class EditorApplication;

    class Document;

    class MusicProxy;

    class Editor : public Controller
    {
    public:
        explicit Editor(Grafkit::Renderer& render, /*Grafkit::IResourceManager* const & resman,*/ GkDemo::Context*const& context);
        ~Editor();

        void Initialize() override;
        //void MediateSiblingModule(Ref<Controller> module);

        void InitializeDocument();

        bool RenderFrame();

        //void RequestLoad();
        //void RequestReload();

        void NewDocument();
        void SaveDocument();
        void OpenDocument(std::string filename);

        //void ExitingApp(bool &isPreventExit);

        // ... 

        // Events
        //Event<std::string&> onSaveDialog;
        Event<GkDemo::Demo* const &> onDocumentChanged;
        Event<const float &> onDemoTimeChanged;

        // Demo's own manager
        Grafkit::IResourceManager* GetResourceManager() const;

        // Application's own manager
        //Grafkit::IResourceManager* GetApplicationResourceManager() const;

        void SetDirty() { m_isDirty = true; }
        bool IsDirty() const { return m_isDirty; }

        //Document * const & GetDocument() const { return m_document; }
        CommandStack* GetCommandStack() const { return m_commandStack; }

        MusicProxy* GetMusicProxy() const { return m_musicProxy; }

    private:
        bool DirtyCheck() const;

        //Grafkit::IResourceManager* const & m_resourceManager;
        Grafkit::Renderer& m_render;

        MusicProxy* m_musicProxy;

        CommandStack* m_commandStack;

        bool m_precalcRequested;
        bool m_reloadRequested;

        bool m_isDirty;

        GkDemo::Demo* m_demo;
        GkDemo::Context*const& m_context;
    };
}
