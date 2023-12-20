// improfx_control_base. RCSZ.

#ifndef _IMPROFX_CONTROL_BASE_HPP
#define _IMPROFX_CONTROL_BASE_HPP
#include <string>
#include <vector>
#include <cstdio>

#include "imgui_glfw/imgui.h"
#include "imgui_glfw/imgui_internal.h"

#define IMGUI_ITEM_SPC ImGui::GetStyle().ItemSpacing.x

#define IMVEC2_ADD2(p1, p2) ImVec2((p1).x + (p2).x, (p1).y + (p2).y)
#define IMVEC2_SUB2(p1, p2) ImVec2((p1).x - (p2).x, (p1).y - (p2).y)
#define IMVEC2_MUL1(p, v)   ImVec2((p).x * (v), (p).y * (v))
#define IMVEC2_MUL2(p1, p2) ImVec2((p1).x * (p2).x, (p1).y * (p2).y)

namespace ImControlBase {

	ImVec4 ExtSubColorGrayscale(const ImVec4& color, float value);
	float ExtItemCenteredCalc(const float& width);

	void ExtDrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, float linewidth);
	void ExtDrawRectangleFill(const ImVec2& position, const ImVec2& size, const ImVec4& color);
	void ExtDrawCircleFill(const ImVec2& position, float size, const ImVec4& color);
	void ExtDrawLine(const ImVec2& point0, const ImVec2& point1, const ImVec4& color, float linewidth);

	void ExtDrawText(const ImVec2& position, const ImVec4& color, const char* text, ...);
}

#endif