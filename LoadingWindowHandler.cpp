#include "LoadingWindowHandler.h"
#include <string>

static LPDIRECT3D8              g_pD3D = NULL;
static LPDIRECT3DDEVICE8        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};
static LPDIRECT3DTEXTURE8 SplashImage = NULL;
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate8(D3D_SDK_VERSION)) == NULL)
        return false;

    D3DDISPLAYMODE d3ddm;
    if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
        return false;

    // Set up the structure used to create the D3DDevice. Most parameters are
    // zeroed out. We set Windowed to TRUE, since we want to do D3D in a
    // window, and then set the SwapEffect to "discard", which is the most
    // efficient method of presenting the back buffer to the display.  And 
    // we request a back buffer format that matches the current desktop display 
    // format.
    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = d3ddm.Format;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync

    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX8_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX8_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_PAINT:
        ::ValidateRect(hWnd, NULL);
        return 0;
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

//TEXTURE LOAD
#include <DirectX-8.1\D3dx8tex.h>
#ifdef _DEBUG
#pragma comment(lib, "d3dx8d.lib")
#else
#pragma comment(lib, "d3dx8.lib")
#endif
#include <filesystem>
bool LoadTextureFromFile(LPDIRECT3DTEXTURE8* out_texture, int* out_width, int* out_height)
{
    // Load texture from disk
    LPDIRECT3DTEXTURE8 texture = NULL;

    std::filesystem::path cwd = std::filesystem::current_path();
    std::string filepath = cwd.string() + "\\movie\\splash.bmp";

    HRESULT hr = D3DXCreateTextureFromFileA(g_pd3dDevice, filepath.c_str(), &texture);
    if (hr != S_OK) {
        return false;
    }

    // Retrieve description of the texture surface so we can access its size
    D3DSURFACE_DESC my_image_desc;
    texture->GetLevelDesc(0, &my_image_desc);
    *out_texture = texture;
    *out_width = (int)my_image_desc.Width;
    *out_height = (int)my_image_desc.Height;
    return true;
}

//TEXTURE LOAD END

void _GetClientRect(HWND hWnd, RECT* prc)
{
    ::GetClientRect(hWnd, prc);
}

int	GetScreenWidth()
{
    return GetSystemMetrics(SM_CXSCREEN);
}

int	GetScreenHeight()
{
    return GetSystemMetrics(SM_CYSCREEN);
}

LoadingMainWindow::LoadingMainWindow()
{
    m_WCEX = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&m_WCEX);
    m_HWND = ::CreateWindow(m_WCEX.lpszClassName, _T("Dear ImGui DirectX8 Example"), WS_OVERLAPPED, 100, 100, 1280, 800, NULL, NULL, m_WCEX.hInstance, NULL);
    // Initialize Direct3D
    if (!CreateDeviceD3D(m_HWND))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(m_WCEX.lpszClassName, m_WCEX.hInstance);
        Close();
    }
    // Show the window
#ifdef OPEN_CENTERED
    RECT rc;
    _GetClientRect(m_HWND, &rc);
    int windowWidth = rc.right - rc.left;
    int windowHeight = rc.bottom - rc.top;
    SetWindowPos(m_HWND, HWND_TOPMOST, (GetScreenWidth() - windowWidth) / 2, (GetScreenHeight() - windowHeight) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
#endif
    ::ShowWindow(m_HWND, SW_SHOWDEFAULT);
    ::UpdateWindow(m_HWND);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_HWND);
    ImGui_ImplDX8_Init(g_pd3dDevice);


    bool ret = LoadTextureFromFile(&SplashImage, &splashImageWidth, &splashImageHeight);

#ifdef _DEBUG
    IM_ASSERT(ret != false);
#endif
}

LoadingMainWindow::~LoadingMainWindow()
{
    Close();
}

void LoadingMainWindow::Close()
{
    ImGui_ImplDX8_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(m_HWND);
    ::UnregisterClass(m_WCEX.lpszClassName, m_WCEX.hInstance);
}

void LoadingMainWindow::Run()
{
    while (!m_Done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                m_Done = true;
        }
        if (m_Done)
            break;
#ifndef TEST_PROGRESS_BAR
        if (m_Progress >= 1.0f)
            m_Done = true;
#endif
        // Start the Dear ImGui frame
        ImGui_ImplDX8_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        //APP_BEGIN

        //START WINDOW
        ImGuiWindowFlags window_flags = 0;
#ifndef TEST_PROGRESS_BAR
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
#endif
#ifdef LOADING_WINDOW_NOBORDERS
        window_flags |= ImGuiTableFlags_Borders;
#endif
        ImGui::Begin("TopWindow", nullptr, window_flags);
        //SHOW BACKGROUND
        ImGui::SetCursorPosY(0);
        auto size = ImGui::GetContentRegionAvail();
        ImGui::Image((void*)SplashImage, ImVec2(size.x, size.y));
#ifdef TEST_PROGRESS_BAR
        static float progress_dir = 1.0f;
        m_Progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
        if (m_Progress >= +1.1f) { m_Progress = +1.1f; progress_dir *= -1.0f; }
        if (m_Progress <= -0.1f) { m_Progress = -0.1f; progress_dir *= -1.0f; }
#endif
        //UPDATE PROGRESS BAR
        Update();

        //END WINDOW
        ImGui::End();
        //APP_END
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        //g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX8_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();

    }
    Close();
}

