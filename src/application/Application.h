
#pragma once

#include <osgViewer/Viewer>

#include "Layer.h"
#include "OsgViewSetup.h"

/*! \namespace OsgCAD
 * \brief Application managerment.
*/
namespace OsgCAD {
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
    uint32_t Width = 1280;
    uint32_t Height = 800;
    std::string Name = "OsgCAD Application";
    std::string WorkingDirectory;
    ApplicationCommandLineArgs CommandLineArgs;
};

/*! \class Application
 * \brief A application managerment class that contains OSG and imGui layers.
*/
class Application
{
public:
    Application(const ApplicationSpecification& specification);
    virtual ~Application();

    inline static Application& Get() {return *s_Instance;}
    const ApplicationSpecification& GetSpecification() const { return m_Specification; }

    template<typename T>
    void PushLayer()
    {
        static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
        const auto &layer = std::make_shared<T>();
        m_LayerStack.emplace_back(layer);
        layer->OnAttach();
    }

    void PushLayer(const std::shared_ptr<Layer>& layer)
    {
        m_LayerStack.emplace_back(layer);
        layer->OnAttach();
    }

    void Close();

    /*! A method to run the application in a loop. */
    void Run();

private:
    static Application* s_Instance;
    ApplicationSpecification m_Specification;

    bool m_nogui = false;
    bool m_Running = true;
    std::vector<std::shared_ptr<Layer>> m_LayerStack;
    std::unique_ptr<OsgWindow::OsgView> m_OsgView;
};

}

