
#include "Application.h"

#include "TestLayer1.h"
#include "TestLayer2.h"

/*! \namespace OsgImGui
 * \brief Create application instance and manage imGui layers.
*/
namespace OsgImGui
{
class CoreGui : public CAD::Application
    {
    public:
        CoreGui(const CAD::ApplicationSpecification& specification)
                : CAD::Application(specification)
        {
            PushLayer( new TestLayer1() );
            PushLayer( new TestLayer2() );
        }

    };

    CAD::Application* CreateApplication(CAD::ApplicationCommandLineArgs args)
    {
        CAD::ApplicationSpecification spec;
        spec.Name = "OSG CAD";
        spec.CommandLineArgs = args;

        return new CoreGui(spec);
    }

}


