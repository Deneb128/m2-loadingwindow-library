#include "LoadingWindowMain.h"


//testing purposes
/*int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    loading_window_app = new App();
    loading_window_app->Run();
}*/

//testing with console
/*int main(int, char**)
{
    loading_window_app = new App();
    loading_window_app->Run();
}*/

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

