#pragma once

#include "Layer.h"

namespace OsgImGui {

class TestLayer2 : public CAD::Layer
    {
    public:
        TestLayer2();
        virtual ~TestLayer2() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;

    private:

    };
}


