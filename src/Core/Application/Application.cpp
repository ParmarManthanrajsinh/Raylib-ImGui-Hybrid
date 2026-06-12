#include "Application.h"
#include "Core/Logging/Log.h"
#include "Core/Input/Input.h" // IWYU pragma: keep

// --- Swap GLAD for standard WebGL headers on the Web ---
#ifdef CORE_PLATFORM_WEB
    #include <GLES3/gl3.h>
    #include <emscripten.h>
#else
    #include <glad/glad.h>
#endif

#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "ApplicationLayout.h"
#include "ApplicationTheme.h"

extern "C" 
{
    #include "rlgl.h"
}

namespace Core 
{
    FApplication* FApplication::s_Instance = nullptr;

    // --- Web Asynchronous Loop Anchor ---
    #ifdef CORE_PLATFORM_WEB
    void EmscriptenLoopCallback(void* Arg)
    {
        FApplication* App = static_cast<FApplication*>(Arg);
        App->WebTick();
    }
    #endif

    FApplication::FApplication(const FApplicationConfig& InConfig)
        : Name(InConfig.Name), 
          Config(InConfig),
          Width(InConfig.Width), 
          Height(InConfig.Height),
          WindowHandle(nullptr), 
          bIsRunning(false), 
          bRenderLoopFinished(false)
    {
        CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;
    }

    FApplication::~FApplication()
    {
        if (RenderThread.joinable())
        {
            RenderThread.join();
        }
    }

    void FApplication::PushLayer(FLayer* InLayer)
    {
        LayerStack.PushLayer(InLayer);
    }

    void FApplication::PushOverlay(FLayer* InOverlay)
    {
        LayerStack.PushOverlay(InOverlay);
    }

    void FApplication::OnEvent(FEvent& InEvent)
    {
        FEventDispatcher Dispatcher(InEvent);
        Dispatcher.Dispatch<FWindowCloseEvent>(CORE_BIND_EVENT_FN(FApplication::OnWindowClose));
        Dispatcher.Dispatch<FWindowResizeEvent>(CORE_BIND_EVENT_FN(FApplication::OnWindowResize));

        for (auto It = LayerStack.rbegin(); It != LayerStack.rend(); ++It)
        {
            if (InEvent.bHandled) 
                break;
            (*It)->OnEvent(InEvent);
        }
    }

    bool FApplication::OnWindowClose(FWindowCloseEvent&)
    {
        bIsRunning = false;
        return true;
    }

    bool FApplication::OnWindowResize(FWindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            return false;
        }

