#include <iostream>
#include "application/HeranutApp.h"

int main(int argc, char** argv)
{
    std::unique_ptr<HeraGui::Application> app;
    app.reset(Hera::CreateApplication({ argc, argv }));

    if(!app.get())
        return -1;

    app->Run();

    return 0;
}

