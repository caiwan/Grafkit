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

    typedef Ref<Demo> DemoRef;
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
        explicit Editor(Grafkit::Renderer& render, const std::weak_ptr<GkDemo::Context> & context);
        ~Editor();

        void Initialize(Grafkit::IResourceManager * const & resourceManager) override;

        void InitializeDocument();

        bool RenderFrame();

        //void RequestLoad();
        //void RequestReload();

        void NewDocument();
        void SaveDocument();
        void OpenDocument();

        //void ExitingApp(bool &isPreventExit);

        // ... 

        // Events
        //Event<std::string&> onSaveDialog;
        Event<const GkDemo::DemoRef &> onDocumentChanged;
        Event<const float &> onDemoTimeChanged;

        // Demo's own manager
        Grafkit::IResourceManager* GetResourceManager() const;

        // Application's own manager
        void SetDirty() { m_isDirty = true; }
        bool IsDirty() const { return m_isDirty; }

        CommandStack* GetCommandStack() const { return m_commandStack; }
        MusicProxy* GetMusicProxy() const { return m_musicProxy; }

    private:
        bool DirtyCheck() const;

        Grafkit::Renderer& m_render;

        Ref<EditorView> m_myView;

        MusicProxy* m_musicProxy;
        CommandStack* m_commandStack;

        bool m_precalcRequested;
        bool m_reloadRequested;

        bool m_isDirty;
        bool m_tmpInitTestStuff;

        std::weak_ptr<GkDemo::Context> m_context;
    };
}
