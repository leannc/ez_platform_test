/*! \mainpage OSGCAD - 3D platform software based on OSG and imGui.
 *
 * \section Description
 *
 * This documentation site covers the OSGCAD API mainly.
 *
 */


#include <iostream>
#include "application/CoreApp.h"


int main(int argc, char** argv)
{
    std::unique_ptr<CAD::Application> app;
    app.reset(OsgImGui::CreateApplication({ argc, argv }));

    if(!app.get())
        return -1;

    app->Run();

    return 0;
}

