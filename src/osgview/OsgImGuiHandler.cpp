#include "OsgImGuiHandler.hpp"
#include <iostream>
#include <osg/Camera>
#include <osgUtil/GLObjectsVisitor>
#include <osgUtil/SceneView>
#include <osgUtil/UpdateVisitor>
#include <osgViewer/ViewerEventHandlers>

#include "imgui.h"
#include "imgui_impl_opengl3.h"

struct OsgImGuiHandler::ImGuiNewFrameCallback : public osg::Camera::DrawCallback
{
    ImGuiNewFrameCallback(OsgImGuiHandler& handler)
        : handler_(handler)
    {
    }

    void operator()(osg::RenderInfo& renderInfo) const override
    {
        handler_.newFrame(renderInfo);
    }

private:
    OsgImGuiHandler& handler_;
};

struct OsgImGuiHandler::ImGuiRenderCallback : public osg::Camera::DrawCallback
{
    ImGuiRenderCallback(OsgImGuiHandler& handler)
        : handler_(handler)
    {
    }

    void operator()(osg::RenderInfo& renderInfo) const override
    {
        handler_.render(renderInfo);
    }

private:
    OsgImGuiHandler& handler_;
};

OsgImGuiHandler::OsgImGuiHandler()
    : time_(0.0f), mousePressed_{false}, mouseWheel_(0.0f), initialized_(false)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    init();
}

/**
 * Imporant Note: Dear ImGui expects the control Keys indices not to be
 * greater thant 511. It actually uses an array of 512 elements. However,
 * OSG has indices greater than that. So here I do a conversion for special
 * keys between ImGui and OSG.
 */
static int ConvertFromOSGKey(int key)
{
    using KEY = osgGA::GUIEventAdapter::KeySymbol;

    switch (key)
    {
        case KEY::KEY_Tab:
            return ImGuiKey_Tab;
        case KEY::KEY_Left:
            return ImGuiKey_LeftArrow;
        case KEY::KEY_Right:
            return ImGuiKey_RightArrow;
        case KEY::KEY_Up:
            return ImGuiKey_UpArrow;
        case KEY::KEY_Down:
            return ImGuiKey_DownArrow;
        case KEY::KEY_Page_Up:
            return ImGuiKey_PageUp;
        case KEY::KEY_Page_Down:
            return ImGuiKey_PageDown;
        case KEY::KEY_Home:
            return ImGuiKey_Home;
        case KEY::KEY_End:
            return ImGuiKey_End;
        case KEY::KEY_Delete:
            return ImGuiKey_Delete;
        case KEY::KEY_BackSpace:
            return ImGuiKey_Backspace;
        case KEY::KEY_Return:
            return ImGuiKey_Enter;
        case KEY::KEY_Escape:
            return ImGuiKey_Escape;
        case KEY::KEY_Shift_L:
            return ImGuiKey_LeftShift;
        case KEY::KEY_Shift_R:
            return ImGuiKey_RightShift;
        case KEY::KEY_Control_L:
            return ImGuiKey_LeftCtrl;
        case KEY::KEY_Control_R:
            return ImGuiKey_RightCtrl;
        case KEY::KEY_0:
            return ImGuiKey_0;
        case KEY::KEY_1:
            return ImGuiKey_1;
        case KEY::KEY_2:
            return ImGuiKey_2;
        case KEY::KEY_3:
            return ImGuiKey_3;
        case KEY::KEY_4:
            return ImGuiKey_4;
        case KEY::KEY_5:
            return ImGuiKey_5;
        case KEY::KEY_6:
            return ImGuiKey_6;
        case KEY::KEY_7:
            return ImGuiKey_7;
        case KEY::KEY_8:
            return ImGuiKey_8;
        case KEY::KEY_9:
            return ImGuiKey_9;
        case KEY::KEY_A:
            return ImGuiKey_A;
        case KEY::KEY_B:
            return ImGuiKey_B;
        case KEY::KEY_C:
            return ImGuiKey_C;
        case KEY::KEY_D:
            return ImGuiKey_D;
        case KEY::KEY_E:
            return ImGuiKey_E;
        case KEY::KEY_F:
            return ImGuiKey_F;
        case KEY::KEY_G:
            return ImGuiKey_G;
        case KEY::KEY_H:
            return ImGuiKey_H;
        case KEY::KEY_I:
            return ImGuiKey_I;
        case KEY::KEY_J:
            return ImGuiKey_J;
        case KEY::KEY_K:
            return ImGuiKey_K;
        case KEY::KEY_L:
            return ImGuiKey_L;
        case KEY::KEY_M:
            return ImGuiKey_M;
        case KEY::KEY_N:
            return ImGuiKey_N;
        case KEY::KEY_O:
            return ImGuiKey_O;
        case KEY::KEY_P:
            return ImGuiKey_P;
        case KEY::KEY_Q:
            return ImGuiKey_Q;
        case KEY::KEY_R:
            return ImGuiKey_R;
        case KEY::KEY_S:
            return ImGuiKey_S;
        case KEY::KEY_T:
            return ImGuiKey_T;
        case KEY::KEY_U:
            return ImGuiKey_U;
        case KEY::KEY_V:
            return ImGuiKey_V;
        case KEY::KEY_W:
            return ImGuiKey_W;
        case KEY::KEY_X:
            return ImGuiKey_X;
        case KEY::KEY_Y:
            return ImGuiKey_Y;
        default: // Not found
            return -1;
    }
}