        SetSize(e.GetWidth(), e.GetHeight());
        return false;
    }

    void FApplication::FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height)
    {
        FApplication* App = (FApplication*)glfwGetWindowUserPointer(Window);
        if (App)
        {
            FWindowResizeEvent Event(Width, Height);
            App->OnEvent(Event);
        }
    }

    void FApplication::WindowCloseCallback(GLFWwindow* Window)
    {
        FApplication* App = (FApplication*)glfwGetWindowUserPointer(Window);
        if (App)
        {
            FWindowCloseEvent Event;
            App->OnEvent(Event);
        }
    }

    void FApplication::Run()
    {
        if (!glfwInit())
        {
            FLog::CoreError("Failed to init GLFW");
            return;
        }

        // --- FIX: Downgrade Context Version Profile for WebGL ---
        #ifdef CORE_PLATFORM_WEB
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        #else
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #endif

    #if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        WindowHandle = glfwCreateWindow(Width, Height, Name.c_str(), nullptr, nullptr);
        if (!WindowHandle)
        {
            FLog::CoreError("Failed to create GLFW window");
            glfwTerminate();
            return;
        }

        glfwSetWindowUserPointer(WindowHandle, this);
        glfwSetFramebufferSizeCallback(WindowHandle, FramebufferSizeCallback);
        glfwSetWindowCloseCallback(WindowHandle, WindowCloseCallback);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        SetApplicationTheme();
        LoadApplicationDefaultIni();

        ImGui_ImplGlfw_InitForOpenGL(WindowHandle, true);

        // --- Separate Paths for Web vs Desktop ---
        #ifdef CORE_PLATFORM_WEB
            // Web lacks secondary graphics threads. Execute everything inline.
            ImGui_ImplOpenGL3_Init("#version 100");
            rlLoadExtensions((void*)glfwGetProcAddress);
            rlglInit(Width, Height);
            OnStart();
            PreviousTime = glfwGetTime();
            bIsRunning = true;

            // Hand loop management straight to browser framework
            emscripten_set_main_loop_arg(EmscriptenLoopCallback, this, 0, 1);
        #else
            // Standard Multi-threaded Desktop Handoff Pipeline
            glfwMakeContextCurrent(nullptr);
            bIsRunning = true;
            RenderThread = std::thread(&FApplication::RenderLoop, this);

            while (bIsRunning)
            {
                glfwWaitEvents();
                if (glfwWindowShouldClose(WindowHandle)) 
                {
                    bIsRunning = false;
                }
            }

            while (!bRenderLoopFinished)
            {
                glfwWaitEventsTimeout(0.005);
            }

            if (RenderThread.joinable())
            {
                RenderThread.join();
            }

            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            glfwDestroyWindow(WindowHandle);
            glfwTerminate();
        #endif
    }
    
    // --- The Core Web Tick Execution Target ---
    #ifdef CORE_PLATFORM_WEB
    void FApplication::WebTick()
    {
        glfwPollEvents();

        double CurrentTime = glfwGetTime();
        float DeltaSeconds = static_cast<float>(CurrentTime - PreviousTime);
        PreviousTime = CurrentTime;

        for (FLayer* Layer : LayerStack)
            Layer->OnUpdate(DeltaSeconds);

        OnUpdate(DeltaSeconds);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (FLayer* Layer : LayerStack)
            Layer->OnUIRender();

        OnUIRender();
        ImGui::Render();

        glViewport(0, 0, Width, Height);
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (FLayer* Layer : LayerStack)
            Layer->OnUIRender();
            
        OnUIRender();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(WindowHandle);

        if (!bIsRunning || glfwWindowShouldClose(WindowHandle))
        {
            OnShutdown();
            rlglClose();
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            glfwDestroyWindow(WindowHandle);
            glfwTerminate();
            emscripten_cancel_main_loop();
        }
    }
    #endif

    // This runs on the SECONDARY THREAD (Desktop Only)
    void FApplication::RenderLoop()
    {
        glfwMakeContextCurrent(WindowHandle);
        glfwSwapInterval(1); 

        rlLoadExtensions((void*)glfwGetProcAddress);
        ImGuiIO& IO = ImGui::GetIO();
        ImGui_ImplOpenGL3_Init("#version 330");
        rlglInit(Width, Height);

        OnStart();
        PreviousTime = glfwGetTime();

        while (bIsRunning)
        {
            double CurrentTime = glfwGetTime();
            float DeltaSeconds = static_cast<float>(CurrentTime - PreviousTime);
            PreviousTime = CurrentTime;

            int CurrentW = Width;
            int CurrentH = Height;

            for (FLayer* Layer : LayerStack)
                Layer->OnUpdate(DeltaSeconds);

            OnUpdate(DeltaSeconds);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            for (FLayer* Layer : LayerStack)
                Layer->OnUIRender();

            OnUIRender();
            ImGui::Render();

            glViewport(0, 0, CurrentW, CurrentH);
            glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* BackupCurrentContext = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(BackupCurrentContext);
            }

            glfwSwapBuffers(WindowHandle);
        }

        OnShutdown();
        rlglClose();
        ImGui_ImplOpenGL3_Shutdown();
        bRenderLoopFinished = true;
    }
}
