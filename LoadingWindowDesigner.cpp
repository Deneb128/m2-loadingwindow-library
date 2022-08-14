#include "LoadingWindowDesigner.h"
#include <ImGui/imgui.h>


namespace LoadingWindowDesigner
{
    void RenderUI(float progress, std::string loading_text)
    {
        auto window_size = ImGui::GetWindowSize();

        ImGui::SetCursorPosY(window_size.y - 30);
        ImGui::Text(loading_text.c_str());
        ImGui::ProgressBar(progress, ImVec2(window_size.x - 15.0f, 5.0f), "");
    }
}
