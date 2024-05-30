// improfx_control_animaxis. RCSZ. [20231220]
// ImGui: [Window(Begin_End)], Animation Axis Editor, Update: 20240216.
#include "improfx_control.h"

#include <iostream>
using namespace std;

constexpr ImVec4 XYZCOL[3] = { ImVec4(1.0f, 0.2f, 0.2f, 1.0f), ImVec4(0.2f, 1.0f, 0.2f, 1.0f), ImVec4(0.5f, 0.5f, 1.0f, 1.0f) };
constexpr ImVec4 PYRCOL[3] = { ImVec4(1.0f, 0.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ImVec4(0.0f, 1.0f, 1.0f, 1.0f) };

constexpr const char* XYZTEXT[3] = { "Xaxis: %.2f","Yaxis: %.2f","Zaxis: %.2f" };
constexpr const char* PYRTEXT[3] = { "Pitch: %.2f","Yaw: %.2f","Roll: %.2f" };

constexpr float EditorSpaceLeft  = 200.0f;
constexpr float EditorSpaceBelow = 32.0f;

namespace IMFXC_WIN {
	AnimAxisEditorWindow::AnimAxisEditorWindow() {
		// config anim_button color & size.
		for (size_t i = 0; i < 6; i++) {
			AnimButton[i].SetButtonStatColor(
				IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.58f),
				IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.2f),
				EditorColorSystem
			);
		}

		float ButtonWidthA = (EditorSpaceLeft - IMGUI_ITEM_SPAC * 3.0f) * 0.5f;
		AnimButton[0].SetButtonStatSize(ImVec2(ButtonWidthA, 28.0f), ImVec2(ButtonWidthA, 28.0f), ImVec2(ButtonWidthA, 28.0f));
		AnimButton[1].SetButtonStatSize(ImVec2(ButtonWidthA, 28.0f), ImVec2(ButtonWidthA, 28.0f), ImVec2(ButtonWidthA, 28.0f));

		float ButtonWidthB = ButtonWidthA * 2.0f + IMGUI_ITEM_SPAC;
		AnimButton[2].SetButtonStatSize(ImVec2(ButtonWidthB, 28.0f), ImVec2(ButtonWidthB, 28.0f), ImVec2(ButtonWidthB, 28.0f));
		AnimButton[3].SetButtonStatSize(ImVec2(ButtonWidthB, 28.0f), ImVec2(ButtonWidthB, 28.0f), ImVec2(ButtonWidthB, 28.0f));

		AnimButton[4].SetButtonStatSize(ImVec2(34.0f, 26.0f), ImVec2(32.0f, 26.0f), ImVec2(32.0f, 26.0f));
		AnimButton[5].SetButtonStatSize(ImVec2(34.0f, 26.0f), ImVec2(32.0f, 26.0f), ImVec2(32.0f, 26.0f));
	}

	void AnimAxisEditorWindow::MouseSetPointValue() {
		if (NodeSetValuePointer && ImGui::IsMouseDown(0)) {
			*NodeSetValuePointer -= ImGui::GetIO().MouseDelta.y * 0.92f;

			// floating window(view value).
			ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.25f, 0.25f, 0.25f, 0.72f));
			ImGui::BeginTooltip();
			ImGui::Text("Value:%.1f", *NodeSetValuePointer);
			ImGui::EndTooltip();
			ImGui::PopStyleColor();
		}
		else
			NodeSetValuePointer = nullptr;
	}

	void AnimAxisEditorWindow::SpacingLine(float space_left, const ImVec4& color) {
		ImGui::Spacing();
		IM_CONTROL_BASE::ListDrawLine(
			ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - ImGui::GetScrollY()),
			ImVec2(ImGui::GetCursorPosX() + space_left - IMGUI_ITEM_SPAC * 2.0f, ImGui::GetCursorPosY() - ImGui::GetScrollY()),
			color, 2.0f
		);
		ImGui::Spacing();
	}

	void AnimAxisEditorWindow::DrawCubicBezierCurve(
		const ImVec2& point0, const ImVec2& point1, const ImVec4& color, const ImVec2& scale,
		float offset, int sample, float centerh
	) {
		// calculate bessel_curve draw_points.
		ImVec2 DrawPoint0(point0.x * scale.x - offset, centerh - point0.y * scale.y);
		ImVec2 DrawPoint1(point1.x * scale.x - offset, centerh - point1.y * scale.y);

		ImVec2 MidPoint = (DrawPoint0 + DrawPoint1) * 0.5f;
		ImVec2 ControlPointA = MidPoint - ImVec2(0.0f, (DrawPoint1 - MidPoint).y);
		ImVec2 ControlPointB = MidPoint + ImVec2(0.0f, (DrawPoint1 - MidPoint).y);

		ImVec2 BeginPointTemp = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, 0.0f);
		for (int i = 0; i < sample; ++i) {

			ImVec2 DrawPoint = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, (float)i / (float)sample);
			IM_CONTROL_BASE::ListDrawLine(BeginPointTemp, DrawPoint, color, EditorScaleLinesWidth * 2.0f);
			BeginPointTemp = DrawPoint;
		}
	}

	void AnimAxisEditorWindow::DrawAnimationPoints(const ImVec2& position, float size, const ImVec4& color, float& value) {
		if (&value == NodeSetValuePointer)
			IM_CONTROL_BASE::ListDrawCircleFill(position, size, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		else
			IM_CONTROL_BASE::ListDrawCircleFill(position, size, color);

		ImGui::SetCursorPos(ImVec2(position.x - size, position.y - size));
		ImGui::InvisibleButton("@SETVALUE", ImVec2(size * 2.0f, size * 2.0f));

		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0) && !NodeSetValuePointer && !PlayerSetScrollyFlag)
			NodeSetValuePointer = &value;
	}

	void AnimAxisEditorWindow::DrawPlayerLine(ImVec2& position, float offset, float max, const ImVec4& color, float xscale) {
		float PlayerRectWidth = IMGUI_ITEM_SPAC * 3.2f;
		float PlayerLineWidth = 2.2f;

		IM_CONTROL_BASE::ListDrawLine(
			ImVec2(position.y * xscale - offset - PlayerLineWidth * 0.5f, IMGUI_ITEM_SPAC + PlayerRectWidth * 0.5f),
			ImVec2(position.y * xscale - offset - PlayerLineWidth * 0.5f, ImGui::GetWindowHeight() - IMGUI_ITEM_SPAC * 4.0f),
			color,
			EditorScaleLinesWidth * 2.2f
		);
		IM_CONTROL_BASE::ListDrawLine(
			ImVec2(position.y * xscale - offset + IMGUI_ITEM_SPAC, ImGui::GetWindowHeight() - IMGUI_ITEM_SPAC * 4.0f),
			ImVec2(position.y * xscale - offset - IMGUI_ITEM_SPAC, ImGui::GetWindowHeight() - IMGUI_ITEM_SPAC * 4.0f),
			color,
			EditorScaleLinesWidth * 2.2f
		);

		IM_CONTROL_BASE::ListDrawRectangleFill(
			ImVec2(position.y * xscale - offset - PlayerRectWidth * 0.5f, IMGUI_ITEM_SPAC),
			ImVec2(PlayerRectWidth, IMGUI_ITEM_SPAC * 2.0f),
			color
		);
		ImGui::SetCursorPos(ImVec2(position.y * xscale - offset - PlayerRectWidth * 0.5f, IMGUI_ITEM_SPAC));
		ImGui::InvisibleButton("@SETPLAYER", ImVec2(PlayerRectWidth, IMGUI_ITEM_SPAC * 2.0f));

		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0) && !NodeSetValuePointer)
			PlayerSetScrollyFlag = true;
		if (PlayerSetScrollyFlag && !ImGui::IsMouseDown(0))
			PlayerSetScrollyFlag = false;
		if (PlayerSetScrollyFlag)
			position.x += ImGui::GetIO().MouseDelta.x / xscale;

		position.x = position.x < 0.0f ? 0.0f : position.x;
		position.x = position.x > max ? max   : position.x;
	}

	bool AnimAxisEditorWindow::RunGetCubicBezierCurve(
		const ImVec2& point0, const ImVec2& point1, float& value, float playerpos, float centerh,
		vector<AnimCoordSample>& src, size_t index
	) {
		// calculate bessel_curve resul_value.
		ImVec2 CalcPoint0(point0.x, centerh - point0.y);
		ImVec2 CalcPoint1(point1.x, centerh - point1.y);

		float PrevDist = src[index].TimePosition;

		if (CalcPoint0.x < playerpos && CalcPoint1.x > playerpos) {
			// bessel_curve control_points.
			ImVec2 MidPoint = (CalcPoint0 + CalcPoint1) * 0.5f;
			ImVec2 ControlPointA = MidPoint - ImVec2(0.0f, (CalcPoint1 - MidPoint).y);
			ImVec2 ControlPointB = MidPoint + ImVec2(0.0f, (CalcPoint1 - MidPoint).y);
			
			value = ImBezierCubicCalc(
				CalcPoint0, ControlPointA, ControlPointB, CalcPoint1, float((int)playerpos - PrevDist) / (CalcPoint1.x - CalcPoint0.x)
			).y;
			return true;
		}
		else {
			value = 0.0f;
			return false;
		}
	}

	bool AnimAxisEditorWindow::PlayerRunSample(AnimGenCoord& CoordParam) {
		CoordParam = PlayerRunCoord;
		return PlayerFlag;
	};

	std::vector<AnimGenCoord> AnimAxisEditorWindow::GenerateBakedBezierCurve() {
		std::vector<AnimGenCoord> BakeGenerate = {};

		for (size_t i = 0; i < AnimDataIndex->size() - 1; ++i) {
			AnimCoordSample DatTempA = (*AnimDataIndex)[i];
			AnimCoordSample DatTempB = (*AnimDataIndex)[i + 1];

			for (uint32_t smp = 0; smp < (*AnimDataIndex)[i].BakeSamplingRate; ++smp) {
				AnimGenCoord CoordTemp = {};

				for (size_t j = 0; j < ANE_COORD_PARAMS; ++j) {
					ImVec2 BezPoint0(DatTempA.TimePosition, DatTempA.AnimSamplePoints[j]);
					ImVec2 BezPoint1(DatTempB.TimePosition, DatTempB.AnimSamplePoints[j]);

					ImVec2 MidPoint = (BezPoint0 + BezPoint1) * 0.5f;
					ImVec2 ControlPointA = MidPoint - ImVec2(0.0f, (BezPoint1 - MidPoint).y);
					ImVec2 ControlPointB = MidPoint + ImVec2(0.0f, (BezPoint1 - MidPoint).y);

					ImVec2 DrawPoint = ImBezierCubicCalc(BezPoint0, ControlPointA, ControlPointB, BezPoint1, (float)smp / (float)DatTempA.BakeSamplingRate);
					CoordTemp.AnimGenVector[j] = DrawPoint.x;
				}
				BakeGenerate.push_back(CoordTemp);
			}
		}
		return BakeGenerate;
	}

	void AnimAxisEditorWindow::DrawEditorWindow(
		uint32_t                 unqiue_id,
		const char*              name,
		float                    track_length,
		float                    player_step,
		vector<AnimCoordSample>& sample,
		bool                     fixed_window,
		bool*                    p_open,
		ImGuiWindowFlags         flags
	) {
		AnimDataIndex = &sample;

		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		if (fixed_window)
			WindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		ImGui::PushID(unqiue_id);

		// frame_background color.
		ImGui::PushStyleColor(ImGuiCol_FrameBg,        IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.58f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.58f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.58f));
		// slider_block color.
		ImGui::PushStyleColor(ImGuiCol_SliderGrab,       IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.16f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.16f));
		ImGui::PushStyleColor(ImGuiCol_Text,             IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.24f));
		
		ImGui::Begin(name, p_open, WindowFlags | flags);
		{
			ImVec2 ChildWindowSize = ImVec2(ImGui::GetWindowSize().x - EditorSpaceLeft, ImGui::GetWindowSize().y - EditorSpaceBelow);

			ImGui::SetCursorPos(ImVec2(EditorSpaceLeft, ImGui::GetWindowSize().y - EditorSpaceBelow - IMGUI_ITEM_SPAC * 0.5f));
			ImGui::SetNextItemWidth(ChildWindowSize.x - ImGui::CalcTextSize("TRACK").x - IMGUI_ITEM_SPAC * 2.0f);

			TrackWindowScrollx.y = track_length * TrackWidthValueScale - ChildWindowSize.x * 0.85f;
			TrackWindowScrollx.y = TrackWindowScrollx.y < 0.0f ? 0.0f : TrackWindowScrollx.y;

			ImGui::SliderFloat("TRACK", &TrackWindowScrollx.x, 0.0f, TrackWindowScrollx.y, "");

			ImGui::SetCursorPos(ImVec2(EditorSpaceLeft, IMGUI_ITEM_SPAC * 4.32f));
			// draw animation_track editor window.
			ImVec2 WindowSizeTemp(ChildWindowSize.x - IMGUI_ITEM_SPAC, ChildWindowSize.y - IMGUI_ITEM_SPAC * 5.5f);
			ImGui::BeginChild("@ANIMTRACK", WindowSizeTemp, false);

			// track window background rect.
			IM_CONTROL_BASE::ListDrawRectangleFill(ImVec2(0.0f, 0.0f), WindowSizeTemp, IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.65f));

			ImVec4 LinesColor = IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.42f);
			// value_zero base_line.
			float HighCenterPosition = ImGui::GetWindowHeight() * 0.5f;
			TrackScrollx += (TrackWindowScrollx.x - IMGUI_ITEM_SPAC * 4.0f - TrackScrollx) * 0.1f;

			for (float i = 50.0f; i <= track_length; i += 100.0f) {
				float LinesScrollxTemp = i * TrackWidthValueScale - TrackScrollx;

				LinesColor.w = 0.32f;
				IM_CONTROL_BASE::ListDrawLine(
					ImVec2(LinesScrollxTemp, IMGUI_ITEM_SPAC * 7.0f),
					ImVec2(LinesScrollxTemp, ImGui::GetWindowHeight() - IMGUI_ITEM_SPAC * 10.0f),
					LinesColor,
					EditorScaleLinesWidth
				);
			}

			char TimeTickText[8] = {}; // char_array.
			for (float i = 0.0f; i <= track_length; i += 100.0f) {
				sprintf_s(TimeTickText, "%.0f", i);

				float LinesScrollxTemp = i * TrackWidthValueScale - TrackScrollx;

				// draw time_tick text.
				ImVec2 TextPos(LinesScrollxTemp - ImGui::CalcTextSize(TimeTickText).x * 0.5f, ImGui::GetWindowHeight() - IMGUI_ITEM_SPAC * 3.5f);
				LinesColor.w = 0.92f; // color ahpla.
				IM_CONTROL_BASE::ListDrawText(TextPos, LinesColor, "%.0f", i);

				LinesColor.w = 0.32f;
				IM_CONTROL_BASE::ListDrawLine(
					ImVec2(LinesScrollxTemp, IMGUI_ITEM_SPAC),
					ImVec2(LinesScrollxTemp, ImGui::GetWindowHeight() - IMGUI_ITEM_SPAC * 4.0f),
					LinesColor,
					EditorScaleLinesWidth * 3.2f
				);
			}

			// draw centerline[value: 0.0f].
			IM_CONTROL_BASE::ListDrawLine(
				ImVec2(0.0f, HighCenterPosition),
				ImVec2(ImGui::GetWindowWidth(), HighCenterPosition),
				LinesColor,
				EditorScaleLinesWidth * 1.8f
			);

			if (EditorModeType) {
				std::copy(std::begin(PYRCOL), std::end(PYRCOL), std::begin(SystemAsixColors));
				std::copy(std::begin(PYRTEXT), std::end(PYRTEXT), std::begin(SystemAsixTexts));
			}
			else {
				std::copy(std::begin(XYZCOL), std::end(XYZCOL), std::begin(SystemAsixColors));
				std::copy(std::begin(XYZTEXT), std::end(XYZTEXT), std::begin(SystemAsixTexts));
			}

			if (!sample.empty()) {
				size_t AnimCount = size_t(sample.size() >= 2);

				for (size_t i = 0; i < sample.size() - 1; ++i) {
					float SampleCoordIndex[ANE_COORD_PARAMS] = {};
					// animation points.
					for (size_t j = 0; j < 3; ++j) {
						size_t ParamCount = j + (size_t)EditorModeType * 3;

						DrawCubicBezierCurve(
							ImVec2(sample[i].TimePosition, sample[i].AnimSamplePoints[ParamCount]),
							ImVec2(sample[i + AnimCount].TimePosition, sample[i + AnimCount].AnimSamplePoints[ParamCount]),
							SystemAsixColors[j],
							ImVec2(TrackWidthValueScale, TrackHeightValueScale),
							TrackScrollx,
							sample[i].PlayerSamplingRate,
							HighCenterPosition
						);
						DrawAnimationPoints(
							ImVec2(sample[i].TimePosition * TrackWidthValueScale - TrackScrollx, HighCenterPosition - sample[i].AnimSamplePoints[ParamCount] * TrackHeightValueScale),
							4.2f,
							SystemAsixColors[j],
							sample[i].AnimSamplePoints[ParamCount]
						);
					}

					// animation bezier_curve.
					for (size_t j = 0; j < 6; ++j) {
						if (RunGetCubicBezierCurve(
							ImVec2(sample[i].TimePosition, sample[i].AnimSamplePoints[j]),
							ImVec2(sample[i + AnimCount].TimePosition, sample[i + AnimCount].AnimSamplePoints[j]),
							SampleCoordIndex[j],
							PlayerLineScrollx.y,
							HighCenterPosition,
							sample, i
						))
							PlayerRunCoord.AnimGenVector[j] = HighCenterPosition - SampleCoordIndex[j];
					}
				}
				size_t Last = sample.size() - 1;
				for (size_t j = 0; j < 3; ++j) {
					size_t ParamCount = j + (size_t)EditorModeType * 3;

					DrawAnimationPoints(
						ImVec2(sample[Last].TimePosition * TrackWidthValueScale - TrackScrollx, HighCenterPosition - sample[Last].AnimSamplePoints[ParamCount] * TrackHeightValueScale),
						4.2f,
						SystemAsixColors[j],
						sample[Last].AnimSamplePoints[ParamCount]
					);
				}
			}
			MouseSetPointValue();
			DrawPlayerLine(PlayerLineScrollx, TrackScrollx, track_length, EditorColorPlayer, TrackWidthValueScale);

			ImGui::EndChild();

			PlayerLineScrollx.y += (PlayerLineScrollx.x - PlayerLineScrollx.y) * 0.12f;
			PlayerLineScrollx.y = abs(PlayerLineScrollx.x - PlayerLineScrollx.y) < 0.01f ? PlayerLineScrollx.x : PlayerLineScrollx.y;

			ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPAC, IMGUI_ITEM_SPAC * 4.32f));
			ImGui::Text("TICK: %.1f", PlayerLineScrollx.y);
			ImGui::Spacing();

			if (AnimButton[0].DrawButton("Begin")) {
				PlayerLineScrollx.x = 0.0f;
				PlayerLineScrollx.y = 0.0f;
			}
			ImGui::SameLine();
			if (AnimButton[1].DrawButton("End")) {
				PlayerLineScrollx.x = track_length;
				PlayerLineScrollx.y = track_length;
			}
			ImGui::Spacing();

			auto ModeButtonFunc = [&](bool& mode_flag, const char* fname, const char* tname, size_t butidx) {
				if (mode_flag) {
					if (AnimButton[butidx].DrawButton(fname))
						mode_flag = !mode_flag;
				}
				else
					if (AnimButton[butidx].DrawButton(tname))
						mode_flag = !mode_flag;
			};

			ModeButtonFunc(PlayerFlag, "STOP", "PLAY", 2);
			if (PlayerFlag)
				PlayerLineScrollx.x += player_step * TrackWidthValueScale;

			ImGui::Spacing();
			ModeButtonFunc(EditorModeType, "X.Y.Z", "P.Y.R", 3);
			
			float SpaceItemLeft = EditorSpaceLeft - IMGUI_ITEM_SPAC * 2.0f;

			ImGui::Spacing();
			if (AnimButton[4].DrawButton("RH"))
				TrackHeightValueScale = 1.0f;

			ImGui::SameLine();
			ImGui::SetNextItemWidth(SpaceItemLeft - 32.0f - IMGUI_ITEM_SPAC);
			ImGui::SliderFloat("##HIGHSCALE", &TrackHeightValueScale, 0.02f, 2.0f, "%.2f");

			ImGui::Spacing();
			if (AnimButton[5].DrawButton("RW"))
				TrackWidthValueScale = 1.0f;

			ImGui::SameLine();
			ImGui::SetNextItemWidth(SpaceItemLeft - 32.0f - IMGUI_ITEM_SPAC);
			ImGui::SliderFloat("##WIDTHSCALE", &TrackWidthValueScale, 0.5f, 2.0f, "%.2f");

			ImGui::Spacing();
			ImGui::SetNextItemWidth(SpaceItemLeft);
			ImGui::InputFloat("##SETPOS", &PlayerLineScrollx.x, 0.0f, 0.0f, "%.1f");

			SpacingLine(EditorSpaceLeft, LinesColor);
			{
				for (size_t i = 0; i < 3; ++i)
					ImGui::TextColored(SystemAsixColors[i], SystemAsixTexts[i], PlayerRunCoord.AnimGenVector[i + (size_t)EditorModeType * 3]);
			}
			SpacingLine(EditorSpaceLeft, LinesColor);
			ImGui::Text("Animations:%u", sample.size());
		}
		ImGui::End();
		ImGui::PopStyleColor(9);
		ImGui::PopID();
	}
}