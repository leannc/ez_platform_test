
#include "Application.h"

namespace CAD {

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
        if (m_nogui)
        {
            while(m_Running)
            {
                if(!m_Running)
                {
                    break;
                }

                // other task

            }
        }
        else
        {

            if(!m_OsgView)
                return;

            // check events and render frame
            while (!m_OsgView->getViewer().done())
            {
                if(!m_Running)
                {
                    break;
                }

                m_OsgView->getViewer().frame();
            }

            m_OsgView->closeImplementation();
        }
    }

}
