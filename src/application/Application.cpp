
#include "Application.h"

namespace OsgCAD {

    Application* Application::s_Instance= nullptr;

    Application::Application(const ApplicationSpecification& specification)
            : m_Specification(specification)

    {
        assert(!s_Instance && "Application already exists!");
        s_Instance=this;

        int argc = specification.CommandLineArgs.Count;
        char** argv = specification.CommandLineArgs.Args;

        osg::ArgumentParser arguments(&argc, argv);
        arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName() + " [options]");
        arguments.getApplicationUsage()->addCommandLineOption("-nogui",                   "Run application without gui");
        if (arguments.read("-nogui"))
        {
            std::cout << arguments.getApplicationUsage()->getCommandLineUsage() << std::endl;
            m_nogui = true;
        }

        m_OsgView = std::make_unique<OsgWindow::OsgView>();
        if( m_nogui )
        {

        }
        else
        {
            m_OsgView->initOsgView( specification.Name, specification.Width, specification.Height,
                                   [this]() {
                                       for(auto& layer : m_LayerStack)
                                           layer->OnImGuiRender();
                                   });
        }
    }

    Application::~Application()
    {
        for(auto& layer : m_LayerStack)
        {
            layer->OnDetach();
        }

        m_LayerStack.clear();
    }

    void Application::Close()
    {
        m_Running = false;
    }

    void Application::Run()
    {
        m_Running = true;
        m_OsgView->exe( m_Running, m_nogui );
    }

}
