#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <memory>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgGA/TrackballManipulator>

#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include "osgViewer/api/SDL2/sdlgraphicswindow.hpp"
#include "OsgImGuiHandler.hpp"
#include <SDL.h>

/*! \namespace OsgWindow
 * \brief OSG window managerment.
*/
namespace OsgWindow {

/*! \class OsgView
 * \brief SDL_Window and osg initialization
*/
class OsgView
{
public:
    OsgView() {}
    virtual ~OsgView();

    void initOsgView( const std::string &title, uint32_t width, uint32_t height, const std::function<void()>& render_func );
    void addEventHandler(osgGA::EventHandler* eventHandler);
    void setRealizeOperation(const std::function<void()>& func, const std::string &name);
    void exe( bool running, bool nogui = false );

public:
    /*! A method to visit osgViewer. */
    osgViewer::Viewer& getViewer() {return m_viewer;}

    /*! A pointer to root node. */
    osg::ref_ptr<osg::Group>& getRootNode() {return m_root;}

    /*! A method to load node data. */
    void loadData(const std::string &filename = "");

private:
    /*! A pointer to main window manager. */
    osg::ref_ptr<SDLUtil::GraphicsWindowSDL2> m_graphicsWindow;

    /*! Root node for scene data.  */
    osg::ref_ptr<osg::Group> m_root;

    /*! Construct the viewer. */
    osgViewer::Viewer m_viewer;
};

}
