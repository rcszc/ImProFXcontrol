// improfx_control_menu. RCSZ. [20231220]
// ImGui: [ChildWindow(BeginChild_EndChild)], Animation Menu, Update: 20240216.

#include "improfx_control.h"

namespace IMFXC_CWIN {
	void SmoothMenuChildWindow::DrawMenuTypeRect(float rect_height, const ImVec4& color) {
		if (MenuBufferWidthType.y > 0.1f) {
			// fill selection_box.
			IM_CONTROL_BASE::ListDrawRectangleFill(
				ImVec2(IM_CONTROL_BASE::ItemCenteredCalc(MenuBufferWidthType.y), MenuBufferTypeScroll.y),
				ImVec2(MenuBufferWidthType.y, rect_height + 2.0f),
				color
			);
			ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		}
	}

	void SmoothMenuChildWindow::DrawMenuItemRect(float rect_height, const ImVec4& color) {
		// fill item_box.
		IM_CONTROL_BASE::ListDrawRectangleFill(
			ImVec2(IM_CONTROL_BASE::ItemCenteredCalc(MenuBufferWidthItem.y), MenuBufferItemScroll.y),
			ImVec2(MenuBufferWidthItem.y, rect_height + 2.0f),
			color
		);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1.2f);
		// fill item_location_box.
		IM_CONTROL_BASE::ListDrawRectangleFill(
			ImVec2(ImGui::GetWindowSize().x - 12.0f, MenuBufferItemScroll.y),
			ImVec2(IMGUI_ITEM_SPAC, rect_height + 2.0f),
			color
		);
		// fill item_location_dir.
		IM_CONTROL_BASE::ListDrawTriangleFill(
			ImVec2(IMGUI_ITEM_SPAC * 2.0f, MenuBufferItemScroll.y + (rect_height + 2.0f) * 0.5f),
			ImVec2(-rect_height / 2.0f, -rect_height / 2.0f),
			ImVec2(-rect_height / 2.0f, rect_height / 2.0f),
			color
		);
		ImGui::PopStyleVar();
		ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
	}

	void SmoothMenuChildWindow::MenuInterCalc(ImVec2& posy_calc, ImVec2& width_calc, float speed) {
		// inter: result = y + (x − y) * t.
		posy_calc.y += (posy_calc.x - posy_calc.y) * 0.072f * speed;
		width_calc.y += (width_calc.x - width_calc.y) * 0.072f * speed;
	}

	bool SmoothMenuChildWindow::DrawMenuWindow(
		const char*                     name,
		const std::vector<std::string>& items,
		uint32_t&                       count,
		const ImVec4&                   color,
		const ImVec2&                   size,
		float                           speed,
		float                           text_scale
	) {
		ImGui::BeginChild(name, size);
		IM_CONTROL_BASE::ListDrawRectangleFill(ImVec2(), size, IM_CONTROL_BASE::ColorBrightnesScale(color, 0.65f));
		ImGui::SetWindowFontScale(text_scale);

		if (ImGui::IsWindowHovered())
			DrawMenuTypeRect(TextDrawHeight, IM_CONTROL_BASE::ColorBrightnesScale(color, 0.32f));
		DrawMenuItemRect(TextDrawHeight, IM_CONTROL_BASE::ColorBrightnesScale(color, 0.42f));

		bool ReturnTypeFlag = false;
		// draw menu_items.
		for (size_t i = 0; i < items.size(); ++i) {

			ImVec2 ItemTextSize = ImGui::CalcTextSize(items[i].c_str());
			float DrawHeightPosition = ImGui::GetCursorPosY();

			ImGui::SetCursorPosX(IM_CONTROL_BASE::ItemCenteredCalc(ItemTextSize.x));
			ImGui::Text(items[i].c_str());

			if (ImGui::IsItemHovered()) {
				MenuBufferTypeScroll.x = DrawHeightPosition - ImGui::GetScrollY();
				MenuBufferWidthType.x = ItemTextSize.x + IMGUI_ITEM_SPAC * 2.0f;

				if (ImGui::GetMouseClickedCount(0)) {
					count = (uint32_t)i;
					ReturnTypeFlag = true;
				}
			}
			if (count == (uint32_t)i) {
				MenuBufferItemScroll.x = DrawHeightPosition - ImGui::GetScrollY();
				MenuBufferWidthItem.x = ItemTextSize.x + IMGUI_ITEM_SPAC * 2.0f;
			}
			TextDrawHeight = ItemTextSize.y;
		}
		MenuInterCalc(MenuBufferTypeScroll, MenuBufferWidthType, speed);
		MenuInterCalc(MenuBufferItemScroll, MenuBufferWidthItem, speed);

		ImGui::PopStyleColor();
		ImGui::EndChild();

		return ReturnTypeFlag;
	}
}