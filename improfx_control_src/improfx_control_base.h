// improfx_control_base. RCSZ.

#ifndef _IMPROFX_CONTROL_BASE_HPP
#define _IMPROFX_CONTROL_BASE_HPP
#include <functional>
#include <string>
#include <vector>
#include <deque>
#include <cstdio>

#include "imgui_glfw/imgui.h"
#include "imgui_glfw/imgui_internal.h"

#define IMGUI_ITEM_SPC ImGui::GetStyle().ItemSpacing.x
namespace ImControlBase {

	ImVec4 ExtSubColorGrayscale(const ImVec4& color, float value);
	float ExtItemCenteredCalc(const float& width);

	void ExtDrawLine(const ImVec2& point0, const ImVec2& point1, const ImVec4& color, float linewidth);
	void ExtDrawText(const ImVec2& position, const ImVec4& color, const char* text, ...);

	void ExtDrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, float linewidth);
	void ExtDrawRectangleFill(const ImVec2& position, const ImVec2& size, const ImVec4& color);
	void ExtDrawCircleFill(const ImVec2& position, float size, const ImVec4& color);
	void ExtDrawTriangleFill(const ImVec2& position, const ImVec2& offset1, const ImVec2& offset2, const ImVec4& color);
}

#endif