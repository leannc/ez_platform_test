
#include "Application.h"

#include "TestLayer1.h"
#include "TestLayer2.h"

/*! \namespace OsgImGui
 * \brief Create application instance and manage imGui layers.
*/
namespace OsgImGui
{
class CoreGui : public OsgCAD::Application
    {
    public:
        CoreGui(const OsgCAD::ApplicationSpecification& specification)
                : OsgCAD::Application(specification)
        {
            PushLayer<TestLayer1>();
            PushLayer<TestLayer2>();
        }

    };

    OsgCAD::Application* CreateApplication(OsgCAD::ApplicationCommandLineArgs args)
    {
        OsgCAD::ApplicationSpecification spec;
        spec.Name = "OSG CAD";
        spec.CommandLineArgs = args;

        return new CoreGui(spec);
    }

}


