
#include "Application.h"

#include "TestLayer1.h"
#include "TestLayer2.h"

namespace Hera
{
class Heranut : public HeraGui::Application
    {
    public:
        Heranut(const HeraGui::ApplicationSpecification& specification)
                : HeraGui::Application(specification)
        {
            PushLayer(new TestLayer1());
            PushLayer(new TestLayer2());
        }

    };

    HeraGui::Application* CreateApplication(HeraGui::ApplicationCommandLineArgs args)
    {
        HeraGui::ApplicationSpecification spec;
        spec.Name = "OSG CAD";
        spec.CommandLineArgs = args;

        return new Hera::Heranut(spec);
    }

}


