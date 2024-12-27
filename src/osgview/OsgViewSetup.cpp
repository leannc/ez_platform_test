#include "OsgViewSetup.h"
#include <assert.h>

namespace OsgWindow {
    void checkSDLError(int ret)
    {
        if (ret != 0)
            osg::notify(osg::WARN) << "SDL error: " << SDL_GetError();
    }

    struct SDLWindowDeleter
    {
        void operator()(SDL_Window* window) const
        {
            if (window)
            {
                SDL_DestroyWindow(window);
            }
        }
    };

    int initSDLWindow(std::unique_ptr<SDL_Window, SDLWindowDeleter>& window,
                      osg::ref_ptr<SDLUtil::GraphicsWindowSDL2> &graphicsWindow,
                      const std::string &title,
                      int width,
                      int height)
    {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            osg::notify(osg::FATAL) << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
            return -1;
        }

        SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "1");
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

        checkSDLError(SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8));
        checkSDLError(SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8));
        checkSDLError(SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8));
        checkSDLError(SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0));
        checkSDLError(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24));

        int antialiasing = 1;
        if (antialiasing > 0)
        {
            checkSDLError(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
            checkSDLError(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialiasing));
        }

    #ifdef OSG_GLES3_AVAILABLE
        // Set the OpenGL ES version
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    #endif

        // Create the window
        SDL_Window* rawWindow = SDL_CreateWindow(title.c_str(),
                                                 SDL_WINDOWPOS_UNDEFINED,
                                                 SDL_WINDOWPOS_UNDEFINED,
                                                 width, height,
                                                 SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |
                                                     SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_KEYBOARD_GRABBED);
        if (!rawWindow) {
            osg::notify(osg::FATAL) << "Could not create window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return -1;
        }

        window.reset( rawWindow );

        osg::GraphicsContext::Traits *traits = new osg::GraphicsContext::Traits;
        SDL_GetWindowPosition(window.get(), &traits->x, &traits->y);
        SDL_GetWindowSize(window.get(), &traits->width, &traits->height);
        traits->windowName = SDL_GetWindowTitle(window.get());
        traits->windowingSystemPreference = "SDL2";
        traits->windowDecoration = !(SDL_GetWindowFlags(window.get())&SDL_WINDOW_BORDERLESS);
        traits->screenNum = SDL_GetWindowDisplayIndex(window.get());
        // We tried to get rid of the hardcoding but failed: https://github.com/OpenMW/openmw/pull/1771
        // Here goes kcat's quote:
        // It's ultimately a chicken and egg problem, and the reason why the code is like it was in the first place.
        // It needs a context to get the current attributes, but it needs the attributes to set up the context.
        // So it just specifies the same values that were given to SDL in the hopes that it's good enough to what the window eventually gets.
        traits->red = 8;
        traits->green = 8;
        traits->blue = 8;
        traits->alpha = 0; // set to 0 to stop ScreenCaptureHandler reading the alpha channel
        traits->depth = 24;
        traits->stencil = 8;
        traits->vsync = true;
        traits->doubleBuffer = true;
        traits->inheritedWindowData = new SDLUtil::GraphicsWindowSDL2::WindowData(window.get());

        // create SDL2 instance and GraphicsContext
        graphicsWindow = new SDLUtil::GraphicsWindowSDL2(traits);
        if(!graphicsWindow->valid()) throw std::runtime_error("Failed to create GraphicsContext");

        return 0;
    }

    void configureShaders(osg::StateSet* stateSet)
    {
        (void)stateSet;
    #ifdef OSG_GLES3_AVAILABLE
        const char* vertexShaderSource = R"(#version 320 es
                precision mediump float;

                uniform mat4 osg_ModelViewProjectionMatrix;
                uniform mat3 osg_NormalMatrix;
                uniform vec3 ecLightDir;

                in vec4 osg_Vertex;
                in vec3 osg_Normal;
                out vec4 color;

                void main()
                {
                    vec3 ecNormal = normalize(osg_NormalMatrix * osg_Normal);
                    float diffuse = max(dot(ecLightDir, ecNormal), 0.0);
                    color = vec4(vec3(diffuse), 1.0);

                    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex;
                }
            )";

        const char* fragmentShaderSource = R"(#version 320 es
                precision mediump float;

                in vec4 color;
                out vec4 fragColor;

                void main()
                {
                    fragColor = color;
                }
            )";

        osg::ref_ptr<osg::Shader> vShader = new osg::Shader(osg::Shader::VERTEX, vertexShaderSource);
        osg::ref_ptr<osg::Shader> fShader = new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSource);

        osg::ref_ptr<osg::Program> program = new osg::Program;
        program->addShader(vShader);
        program->addShader(fShader);
        stateSet->setAttribute(program);

        osg::Vec3f lightDir(0.0f, 0.5f, 1.0f);
        lightDir.normalize();
        stateSet->addUniform(new osg::Uniform("ecLightDir", lightDir));
    #else
        /*
            const char* vertexShaderSource = R"(#version 300
                precision mediump float;

                uniform mat4 osg_ModelViewProjectionMatrix;
                uniform mat3 osg_NormalMatrix;
                uniform vec3 ecLightDir;

                in vec4 osg_Vertex;
                in vec3 osg_Normal;
                out vec4 color;

                void main()
                {
                    vec3 ecNormal = normalize(osg_NormalMatrix * osg_Normal);
                    float diffuse = max(dot(ecLightDir, ecNormal), 0.0);
                    color = vec4(vec3(diffuse), 1.0);

                    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex;
                }
            )";

            const char* fragmentShaderSource = R"(#version 300
                precision mediump float;

                in vec4 color;
                out vec4 fragColor;

                void main()
                {
                    fragColor = color;
                }
            )";
        */
    #endif

    }

    class ImGuiRender : public OsgImGuiHandler
    {
    public:
        ImGuiRender( const std::function<void()>& render_funs )
        {
            this->m_render_funs = render_funs;
        }

    protected:
        void drawUi() override
        {
            // ImGui code goes here...
            this->m_render_funs();
        }

    private:
        std::function<void()> m_render_funs;
    };

    class InitOperation : public osg::Operation
    {
    public:
        InitOperation( const std::function<void()>& funs, const std::string& name = "InitOperation")
            : osg::Operation(name, false),
            m_operator_funs(funs)
        {
        }

        void operator()(osg::Object* object) override
        {
            osg::GraphicsContext* context = dynamic_cast<osg::GraphicsContext*>(object);
            if (!context)
                return;

            if( m_operator_funs )
            {
                this->m_operator_funs();
            }
        }

    private:
        std::function<void()> m_operator_funs;
    };

    class ImGuiInitOperation : public osg::Operation
    {
    public:
        ImGuiInitOperation()
            : osg::Operation("ImGuiInitOperation", false)
        {
        }

        void operator()(osg::Object* object) override
        {
            osg::GraphicsContext* context = dynamic_cast<osg::GraphicsContext*>(object);
            if (!context)
                return;

            if (!ImGui_ImplOpenGL3_Init())
            {
                std::cout << "ImGui_ImplOpenGL3_Init() failed\n";
            }
        }
    };


    OsgView::~OsgView()
    {

    }

    void OsgView::initOsgView( const std::string &title, uint32_t width, uint32_t height, const std::function<void()>& render_func )
    {
        std::cout << __FUNCTION__ << std::endl;

        // create SDL window and GraphicsContext
        static std::unique_ptr<SDL_Window, SDLWindowDeleter> window;
        if(-1 == initSDLWindow(window, m_graphicsWindow, title, width, height))
            return;

        m_root = new osg::Group();

        // load .osgt for testing
        loadData();

        m_viewer.setThreadingModel(osgViewer::Viewer::ThreadingModel::CullDrawThreadPerContext);
        m_viewer.addEventHandler( new ImGuiRender(render_func) );
        m_viewer.setRealizeOperation( new ImGuiInitOperation() );

        osg::ref_ptr<osg::Camera> camera = m_viewer.getCamera();
        camera->setGraphicsContext(m_graphicsWindow.get());
        camera->setViewport(0, 0, width, height);

        m_viewer.realize();

        // optimize the scene graph, remove redundant nodes and state etc.
        osgUtil::Optimizer optimizer;
        optimizer.optimize(m_root.get(), osgUtil::Optimizer::ALL_OPTIMIZATIONS  | osgUtil::Optimizer::TESSELLATE_GEOMETRY);

        // set the scene to render
        m_viewer.setSceneData(m_root);

        // keep handle key events
        m_viewer.setKeyEventSetsDone(0);

        // set camera manipulator
        if (!m_viewer.getCameraManipulator() && m_viewer.getCamera()->getAllowEventFocus())
        {
            m_viewer.setCameraManipulator(new osgGA::TrackballManipulator());
        }

        // do not release context each frame
        m_viewer.setReleaseContextAtEndOfFrameHint(false);
    }

    void OsgView::addEventHandler(osgGA::EventHandler* eventHandler)
    {
        m_viewer.addEventHandler(eventHandler);
    }

    void OsgView::setRealizeOperation(const std::function<void()>& func, const std::string &name)
    {
        m_viewer.setRealizeOperation( new InitOperation(func, name));
    }

    void OsgView::closeImplementation()
    {
        if( m_graphicsWindow.valid() )
            m_graphicsWindow->closeImplementation();

        SDL_Quit();
        std::cout << __FUNCTION__ << std::endl;
    }

    void OsgView::loadData(const std::string &filename)
    {
        // if not loaded assume no arguments passed in, try use default mode instead.
        osg::ref_ptr<osg::Node> loadedModel = osgDB::readRefNodeFile( filename.empty() ? "cow.osgt" : filename);

        // if no model has been successfully loaded report failure.
        if (!loadedModel)
        {
            std::cout << "No data loaded" << std::endl;
            return;
        }

    #ifdef ARM_PLATFORM
        // prevent crash on arm platform
        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
        geom->setUseVertexBufferObjects(true);
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        geode->addDrawable(geom);
        m_root->addChild(geode);
    #endif

        m_root->addChild(loadedModel);
    }

}
