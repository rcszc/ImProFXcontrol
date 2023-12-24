// improfx_control_coordeditor. RCSZ. [20231224]
// ImGui: [ChildWindow(BeginChild_EndChild)], Animation Groups Editor, Update: 20231225.

#include "improfx_control.h"

#define IMVEC2_ADD2(p1, p2) ImVec2((p1).x + (p2).x, (p1).y + (p2).y)
#define IMVEC2_SUB2(p1, p2) ImVec2((p1).x - (p2).x, (p1).y - (p2).y)
#define LIMIT_CLAMP(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

void CoordSystemEditorWindow::DrawCoordinateRulerX(const ImVec2& limit, const ImVec4& color, float length, float center, float scale, float ruler) {
	const float RulerFixedValue = 5.0f;
	const float RulerSmallScale = ruler / RulerFixedValue;

	for (float i = limit.x * scale + center; i < limit.y * scale + center + 0.1f; i += ruler * scale) {
		// ruler main scale.
		ImControlBase::ExtDrawLine(ImVec2(i, 0.0f), ImVec2(i, length), color, 1.8f);
		if (scale > 0.72f)
			for (float j = 0.0f; j < RulerSmallScale; j += 1.0f)
				ImControlBase::ExtDrawLine(ImVec2(i + RulerFixedValue * scale * j, 0.0f), ImVec2(i + RulerFixedValue * scale * j, length * 0.58f), color, 1.2f);
	}
}

void CoordSystemEditorWindow::DrawCoordinateRulerY(const ImVec2& limit, const ImVec4& color, float length, float center, float scale, float ruler) {
	const float RulerFixedValue = 5.0f;
	const float RulerSmallScale = ruler / RulerFixedValue;

	for (float i = limit.x * scale + center; i < limit.y * scale + center + 0.1f; i += ruler * scale) {
		// ruler main scale.
		ImControlBase::ExtDrawLine(ImVec2(0.0f, i), ImVec2(length, i), color, 1.8f);
		if (scale > 0.72f)
			for (float j = 0.0f; j < RulerSmallScale; j += 1.0f)
				ImControlBase::ExtDrawLine(ImVec2(0.0f, i + RulerFixedValue * scale * j), ImVec2(length * 0.58f, i + RulerFixedValue * scale * j), color, 1.2f);
	}
}

void CoordSystemEditorWindow::DrawCoordinateLines(const ImVec4& color, const ImVec2& mouse, const ImVec2& win_size) {
	ImControlBase::ExtDrawLine(ImVec2(mouse.x, 0.0f), ImVec2(mouse.x, mouse.y), color, 1.2f);
	ImControlBase::ExtDrawLine(ImVec2(0.0f, mouse.y), ImVec2(mouse.x, mouse.y), color, 1.2f);
}

void CoordSystemEditorWindow::DrawGrid(
	const ImVec2& limitx, const ImVec2& limity, const ImVec4& color, const ImVec2& pos, const ImVec2& win_size,
	float scale, float ruler
) {
	ImVec2 DrawPosition(pos.x + win_size.x * 0.5f, pos.y + win_size.y * 0.5f);

	for (float i = limitx.x * scale + DrawPosition.x; i < limitx.y * scale + DrawPosition.x + 0.1f; i += ruler * scale)
		ImControlBase::ExtDrawLine(ImVec2(i, limity.x * scale + DrawPosition.y), ImVec2(i, limity.y * scale + DrawPosition.y), color, 1.8f);
	for (float i = limity.x * scale + DrawPosition.y; i < limity.y * scale + DrawPosition.y + 0.1f; i += ruler * scale)
		ImControlBase::ExtDrawLine(ImVec2(limitx.x * scale + DrawPosition.x, i), ImVec2(limitx.y * scale + DrawPosition.x, i), color, 1.8f);

	ImControlBase::ExtDrawCircleFill(DrawPosition, 3.2f, ImControlBase::ExtSubColorGrayscale(color, -0.16f));
}

