
#pragma once

#include <iostream>
#include <string>
#include <assert.h>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/config/SingleWindow>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgGA/TrackballManipulator>

#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include "osgViewer/api/SDL2/sdlgraphicswindow.hpp"
#include "OsgImGuiHandler.hpp"

#include <SDL.h>
#include <chrono>
#include <memory>

#include "LayerStack.h"

namespace HeraGui {
struct ApplicationCommandLineArgs
{
    int Count = 0;
    char** Args = nullptr;

    const char* operator[](int index) const
    {
        assert(index < Count);
        return Args[index];
    }
};

struct ApplicationSpecification
{
    std::string Name = "Hera Application";
    std::string WorkingDirectory;
    ApplicationCommandLineArgs CommandLineArgs;
};


class Application
{
public:
    Application(const ApplicationSpecification& specification);
    virtual ~Application();

    inline static Application& Get() {return *s_Instance;}
    const ApplicationSpecification& GetSpecification() const { return m_Specification; }

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);
    void ImGuiRender();
    void Close();
    void Run();
    void loadData(int argc, char** argv);

private:
    static Application* s_Instance;
    ApplicationSpecification m_Specification;

    bool m_nogui = false;
    bool m_Running = true;
    bool m_Minimized = false;
    LayerStack m_LayerStack;

private:
    /*!< A pointer to main window manager. */
    osg::ref_ptr<SDLUtil::GraphicsWindowSDL2> m_graphicsWindow;

     /*!< Root node for scene data.  */
    osg::ref_ptr<osg::Group> m_root;

     /*!< Construct the viewer. */
    osgViewer::Viewer m_viewer;
};

}