void OsgImGuiHandler::init()
{
    if(IMGUI_VERSION_NUM < 18717)
    {
        ImGuiIO& io = ImGui::GetIO();

        // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
        io.KeyMap[ImGuiKey_Tab] = ImGuiKey_Tab;
        io.KeyMap[ImGuiKey_LeftArrow] = ImGuiKey_LeftArrow;
        io.KeyMap[ImGuiKey_RightArrow] = ImGuiKey_RightArrow;
        io.KeyMap[ImGuiKey_UpArrow] = ImGuiKey_UpArrow;
        io.KeyMap[ImGuiKey_DownArrow] = ImGuiKey_DownArrow;
        io.KeyMap[ImGuiKey_PageUp] = ImGuiKey_PageUp;
        io.KeyMap[ImGuiKey_PageDown] = ImGuiKey_PageDown;
        io.KeyMap[ImGuiKey_Home] = ImGuiKey_Home;
        io.KeyMap[ImGuiKey_End] = ImGuiKey_End;
        io.KeyMap[ImGuiKey_Delete] = ImGuiKey_Delete;
        io.KeyMap[ImGuiKey_Backspace] = ImGuiKey_Backspace;
        io.KeyMap[ImGuiKey_Enter] = ImGuiKey_Enter;
        io.KeyMap[ImGuiKey_Escape] = ImGuiKey_Escape;
        io.KeyMap[ImGuiKey_A] = osgGA::GUIEventAdapter::KeySymbol::KEY_A;
        io.KeyMap[ImGuiKey_C] = osgGA::GUIEventAdapter::KeySymbol::KEY_C;
        io.KeyMap[ImGuiKey_V] = osgGA::GUIEventAdapter::KeySymbol::KEY_V;
        io.KeyMap[ImGuiKey_X] = osgGA::GUIEventAdapter::KeySymbol::KEY_X;
        io.KeyMap[ImGuiKey_Y] = osgGA::GUIEventAdapter::KeySymbol::KEY_Y;
        io.KeyMap[ImGuiKey_Z] = osgGA::GUIEventAdapter::KeySymbol::KEY_Z;
    }
}

void OsgImGuiHandler::setCameraCallbacks(osg::Camera* camera)
{
    camera->setPreDrawCallback(new ImGuiNewFrameCallback(*this));
    camera->setPostDrawCallback(new ImGuiRenderCallback(*this));
}

void OsgImGuiHandler::newFrame(osg::RenderInfo& renderInfo)
{
    ImGui_ImplOpenGL3_NewFrame();

    ImGuiIO& io = ImGui::GetIO();

    osg::Viewport* viewport = renderInfo.getCurrentCamera()->getViewport();

    io.DisplaySize = ImVec2(viewport->width(), viewport->height());

    double currentTime = renderInfo.getView()->getFrameStamp()->getSimulationTime();
    io.DeltaTime = currentTime - time_ + 0.0000001;
    time_ = currentTime;

    for (int i = 0; i < 3; i++)
    {
        io.MouseDown[i] = mousePressed_[i];
    }

    io.MouseWheel = mouseWheel_;
    mouseWheel_ = 0.0f;

    ImGui::NewFrame();
}

