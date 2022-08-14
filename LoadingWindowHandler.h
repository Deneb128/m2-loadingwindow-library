#pragma once
#include "LoadingWindowDesigner.h"
#include <ImGui\imgui.h>
#include <ImGui\backend\imgui_impl_win32.h>
#include <ImGui\backend\imgui_impl_DX8.h>
#include <DirectX-8.1\d3d8.h>
#include <tchar.h>
#include <memory>
#include <atomic>
#include <string>
#pragma comment(lib, "d3d8.lib")
class LoadingMainWindow
{
public:
    LoadingMainWindow();
    virtual ~LoadingMainWindow();


    void Run();
    void Close();
    void SetProgress(float progress) { m_Progress = progress; }
    void SetDone() { m_Done = true;}
    void SetProgressText(std::string text) { m_LoadingText = text; }
    virtual void Update() = 0;
    virtual void StartUp() = 0;

private:
    WNDCLASSEX m_WCEX;
    HWND m_HWND;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool m_Done = false;
    int splashImageWidth = 0;
    int splashImageHeight = 0;
protected:
    float m_Progress = 0.0f;
    std::string m_LoadingText = "Loading...";
};
