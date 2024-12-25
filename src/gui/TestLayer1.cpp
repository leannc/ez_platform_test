#include "TestLayer1.h"
#include <imgui.h>

namespace Hera {


    TestLayer1::TestLayer1()
            : Layer("TestLayer1")
    {
    }

    void TestLayer1::OnAttach()
    {

    }

    void TestLayer1::OnDetach()
    {

    }


    void TestLayer1::OnImGuiRender()
    {
        ImGui::ShowDemoWindow();
    }

}
