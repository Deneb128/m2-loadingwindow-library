#include "LoadingWindowMain.h"


#if defined(TEST_PROGRESS_BAR) && !defined(_DEBUG)
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    loading_window_app = new App();
    loading_window_app->Run();
}
#endif

#if defined(TEST_PROGRESS_BAR) && defined(_DEBUG)
int main(int, char**)
{
    loading_window_app = new App();
    loading_window_app->Run();
}
#endif

namespace LoadingWindow {
    void InitializeLibrary()
    {
        loading_window_app = new App();
        loading_window_app->Run();
    }

    void CleanUpLibrary()
    {
        if (loading_window_app)
            loading_window_app->SetDone();
    }

    void UpdateProgress(std::atomic<float> progress)
    {
        if (loading_window_app)
            loading_window_app->SetProgress(progress);
    }

    void UpdateProgressText(std::string text)
    {
        if (loading_window_app)
            loading_window_app->SetProgressText(text);
    }
}