void OsgImGuiHandler::render(osg::RenderInfo&)
{
    drawUi();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool OsgImGuiHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    if (!initialized_)
    {
        auto view = aa.asView();
        if (view)
        {
            setCameraCallbacks(view->getCamera());
            initialized_ = true;
        }
    }

    ImGuiIO& io = ImGui::GetIO();
    const bool wantCaptureMouse = io.WantCaptureMouse;
    const bool wantCaptureKeyboard = io.WantCaptureKeyboard;

    switch (ea.getEventType())
    {
        case osgGA::GUIEventAdapter::KEYDOWN:
        case osgGA::GUIEventAdapter::KEYUP:
        {
            const bool isKeyDown = ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN;
            const int c = ea.getKey();
            const int mod_c = ea.getUnmodifiedKey();
            const int special_key = ConvertFromOSGKey(c);
            const int mod_key = ConvertFromOSGKey(mod_c);

            // std::cout << "c: " << c << " " << "mod_c: " << mod_c << std::endl;
            // std::cout << "special_key: " << special_key << " " << "mod_key: " << mod_key << std::endl;

            if (mod_key > 0)
            {
                assert((mod_key >= ImGuiKey_NamedKey_BEGIN && mod_key < ImGuiKey_NamedKey_END) && "ImGui KeysMap is an array of 512");

                if(IMGUI_VERSION_NUM >= 18717)
                {
                    io.AddKeyEvent(ImGuiMod_Ctrl, ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL);
                    io.AddKeyEvent(ImGuiMod_Shift, ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT);
                    io.AddKeyEvent(ImGuiMod_Alt, ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT);
                    io.AddKeyEvent(ImGuiMod_Super, ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SUPER);

                    io.AddKeyEvent( (ImGuiKey)mod_key, isKeyDown );
                }
            }
            else
            {
                // if(IMGUI_VERSION_NUM >= 18717)
                // {
                //     io.AddKeyEvent(ImGuiMod_Ctrl, false);
                //     io.AddKeyEvent(ImGuiMod_Shift, false);
                //     io.AddKeyEvent(ImGuiMod_Alt, false);
                //     io.AddKeyEvent(ImGuiMod_Super, false);

                //     io.AddKeyEvent( ImGuiKey_LeftCtrl, false );
                //     io.AddKeyEvent( ImGuiKey_LeftShift, false );
                //     io.AddKeyEvent( ImGuiKey_LeftAlt, false );
                //     io.AddKeyEvent( ImGuiKey_LeftSuper, false );
                // }
            }

            if (special_key > 0)
            {
                assert((special_key >= ImGuiKey_NamedKey_BEGIN && special_key < ImGuiKey_NamedKey_END) && "ImGui KeysMap is an array of 512");

                if(IMGUI_VERSION_NUM < 18717)
                {
                    io.KeysDown[special_key] = isKeyDown;

                    io.KeyCtrl = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL;
                    io.KeyShift = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT;
                    io.KeyAlt = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT;
                    io.KeySuper = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SUPER;
                }
                else
                {
                    io.AddKeyEvent( (ImGuiKey)special_key, isKeyDown );
                }
            }

            if (isKeyDown && c > 0 && c < 0xFF)
            {
                if( !(special_key > -1 && mod_key > -1) )
                    io.AddInputCharacter((unsigned short)c);
            }

            if(IMGUI_VERSION_NUM >= 18717)
            {
#ifndef IMGUI_DISABLE_OBSOLETE_KEYIO
                for (int n = ImGuiKey_NamedKey_BEGIN; n < ImGuiKey_GamepadRStickDown; n++)
                {
                    // std::cout << n << ":" << io.KeyMap[n] << std::endl;
                    io.AddKeyEvent((ImGuiKey)n, false);
                }
#endif

                io.AddKeyEvent(ImGuiMod_Ctrl, false);
                io.AddKeyEvent(ImGuiMod_Shift, false);
                io.AddKeyEvent(ImGuiMod_Alt, false);
                io.AddKeyEvent(ImGuiMod_Super, false);
                io.AddKeyEvent(ImGuiKey_ReservedForModCtrl, false);
                io.AddKeyEvent(ImGuiKey_ReservedForModShift, false);
                io.AddKeyEvent(ImGuiKey_ReservedForModAlt, false);
                io.AddKeyEvent(ImGuiKey_ReservedForModSuper, false);

                io.KeyCtrl = false;
                io.KeyShift = false;
                io.KeyAlt = false;
                io.KeySuper = false;
            }

            return wantCaptureKeyboard;
        }
        case (osgGA::GUIEventAdapter::RELEASE):
        case (osgGA::GUIEventAdapter::PUSH):
        {
            io.MousePos = ImVec2(ea.getX(), io.DisplaySize.y - ea.getY());
            mousePressed_[0] = ea.getButtonMask() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON;
            mousePressed_[1] = ea.getButtonMask() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON;
            mousePressed_[2] = ea.getButtonMask() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON;
            return wantCaptureMouse;
        }
        case (osgGA::GUIEventAdapter::DRAG):
        case (osgGA::GUIEventAdapter::MOVE):
        {
            io.MousePos = ImVec2(ea.getX(), io.DisplaySize.y - ea.getY());
            return wantCaptureMouse;
        }
        case (osgGA::GUIEventAdapter::SCROLL):
        {
            mouseWheel_ = ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP ? 1.0 : -1.0;
            return wantCaptureMouse;
        }
        default:
        {
            return false;
        }
    }

    return false;
}
