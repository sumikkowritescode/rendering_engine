#include <SDL.h>

#include "framework/3rdparty/imgui/imgui.h"
#include "framework/3rdparty/imgui/imgui_impl_sdl_gl3.h"

#include "framework/base/time.h"

#include "framework/renderer/renderer_gl.h"
#include "framework/scene/scene.h"

// Scene settings
Framework::Camera camera(glm::vec3(100.0f, 100.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);
Framework::Renderer g_renderer;

int u_drawMode = 1;
bool u_useBloom = true;
bool u_useMotionBlur = true;
bool m_shadowDebug = false;
bool m_showMenuPanel = false;
bool quit = false;
bool g_firstInit = true;

GLfloat u_exposure = 0.032f;
GLfloat g_mouseSensitivity = 0.25f;
GLfloat u_ambience = 0.045f;
GLfloat u_motionScale = 1.0f;

// Mouse and keyboard movement variables
GLfloat m_mouseXOffset, m_mouseYOffset;
int m_mouseXPos, m_mouseYPos;
bool m_firstMouse = true;
bool m_mouseLook = true;
float g_moveSpeed = 60.0f;

const Uint8 *m_keyboardState = SDL_GetKeyboardState(nullptr);

// Function prototypes
void UpdateImgui(SDL_Window* &window, Framework::Scene &scene, SDL_GLContext &glcontext);
void GetKeyboard(const Framework::Time &time);
void ResizeWindow(GLuint width, GLuint height, SDL_Window* &window, SDL_GLContext &glcontext);
void UpdateObjects(std::vector<Framework::RenderObject> &renderObjects);

int main(int, char**)
{
    SDL_Window* window;
    SDL_GLContext glcontext;

    if (g_firstInit)
    {
        SDL_Init(SDL_INIT_VIDEO);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        camera.SetPlanes(1.0f, 750.0f);

        ResizeWindow(1024, 768, window, glcontext);

        g_firstInit = false;
    }

    glewExperimental = GL_TRUE;
    glewInit();
    ImGui_ImplSdlGL3_Init(window);

    glEnable(GL_DEPTH_TEST);
    std::cout << glGetString(GL_VERSION) << std::endl;

    // Create objects we want to render in the scene
    std::vector<Framework::RenderObject> renderObjects;
    Framework::RenderObject sponza("../resources/models/sponza/sponza.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.25f), glm::vec3(1.0f), float(0.0f));
    Framework::RenderObject teapot1("../resources/models/teapot/teapot.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f), glm::vec3(1.0f), float(-90.0f));
    Framework::RenderObject teapot2("../resources/models/teapot/teapot.obj", glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(5.0f), glm::vec3(1.0f), float(-90.0f));
    renderObjects.push_back(sponza);
    renderObjects.push_back(teapot1);
    renderObjects.push_back(teapot2);

    Framework::Time time;

    Framework::Scene scene(g_renderer);
    scene.Init();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!quit)
    {
        time.Update();

        SDL_Event event;

        if (m_mouseLook)
            SDL_ShowCursor(0);

        if (!m_mouseLook)
            SDL_ShowCursor(1);

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                quit = true;

            if (event.type == SDL_MOUSEMOTION)
            {
                if (m_mouseLook)
                {
                    m_mouseXPos = event.motion.x;
                    m_mouseYPos = event.motion.y;
                    m_mouseXOffset = (GLfloat)m_mouseXPos - (GLfloat)(g_renderer.GetScreenWidth() / 2.0f);
                    m_mouseYOffset = (GLfloat)(g_renderer.GetScreenHeight() / 2.0f) - (GLfloat)m_mouseYPos;

                    camera.ProcessMouseMovement(m_mouseXOffset, m_mouseYOffset, g_mouseSensitivity);
                    SDL_WarpMouseInWindow(window, (g_renderer.GetScreenWidth() / 2), (g_renderer.GetScreenHeight()/2)); // Lock cursor in the middle
                }
            }
        }

        UpdateImgui(window, scene, glcontext);

        GetKeyboard(time);

        UpdateObjects(renderObjects);

        scene.GeometryPass(renderObjects, camera, g_renderer);
        scene.ShadowmapPass(renderObjects);
        scene.LightingPass(u_drawMode, u_ambience, camera, g_renderer, time);
        scene.RenderLights(g_renderer);
        scene.RenderSkybox(camera);
        scene.PostProcessPass(u_useBloom, u_useMotionBlur, u_exposure, u_motionScale, camera, g_renderer);

        std::cout << glGetError() << std::endl;

        if(m_shadowDebug)
            scene.m_shadowMap.RenderDebug(scene.m_fsQuad, scene.GetShadowNearPlane(), scene.GetShadowFarPlane(), g_renderer);

        ImGui::Render();
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplSdlGL3_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

// Function declarations
void GetKeyboard(const Framework::Time &time) {
    if (m_keyboardState[SDL_SCANCODE_W])
    {
        camera.ProcessKeyboard(Framework::Camera::FORWARD, time.deltaTime * g_moveSpeed);
    }
    if (m_keyboardState[SDL_SCANCODE_S])
    {
        camera.ProcessKeyboard(Framework::Camera::BACKWARD, time.deltaTime * g_moveSpeed);
    }
    if (m_keyboardState[SDL_SCANCODE_A])
    {
        camera.ProcessKeyboard(Framework::Camera::LEFT, time.deltaTime * g_moveSpeed);
    }
    if (m_keyboardState[SDL_SCANCODE_D])
    {
        camera.ProcessKeyboard(Framework::Camera::RIGHT, time.deltaTime * g_moveSpeed);
    }
    if (m_keyboardState[SDL_SCANCODE_ESCAPE])
    {
        m_showMenuPanel = !m_showMenuPanel;
        SDL_Delay(200);
    }

    if (m_keyboardState[SDL_SCANCODE_1])
        u_drawMode = 1;
    if (m_keyboardState[SDL_SCANCODE_2])
        u_drawMode = 2;
    if (m_keyboardState[SDL_SCANCODE_3])
        u_drawMode = 3;
    if (m_keyboardState[SDL_SCANCODE_4])
        u_drawMode = 4;
    if (m_keyboardState[SDL_SCANCODE_5])
        u_drawMode = 5;
    if (m_keyboardState[SDL_SCANCODE_6])
        u_drawMode = 6;
}

void UpdateImgui(SDL_Window* &window, Framework::Scene& scene, SDL_GLContext& glcontext) {
    ImGui_ImplSdlGL3_NewFrame(window);

    if (m_showMenuPanel)
    {
        m_mouseLook = false;
    
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Settings", &m_showMenuPanel);
    
        if (ImGui::CollapsingHeader("Movement Settings"))
        {
            ImGui::SliderFloat("Move Speed", &g_moveSpeed, 1.0f, 100.0f);
            ImGui::SliderFloat("Mouse Sensitivity", &g_mouseSensitivity, 0.05f, 1.0f);
        
        }
    
        if (ImGui::CollapsingHeader("Scene Settings"))
        {
            glm::vec3 sunLightPos = scene.GetSunLightPos();
            ImGui::SliderFloat("Sunlight Pos X", &sunLightPos.x, -500.0f, 500.0f);
            ImGui::SliderFloat("Sunlight Pos Y", &sunLightPos.y, -500.0f, 500.0f);
            ImGui::SliderFloat("Sunlight Pos Z", &sunLightPos.z, -500.0f, 500.0f);
            scene.SetSunLightPos(sunLightPos);
        }
    
        if (ImGui::CollapsingHeader("SSAO Settings"))
        {
            int   kernelSize = scene.m_ssao.GetKernelSize();
            float radius = scene.m_ssao.GetRadius();
            float power = scene.m_ssao.GetPower();

            ImGui::SliderInt("SSAO kernel size", &kernelSize, 0, 256);
            ImGui::SliderFloat("SSAO kernel radius", &radius, 0.0f, 5.0f);
            ImGui::SliderFloat("SSAO occlusion power", &power, 0.0f, 5.0f);

            scene.m_ssao.SetKernelSize(kernelSize);
            scene.m_ssao.SetRadius(radius);
            scene.m_ssao.SetPower(power);

            if (ImGui::Button("Reload Shaders")) scene.m_ssao.ReloadShaders();
        }
    
        if (ImGui::CollapsingHeader("PostProcess Settings"))
        {
            ImGui::SliderFloat("Ambience", &u_ambience, 0.0f, 1.0f);
            ImGui::SliderFloat("Exposure", &u_exposure, 0.0f, 10.0f);
            ImGui::SliderFloat("Motion Blur Velocity Scale", &u_motionScale, 0.0f, 2.0f);
            if (ImGui::Button("Enable Bloom")) u_useBloom ^= 1;
            if (ImGui::Button("Enable Motion Blur")) u_useMotionBlur ^= 1;
            if (ImGui::Button("Reload Shaders")) {
                scene.m_postProcess.ReloadShaders();
                scene.ReloadLightingPass();
            }
    
        }
    
        if (ImGui::CollapsingHeader("Shadowmap Settings"))
        {
            float zNearShadow = scene.GetShadowNearPlane();
            float zFarShadow = scene.GetShadowFarPlane();

            if (ImGui::Button("Shadowmap Debug")) m_shadowDebug ^= 1;
            ImGui::SliderFloat("Shadowmap zNear", &zNearShadow, -500.0f, 500.0f);
            ImGui::SliderFloat("Shadowmap zFar", &zFarShadow, 10.0f, 2000.0f);

            scene.SetShadowNearPlane(zNearShadow);
            scene.SetShadowFarPlane(zFarShadow);

            if (ImGui::Button("Reload Shaders")) scene.m_shadowMap.ReloadShaders();
        }

        ImGui::NewLine();
        if (ImGui::Button("Quit")) quit = true;
    
        ImGui::Text("avg framerate %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    
    
    if (!m_showMenuPanel)
        m_mouseLook = true;
}

void ResizeWindow(GLuint width, GLuint height, SDL_Window* &window, SDL_GLContext &glcontext) {
    g_renderer.SetResolution(width, height);

    if (!g_firstInit)
    {
        SDL_DestroyWindow(window);
    }

    window = SDL_CreateWindow("Rendering Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, g_renderer.GetScreenWidth(), g_renderer.GetScreenHeight(), SDL_WINDOW_OPENGL);

    if (g_firstInit)
        glcontext = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, glcontext);
    g_renderer.SetViewport();
    g_renderer.SetProjectionMatrix(camera);
}

void UpdateObjects(std::vector<Framework::RenderObject> &renderObjects) {
        std::vector<Framework::RenderObject>::iterator it = renderObjects.begin();

        (it + 1)->SetPosition(glm::vec3(1.0f, (sin(SDL_GetTicks() * 0.001f * 3.0f) * 50.0f) + 100.0f, 1.0f));
        (it + 1)->SetRotation((it+1)->GetRotation() + glm::vec3(0.0f, 1.0f, 0.0f));
        (it + 1)->SetAngle((it + 1)->GetAngle() +  float(2.0f));

        (it + 2)->SetPosition(glm::vec3(1.0f, 160.0f, -40.0f));
        (it + 2)->SetRotation((it + 2)->GetRotation() + glm::vec3(1.0f, 0.0f, 1.0f));
        (it + 2)->SetAngle((it + 2)->GetAngle() + float(15.0f));
}
