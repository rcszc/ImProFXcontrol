// improfx_control_animmenu. RCSZ.
// ImGui: [ChildWindow(BeginChild_EndChild)], Animation Menu, Update: 20231220.

#include "improfx_control.h"

void AnimMenuChildWindow::DrawMenuTypeRect(float rect_height, const ImVec4& color) {
	// fill selection_box.
	ImControlBase::ExtDrawRectangleFill(
		ImVec2(ImControlBase::ExtItemCenteredCalc(AnimBufferWidthType.y), AnimBufferPosyType.y),
		ImVec2(AnimBufferWidthType.y, rect_height + 2.0f),
		color
	);
	ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
}

void AnimMenuChildWindow::DrawMenuItemRect(float rect_height, const ImVec4& color) {
	// fill item_box.
	ImControlBase::ExtDrawRectangleFill(
		ImVec2(ImControlBase::ExtItemCenteredCalc(AnimBufferWidthItem.y), AnimBufferPosyItem.y),
		ImVec2(AnimBufferWidthItem.y, rect_height + 2.0f),
		color
	);
	// fill item_location_box.
	ImControlBase::ExtDrawRectangleFill(
		ImVec2(ImGui::GetWindowSize().x - 12.0f, AnimBufferPosyItem.y), 
		ImVec2(8.0f, rect_height + 2.0f), 
		color
	);
	ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
}

void AnimMenuChildWindow::MenuInterCalc(ImVec2& posy_calc, ImVec2& width_calc, float speed) {
	// inter: result = y + (x − y) * t.
	posy_calc.y += (posy_calc.x - posy_calc.y) * 0.072f * speed;
	width_calc.y += (width_calc.x - width_calc.y) * 0.072f * speed;
}

void AnimMenuChildWindow::DrawMenuWindow(
	const char*                     name,
	const std::vector<std::string>& items,
	uint32_t&                       count,
	const ImVec4&                   color,
	const ImVec2&                   size,
	float                           speed,
	float                           textscale
) {
	ImGui::BeginChild(name, size, true);
	ImGui::SetWindowFontScale(textscale);

	ImVec4 ImGuiTextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
	DrawMenuTypeRect(TextDrawHeight, color);
	DrawMenuItemRect(TextDrawHeight, ImControlBase::ExtSubColorGrayscale(ImGuiTextColor, 0.25f));

	// draw menu_items.
	for (size_t i = 0; i < items.size(); ++i) {

		ImVec2 ItemTextSize = ImGui::CalcTextSize(items[i].c_str());
		float DrawHeightPosition = ImGui::GetCursorPosY();

		ImGui::SetCursorPosX(ImControlBase::ExtItemCenteredCalc(ItemTextSize.x));
		ImGui::Text(items[i].c_str());

		if (ImGui::IsItemHovered()) {
			AnimBufferPosyType.x = DrawHeightPosition - ImGui::GetScrollY();
			AnimBufferWidthType.x = ItemTextSize.x + IMGUI_ITEM_SPC * 2.0f;

			if (ImGui::GetMouseClickedCount(0))
				count = (uint32_t)i;
		}
		if (count == (uint32_t)i) {
			AnimBufferPosyItem.x = DrawHeightPosition - ImGui::GetScrollY();
			AnimBufferWidthItem.x = ItemTextSize.x + IMGUI_ITEM_SPC * 2.0f;
		}
		TextDrawHeight = ItemTextSize.y;
	}
	MenuInterCalc(AnimBufferPosyType, AnimBufferWidthType, speed);
	MenuInterCalc(AnimBufferPosyItem, AnimBufferWidthItem, speed);

	ImGui::EndChild();
}