#define IMEXT_DISTANCE(pos1, pos2) std::sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y))

void CoordSystemEditorWindow::PositioningWindow(
	ImVec2& index, const ImVec2& limitx, const ImVec2& limity, const ImVec4& color, ImTextureID texture,
	float size, float scale
) {
	const float CircleSize = 20.0f;

	ImVec2 WindowCenter(size * 0.5f, size * 0.5f);
	ImVec2 LimitMin(0.0f - WindowCenter.x, 0.0f - WindowCenter.y);
	// LimitMax = WindowCenter;

	ImGui::BeginChild("@POSWIN", ImVec2(size, size));
	{
		PosWinCurrPosition.x = size * (limitx.y * scale - index.x) / (limitx.y * scale - limitx.x * scale) - WindowCenter.x;
		PosWinCurrPosition.y = size * (limity.y * scale - index.y) / (limity.y * scale - limity.x * scale) - WindowCenter.y;

		ImVec2 WindowMousePos = IMVEC2_SUB2(IMVEC2_SUB2(ImGui::GetMousePos(), ImGui::GetWindowPos()), WindowCenter);   // 鼠标相对子窗口中心坐标.
		ImVec2 Proportion((limitx.y * scale - limitx.x * scale) / size, (limity.y * scale - limity.x * scale) / size); // 映射到网格窗口坐标缩放比例.

		ImControlBase::ExtDrawRectangleFill(ImVec2(0.0f, 0.0f), ImVec2(size, size), ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.64f));
		if (texture)
			ImGui::Image(texture, ImVec2(size, size));

		ImControlBase::ExtDrawLine(ImVec2(WindowCenter.x, 0.0f), ImVec2(WindowCenter.x, size), ImControlBase::ExtSubColorGrayscale(color, 0.32f), 2.0f);
		ImControlBase::ExtDrawLine(ImVec2(0.0f, WindowCenter.y), ImVec2(size, WindowCenter.y), ImControlBase::ExtSubColorGrayscale(color, 0.32f), 2.0f);
		ImControlBase::ExtDrawCircleFill(IMVEC2_ADD2(PosWinCurrPosition, WindowCenter), CircleSize, ImVec4(color.x, color.y, color.z, 0.48f));
		
		if (ImGui::IsMouseDown(0) && IMEXT_DISTANCE(WindowMousePos, PosWinCurrPosition) < CircleSize && PosWinCurrFocus) {
			PosWinCurrFocus = true;

			index.x -= ImGui::GetIO().MouseDelta.x * Proportion.x;
			index.y -= ImGui::GetIO().MouseDelta.y * Proportion.y;
		}
		else
			PosWinCurrFocus = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

		PosWinCurrPosition.x = LIMIT_CLAMP(PosWinCurrPosition.x, LimitMin.x, WindowCenter.x);
		PosWinCurrPosition.y = LIMIT_CLAMP(PosWinCurrPosition.y, LimitMin.y, WindowCenter.y);
	}
	ImGui::EndChild();
}

