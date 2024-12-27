
#pragma once

#include "Layer.h"
#include "OsgViewSetup.h"

/*! \namespace CAD
 * \brief Application managerment.
*/
namespace CAD {
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

    void PushLayer(Layer* layer)
    {
        if(!layer)
        {
            std::cerr << "layer is invalid!" << std::endl;
            return;
        }

        m_LayerStack.emplace_back(std::unique_ptr<Layer>(layer));
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
    std::vector<std::unique_ptr<Layer>> m_LayerStack;
    std::unique_ptr<OsgWindow::OsgView> m_OsgView;
};

}

