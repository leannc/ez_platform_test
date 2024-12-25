#pragma once

#include "Layer.h"

namespace Hera {

class TestLayer1 : public HeraGui::Layer
    {
    public:
        TestLayer1();
        virtual ~TestLayer1() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;
    };
}