void CoordSystemEditorWindow::DrawEditorWindow(
	uint32_t                             unqiue_id,
	const char*                          name,
	std::function<void(CoordSystemInfo)> draw,
	ImTextureID                          poswin_image,
	bool                                 fixed_window,
	float                                speed,
	const ImVec2&                        coord_size,
	const ImVec2&						 coord_winsize,
	bool*                                p_open,
	ImGuiWindowFlags                     flags
) {
	ImVec2 CoordXLimit(-coord_size.x * 0.5f, coord_size.x * 0.5f);
	ImVec2 CoordYLimit(-coord_size.y * 0.5f, coord_size.y * 0.5f);

	ImGuiWindowFlags WindowFlags = 0;
	if (fixed_window)
		WindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
	ImGui::PushID(unqiue_id);

	// frame_background color.
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.58f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.58f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.58f));
	// slider_block color.
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.16f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.16f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.2f));
	// button color.
	ImGui::PushStyleColor(ImGuiCol_Button, ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.58f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.2f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorColorSystem);

	ImGui::Begin(name, p_open, WindowFlags | flags);
	{
		const float CoordRulerWin = 20.0f;
		const ImVec4 RulerCursorColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
		
		ImVec2 GridScaleLimit = {};
		if (CoordXLimit.y - CoordXLimit.x >= CoordYLimit.y - CoordYLimit.x)
			GridScaleLimit = ImVec2(coord_winsize.x / (CoordXLimit.y - CoordXLimit.x), (CoordXLimit.y - CoordXLimit.x) / coord_winsize.x);
		else
			GridScaleLimit = ImVec2(coord_winsize.x / (CoordYLimit.y - CoordYLimit.x), (CoordYLimit.y - CoordYLimit.x) / coord_winsize.x);
		ImVec2 MousePosition = {};

		ImVec2 GridWindowPos = ImVec2(CoordRulerWin + IMGUI_ITEM_SPC * 1.5f, CoordRulerWin + IMGUI_ITEM_SPC * 4.75f);
		// grid window scale.
		if (ImGui::IsMouseHoveringRect(
			ImVec2(IMVEC2_ADD2(ImGui::GetWindowPos(), GridWindowPos)),
			ImVec2(IMVEC2_ADD2(ImGui::GetWindowPos(), IMVEC2_ADD2(GridWindowPos, coord_winsize)))
		))
			GridSizeScale.x += ImGui::GetIO().MouseWheel * 0.12f * GridSizeScale.x;

		GridSizeScale.x = LIMIT_CLAMP(GridSizeScale.x, GridScaleLimit.x, GridScaleLimit.y);
		GridSizeScale.y += (GridSizeScale.x - GridSizeScale.y) * 0.1f * speed;

		ImVec2 ViewLimit0(CoordXLimit.x * GridSizeScale.y, CoordXLimit.y * GridSizeScale.y);
		ImVec2 ViewLimit1(CoordYLimit.x * GridSizeScale.y, CoordYLimit.y * GridSizeScale.y);

		// mouse move coord.
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && GridWinFocus &&
			ImGui::IsMouseHoveringRect(
				ImVec2(IMVEC2_ADD2(ImGui::GetWindowPos(), GridWindowPos)),
				ImVec2(IMVEC2_ADD2(ImGui::GetWindowPos(), IMVEC2_ADD2(GridWindowPos, coord_winsize)))
			)) {
			GridWinFocus = true;

			GridCenterPosition.x += ImGui::GetIO().MouseDelta.x;
			GridCenterPosition.y += ImGui::GetIO().MouseDelta.y;
		}
		else
			GridWinFocus = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

		GridCenterPosition.x = LIMIT_CLAMP(GridCenterPosition.x, ViewLimit0.x, ViewLimit0.y);
		GridCenterPosition.y = LIMIT_CLAMP(GridCenterPosition.y, ViewLimit1.x, ViewLimit1.y);

		GridCenterPositionSmooth.x += (GridCenterPosition.x - GridCenterPositionSmooth.x) * 0.1f * speed;
		GridCenterPositionSmooth.y += (GridCenterPosition.y - GridCenterPositionSmooth.y) * 0.1f * speed;

		ImGui::SetCursorPos(GridWindowPos);
		ImGui::BeginChild("@COORDWIN", coord_winsize, true, ImGuiWindowFlags_NoScrollbar);
		{
			// "2.0f":子窗口边框偏移修正.
			MousePosition = ImVec2(ImGui::GetMousePos().x - ImGui::GetWindowPos().x + 2.0f, ImGui::GetMousePos().y - ImGui::GetWindowPos().y + 2.0f);
			MousePosition.x = LIMIT_CLAMP(MousePosition.x, 0.0f, coord_winsize.x);
			MousePosition.y = LIMIT_CLAMP(MousePosition.y, 0.0f, coord_winsize.y);
			
			// mouse virtual coord.
			GridMousePosition.x = (MousePosition.x - GridCenterPosition.x - coord_winsize.x * 0.5f) / GridSizeScale.y;
			GridMousePosition.y = (GridCenterPosition.y - MousePosition.y + coord_winsize.y * 0.5f) / GridSizeScale.y;

			EditorCoordInfo = CoordSystemInfo(
				ImVec2(GridCenterPositionSmooth.x + coord_winsize.x * 0.5f, GridCenterPositionSmooth.y + coord_winsize.y * 0.5f),
				MousePosition,
				EditorSeBoxVirCoord[0], 
				EditorSeBoxVirCoord[1], 
				GridSizeScale.y
			);

			DrawGrid(CoordXLimit, CoordYLimit, ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.64f), GridCenterPositionSmooth, coord_winsize, GridSizeScale.y);
			draw(EditorCoordInfo);
			// mouse right.
			if (ImGui::IsMouseDown(1) && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
				DrawCoordinateLines(RulerCursorColor, MousePosition, coord_winsize);

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
					EditorSeBoxPoints[0] = MousePosition;
					EditorSeBoxVirCoord[0] = GridMousePosition;
				}
				EditorSeBoxPoints[1] = MousePosition;
				EditorSeBoxVirCoord[1] = GridMousePosition;

				// 垃圾imgui两点矩形还要区分min,max你怎么不直接让开发者写布雷森汉姆生成直线...
				ImVec2 DrawBoxMin = ImVec2(fmin(EditorSeBoxPoints[0].x, EditorSeBoxPoints[1].x), fmin(EditorSeBoxPoints[0].y, EditorSeBoxPoints[1].y));
				ImVec2 DrawBoxMax = ImVec2(fmax(EditorSeBoxPoints[0].x, EditorSeBoxPoints[1].x), fmax(EditorSeBoxPoints[0].y, EditorSeBoxPoints[1].y));

				ImVec2 CalcBoxMin = ImVec2(fmin(EditorSeBoxVirCoord[0].x, EditorSeBoxVirCoord[1].x), fmin(EditorSeBoxVirCoord[0].y, EditorSeBoxVirCoord[1].y));
				ImVec2 CalcBoxMax = ImVec2(fmax(EditorSeBoxVirCoord[0].x, EditorSeBoxVirCoord[1].x), fmax(EditorSeBoxVirCoord[0].y, EditorSeBoxVirCoord[1].y));

				ImVec2 BoxLen = IMVEC2_SUB2(CalcBoxMax, CalcBoxMin);
				ImGui::BeginTooltip();
				ImGui::Text("Size:%0.2f", BoxLen.x * BoxLen.y);
				ImGui::EndTooltip();

				// draw selection box color.
				ImVec4 BoxColor = ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.58f);
				// draw selection box.
				ImGui::GetWindowDrawList()->AddRectFilled(
					IMVEC2_ADD2(ImGui::GetWindowPos(), DrawBoxMin),
					IMVEC2_ADD2(ImGui::GetWindowPos(), DrawBoxMax),
					IM_COL32(ImU32(BoxColor.x * 255.0f), ImU32(BoxColor.y * 255.0f), ImU32(BoxColor.z * 255.0f), ImU32(BoxColor.w * 255.0f)),
					ImGui::GetStyle().FrameRounding,
					NULL
				);
			}
			else {
				EditorSeBoxVirCoord[0] = ImVec2();
				EditorSeBoxVirCoord[1] = ImVec2();
			}
		}
		ImGui::EndChild();

		ImVec2 DrawPosition(GridCenterPositionSmooth.x + coord_winsize.x * 0.5f, GridCenterPositionSmooth.y + coord_winsize.y * 0.5f);
		ImGui::SetCursorPos(ImVec2(CoordRulerWin + IMGUI_ITEM_SPC * 1.5f, IMGUI_ITEM_SPC * 4.25f));
		ImGui::BeginChild("@COORDX", ImVec2(coord_winsize.x, CoordRulerWin));
		{
			ImControlBase::ExtDrawRectangleFill(
				ImVec2(0.0f, 0.0f), 
				ImVec2(coord_winsize.x, CoordRulerWin), 
				ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.58f)
			);
			DrawCoordinateRulerX(CoordYLimit, EditorColorSystem, 15.0f, DrawPosition.x, GridSizeScale.y);
			// mouse position_x cursor.
			ImControlBase::ExtDrawLine(ImVec2(MousePosition.x, 0.0f), ImVec2(MousePosition.x, CoordRulerWin), RulerCursorColor, 2.0f);
		}
		ImGui::EndChild();

		ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, CoordRulerWin + IMGUI_ITEM_SPC * 4.75f));
		ImGui::BeginChild("@COORDY", ImVec2(CoordRulerWin, coord_winsize.y));
		{
			ImControlBase::ExtDrawRectangleFill(
				ImVec2(0.0f, 0.0f),
				ImVec2(CoordRulerWin, coord_winsize.y),
				ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.58f)
			);
			DrawCoordinateRulerY(CoordYLimit, EditorColorSystem, 15.0f, DrawPosition.y, GridSizeScale.y);
			// mouse position_y cursor.
			ImControlBase::ExtDrawLine(ImVec2(0.0f, MousePosition.y), ImVec2(CoordRulerWin, MousePosition.y), RulerCursorColor, 2.0f);
		}
		ImGui::EndChild();

		ImVec2 InfoWindowSize(240.0f, coord_winsize.y + CoordRulerWin + IMGUI_ITEM_SPC * 0.5f);
		// editor info panel.
		ImGui::SetCursorPos(ImVec2(CoordRulerWin + IMGUI_ITEM_SPC * 2.5f + coord_winsize.x, IMGUI_ITEM_SPC * 4.25f));
		ImGui::BeginChild("@INFOWIN", InfoWindowSize);
		{
			ImControlBase::ExtDrawRectangleFill(ImVec2(0.0f, 0.0f), InfoWindowSize, ImControlBase::ExtSubColorGrayscale(EditorColorSystem, 0.64f));

			ImGui::SetNextItemWidth(InfoWindowSize.x - ImGui::CalcTextSize("Grid").x - IMGUI_ITEM_SPC);
			ImGui::InputFloat2("GRID", &GridCenterPosition.x, "%.0f");

			ImGui::Spacing();
			if (ImGui::Button("RE", ImVec2(32.0f, 0.0f)))
				GridSizeScale.x = 1.0f;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(InfoWindowSize.x - IMGUI_ITEM_SPC - 32.0f);
			ImGui::SliderFloat("##SCALE", &GridSizeScale.x, GridScaleLimit.x, GridScaleLimit.y, "%.2f");
			
			ImGui::Spacing();
			PositioningWindow(GridCenterPosition, CoordXLimit, CoordYLimit, EditorColorSystem, poswin_image, InfoWindowSize.x, GridSizeScale.y);
			
			ImGui::Spacing();
			ImGui::SetNextItemWidth(InfoWindowSize.x);
			ImGui::ColorEdit4("##EDITCOL", &EditorColorSystem.x);

			ImGui::Spacing();
			ImGui::Indent(4.0f);
			ImGui::Text("[BoxP0] x:%.0f y:%.0f", EditorSeBoxVirCoord[0].x, EditorSeBoxVirCoord[0].y);
			ImGui::Text("[BoxP1] x:%.0f y:%.0f", EditorSeBoxVirCoord[1].x, EditorSeBoxVirCoord[1].y);
			ImGui::Text("[Coord] x:%.0f y:%.0f", GridMousePosition.x, GridMousePosition.y);
			ImGui::Text("[Mouse] x:%.0f y:%.0f", MousePosition.x, MousePosition.y);
			ImGui::Unindent(4.0f);

			EditorToolbar();
		}
		ImGui::EndChild();
	}
	ImGui::End();
	ImGui::PopStyleColor(9);
	ImGui::PopID();
}