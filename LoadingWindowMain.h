#pragma once
#include "LoadingWindowHandler.h"

class App : public LoadingMainWindow
{
public:
    App() = default;
    ~App() = default;

    virtual void StartUp()
    {}
    virtual void Update() { LoadingWindowDesigner::RenderUI(m_Progress, m_LoadingText); };

};

static App* loading_window_app = nullptr;
namespace LoadingWindow {
    auto InitializeLibrary()->void;
    auto UpdateProgress(std::atomic<float> progress)->void;
    auto UpdateProgressText(std::string text)->void;
    auto CleanUpLibrary()->void;
    //auto UpdateProgressText(std::string loading_text)->void;
}

