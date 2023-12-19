// improfx_control_base. RCSZ.

#ifndef _IMPROFX_CONTROL_BASE_HPP
#define _IMPROFX_CONTROL_BASE_HPP
#include <vector>
#include <cstdio>

#include "imgui_glfw/imgui.h"
#include "imgui_glfw/imgui_internal.h"

#define IMGUI_ITEM_SPC ImGui::GetStyle().ItemSpacing.x
namespace ImExtBaseControl {

	inline ImVec4 ExtSubColorGrayscale(const ImVec4& color, float value) {
		return ImVec4(
			color.x - value < 0.0f ? 0.0f : color.x - value,
			color.y - value < 0.0f ? 0.0f : color.y - value,
			color.z - value < 0.0f ? 0.0f : color.z - value,
			color.w - value < 0.0f ? 0.0f : color.w - value
		);
	}

	inline float ExtItemCenteredCalc(const float& width) {
		return ImGui::GetWindowSize().x / 2.0f - width / 2.0f - IMGUI_ITEM_SPC;
	}

	inline void ExtDrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, float linewidth) {
		// draw line_rectangle.
		ImGui::GetWindowDrawList()->AddRect(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
			IM_COL32(ImU32(color.x * 255.0f), ImU32(color.y * 255.0f), ImU32(color.z * 255.0f), ImU32(color.w * 255.0f)),
			ImGui::GetStyle().FrameRounding,
			NULL,
			linewidth
		);
	}

	inline void ExtDrawRectangleFill(const ImVec2& position, const ImVec2& size, const ImVec4& color) {
		// draw fill_rectangle.
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
			IM_COL32(ImU32(color.x * 255.0f), ImU32(color.y * 255.0f), ImU32(color.z * 255.0f), ImU32(color.w * 255.0f)),
			ImGui::GetStyle().FrameRounding,
			NULL
		);
	}

	inline void ExtDrawCircleFill(const ImVec2& position, float size, const ImVec4& color) {
		// draw fill_circle.
		ImGui::GetWindowDrawList()->AddCircleFilled(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			size,
			IM_COL32(ImU32(color.x * 255.0f), ImU32(color.y * 255.0f), ImU32(color.z * 255.0f), ImU32(color.w * 255.0f))
		);
	}

	inline void ExtDrawLine(const ImVec2& point0, const ImVec2& point1, const ImVec4& color, float linewidth) {
		// draw line_segment.
		ImGui::GetWindowDrawList()->AddLine(
			ImVec2(ImGui::GetWindowPos().x + point0.x, ImGui::GetWindowPos().y + point0.y),
			ImVec2(ImGui::GetWindowPos().x + point1.x, ImGui::GetWindowPos().y + point1.y),
			IM_COL32(ImU32(color.x * 255.0f), ImU32(color.y * 255.0f), ImU32(color.z * 255.0f), ImU32(color.w * 255.0f)),
			linewidth
		);
	}

	inline void ExtDrawText(const ImVec2& position, const ImVec4& color, const char* text, ...) {
		char LoggerStrTemp[1024] = {};

		va_list ParamArgs;
		va_start(ParamArgs, text);
		vsnprintf(LoggerStrTemp, 1024, text, ParamArgs);
		va_end(ParamArgs);

		ImGui::GetWindowDrawList()->AddText(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			IM_COL32(ImU32(color.x * 255.0f), ImU32(color.y * 255.0f), ImU32(color.z * 255.0f), ImU32(color.w * 255.0f)),
			LoggerStrTemp
		);
	}
}

#endif