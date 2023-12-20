// improfx_control_animeditor. RCSZ.
// ImGui: [Window(Begin_End)], Animation Nodes Editor, Update: 20231220.

#include "improfx_control.h"

void AnimTrackEditorWindow::MouseSetPointValue() {
	if (NodeSetValuePointer && ImGui::IsMouseDown(0))
		*NodeSetValuePointer -= ImGui::GetIO().MouseDelta.y;
	else
		NodeSetValuePointer = nullptr;
}

void AnimTrackEditorWindow::SpacingLine(float space_left, const ImVec4& color) {
	ImGui::Spacing();
	ImControlBase::ExtDrawLine(
		ImGui::GetCursorPos(), 
		ImVec2(ImGui::GetCursorPosX() + space_left - IMGUI_ITEM_SPC * 2.0f, ImGui::GetCursorPosY()), 
		color, 2.0f
	);
	ImGui::Spacing();
}

bool AnimTrackEditorWindow::RenderCubicBezierCurve(
	const ImVec2& point0, const ImVec2& point1, const ImVec4& color, 
	float offset, int sample, float& value, float playerpos
) {
	// calculate control_points (bessel_curve).
	ImVec2 DrawPoint0(point0.x - offset, point0.y);
	ImVec2 DrawPoint1(point1.x - offset, point1.y);

	ImVec2 MidPoint      = IMVEC2_MUL1(IMVEC2_ADD2(DrawPoint0, DrawPoint1), 0.5f);
	ImVec2 ControlPointA = IMVEC2_SUB2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(DrawPoint1, MidPoint).y));
	ImVec2 ControlPointB = IMVEC2_ADD2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(DrawPoint1, MidPoint).y));

	ImVec2 LinePointTemp = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, 0.0f);
	for (int i = 0; i < sample; ++i) {

		ImVec2 DrawPoint = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, (float)i / (float)sample);
		ImControlBase::ExtDrawLine(LinePointTemp, DrawPoint, color, EditorScaleLinesWidth * 2.0f);
		LinePointTemp = DrawPoint;
	}
	if (DrawPoint0.x + offset < playerpos && DrawPoint1.x + offset > playerpos) {
		value = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, float((int)playerpos % 100) / (DrawPoint1.x - DrawPoint0.x)).y;
		return true;
	}
	else {
		value = 0.0f;
		return false;
	}
}

void AnimTrackEditorWindow::RenderAnimationPoints(const ImVec2& position, float size, const ImVec4& color, float& value) {
	ImControlBase::ExtDrawCircleFill(position, size, color);

	ImGui::SetCursorPos(ImVec2(position.x - size, position.y - size));
	ImGui::InvisibleButton("@SETVALUE", ImVec2(size * 2.0f, size * 2.0f));

	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0) && !NodeSetValuePointer && !PlayerSetposFlag)
		NodeSetValuePointer = &value;
}

void AnimTrackEditorWindow::RenderPlayerLine(ImVec2& position, float offset, float max, const ImVec4& color) {
	float PlayerRectWidth = IMGUI_ITEM_SPC * 3.2f;
	float PlayerLineWidth = 2.2f;

	ImControlBase::ExtDrawLine(
		ImVec2(position.y - offset - PlayerLineWidth * 0.5f, IMGUI_ITEM_SPC + PlayerRectWidth * 0.5f),
		ImVec2(position.y - offset - PlayerLineWidth * 0.5f, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
		color,
		EditorScaleLinesWidth * 2.2f
	);
	ImControlBase::ExtDrawLine(
		ImVec2(position.y - offset + IMGUI_ITEM_SPC, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
		ImVec2(position.y - offset - IMGUI_ITEM_SPC, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
		color,
		EditorScaleLinesWidth * 2.2f
	);

	ImControlBase::ExtDrawRectangleFill(
		ImVec2(position.y - offset - PlayerRectWidth * 0.5f, IMGUI_ITEM_SPC),
		ImVec2(PlayerRectWidth, IMGUI_ITEM_SPC * 2.0f),
		color
	);
	ImGui::SetCursorPos(ImVec2(position.y - offset - PlayerRectWidth * 0.5f, IMGUI_ITEM_SPC));
	ImGui::InvisibleButton("@SETPLAYER", ImVec2(PlayerRectWidth, IMGUI_ITEM_SPC * 2.0f));

	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0) && !NodeSetValuePointer)
		PlayerSetposFlag = true;
	if (PlayerSetposFlag && !ImGui::IsMouseDown(0))
		PlayerSetposFlag = false;
	if (PlayerSetposFlag)
		position.x += ImGui::GetIO().MouseDelta.x;

	position.x = position.x < 0.0f ? 0.0f : position.x;
	position.x = position.x > max ? max : position.x;
}

bool AnimTrackEditorWindow::PlayerRunSample(AnimGenCoord& CoordParam) {
	CoordParam = PlayerRunCoord;
	return PlayerPlayFlag;
};

std::vector<AnimGenCoord> AnimTrackEditorWindow::GenerateBakedBezierCurve() {
	std::vector<AnimGenCoord> BakeGenerate = {};

	for (size_t i = 0; i < AnimDataIndex->size() - 1; ++i) {
		AnimCoordSample DatTempA = (*AnimDataIndex)[i];
		AnimCoordSample DatTempB = (*AnimDataIndex)[i + 1];

		for (uint32_t smp = 0; smp < (*AnimDataIndex)[i].BakeSamplingRate; ++smp) {
			AnimGenCoord CoordTemp = {};

			for (size_t j = 0; j < 3; ++j) {
				ImVec2 DrawPoint0(DatTempA.TimePosition, DatTempA.AnimSamplePoints[j]);
				ImVec2 DrawPoint1(DatTempB.TimePosition, DatTempB.AnimSamplePoints[j]);

				ImVec2 MidPoint      = IMVEC2_MUL1(IMVEC2_ADD2(DrawPoint0, DrawPoint1), 0.5f);
				ImVec2 ControlPointA = IMVEC2_SUB2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(DrawPoint1, MidPoint).y));
				ImVec2 ControlPointB = IMVEC2_ADD2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(DrawPoint1, MidPoint).y));

				ImVec2 DrawPoint = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, (float)smp / (float)DatTempA.BakeSamplingRate);
				CoordTemp.AnimGenVector[j] = DrawPoint.x;
			}
			BakeGenerate.push_back(CoordTemp);
		}
	}
	return BakeGenerate;
}

void AnimTrackEditorWindow::DrawEditorWindow(
	uint32_t                      unqiue_id,
	const char*                   name,
	float                         track_length,
	float                         player_step,
	std::vector<AnimCoordSample>& sample,
	bool                          fixed_window,
	bool*                         p_open,
	ImGuiWindowFlags              flags
) {
	const float SpaceLeft  = 160.0f;
	const float SpaceBelow = 32.0f;
	AnimDataIndex = &sample;

	ImGuiWindowFlags WindowFlags = 0;
	if (fixed_window)
		WindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
	ImGui::PushID(unqiue_id);
	ImGui::Begin(name, p_open, WindowFlags | flags);
	{
		ImVec2 ChildWindowSize = ImVec2(ImGui::GetWindowSize().x - SpaceLeft, ImGui::GetWindowSize().y - SpaceBelow);

		ImGui::SetCursorPos(ImVec2(SpaceLeft, ImGui::GetWindowSize().y - SpaceBelow - IMGUI_ITEM_SPC * 0.5f));
		ImGui::SetNextItemWidth(ChildWindowSize.x - ImGui::CalcTextSize("TRACK").x - IMGUI_ITEM_SPC * 2.0f);

		TrackWindowXpos.y = track_length - ChildWindowSize.x * 0.85f;
		TrackWindowXpos.y = TrackWindowXpos.y < 0.0f ? 0.0f : TrackWindowXpos.y;

		// frame_background color.
		ImGui::PushStyleColor(ImGuiCol_FrameBg, EditorColorTimeLines);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, EditorColorTimeLines);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, EditorColorTimeLines);
		// slider_block color.
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImControlBase::ExtSubColorGrayscale(EditorColorTimeLines, -0.32f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImControlBase::ExtSubColorGrayscale(EditorColorTimeLines, -0.32f));
		{
			ImGui::SliderFloat("TRACK", &TrackWindowXpos.x, 0.0f, TrackWindowXpos.y);
		}
		ImGui::PopStyleColor(5);

		ImGui::SetCursorPos(ImVec2(SpaceLeft, IMGUI_ITEM_SPC * 4.32f));
		// draw animation_track editor window.
		ImGui::BeginChild("@TIMETRACK", ImVec2(ChildWindowSize.x - IMGUI_ITEM_SPC, ChildWindowSize.y - IMGUI_ITEM_SPC * 5.5f), true);

		float HeightCenter = ImGui::GetWindowHeight() * 0.5f;
		TrackXpos += (TrackWindowXpos.x - IMGUI_ITEM_SPC * 4.0f - TrackXpos) * 0.1f;

		for (float i = 0.0f; i <= track_length; i += 50.0f) {
			ImControlBase::ExtDrawLine(
				ImVec2(i - TrackXpos, IMGUI_ITEM_SPC),
				ImVec2(i - TrackXpos, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
				EditorColorTimeLines,
				EditorScaleLinesWidth
			);
		}

		for (float i = 0.0f; i <= track_length; i += 100.0f) {
			char TimeTickText[32] = {};
			sprintf_s(TimeTickText, "%.0f", i);

			// draw time_tick text.
			ImControlBase::ExtDrawText(
				ImVec2(i - TrackXpos - ImGui::CalcTextSize(TimeTickText).x * 0.5f, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 3.5f),
				EditorColorTimeLines, "%.0f", i
			);

			ImControlBase::ExtDrawLine(
				ImVec2(i - TrackXpos, IMGUI_ITEM_SPC),
				ImVec2(i - TrackXpos, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
				EditorColorTimeLines,
				EditorScaleLinesWidth * 2.0f
			);
		}

		// draw centerline[value: 0.0f].
		ImControlBase::ExtDrawLine(
			ImVec2(0.0f, HeightCenter),
			ImVec2(ImGui::GetWindowWidth(), HeightCenter),
			EditorColorTimeLines,
			EditorScaleLinesWidth * 1.8f
		);

		const ImVec4 AsixColors[3] = {
			ImVec4(1.0f, 0.2f, 0.2f, 1.0f), ImVec4(0.2f, 1.0f, 0.2f, 1.0f), ImVec4(0.5f, 0.5f, 1.0f, 1.0f)
		};

		for (size_t i = 0; i < sample.size() - 1; ++i) {
			size_t AnimCount = size_t(sample.size() >= 2);
			float SampleCoordIndex[3] = {};

			for (size_t j = 0; j < 3; ++j) {
				if (RenderCubicBezierCurve(
					ImVec2(sample[i].TimePosition, (HeightCenter - sample[i].AnimSamplePoints[j] * TrackValueScale)),
					ImVec2(sample[i + AnimCount].TimePosition, (HeightCenter - sample[i + AnimCount].AnimSamplePoints[j] * TrackValueScale)),
					AsixColors[j],
					TrackXpos, sample[i].PlayerSamplingRate, SampleCoordIndex[j], PlayerLineXpos.y
				))
					PlayerRunCoord.AnimGenVector[j] = HeightCenter - SampleCoordIndex[j];

				RenderAnimationPoints(
					ImVec2(sample[i].TimePosition - TrackXpos, HeightCenter - sample[i].AnimSamplePoints[j] * TrackValueScale),
					4.2f,
					AsixColors[j],
					sample[i].AnimSamplePoints[j]
				);
			}
		}
		size_t LastPoint = sample.size() - 1;
		for (size_t j = 0; j < 3; ++j)
			RenderAnimationPoints(
				ImVec2(sample[LastPoint].TimePosition - TrackXpos, HeightCenter - sample[LastPoint].AnimSamplePoints[j] * TrackValueScale),
				4.2f,
				AsixColors[j],
				sample[LastPoint].AnimSamplePoints[j]
			);

		MouseSetPointValue();
		RenderPlayerLine(PlayerLineXpos, TrackXpos, track_length, EditorColorPlayer);

		ImGui::EndChild();

		PlayerLineXpos.y += (PlayerLineXpos.x - PlayerLineXpos.y) * 0.12f;
		PlayerLineXpos.y = abs(PlayerLineXpos.x - PlayerLineXpos.y) < 0.01f ? PlayerLineXpos.x : PlayerLineXpos.y;

		ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC * 4.32f));
		ImGui::Text("TICK: %.1f", PlayerLineXpos.y);
		ImGui::Spacing();

		float ButtonWidth = (SpaceLeft - IMGUI_ITEM_SPC * 3.0f) * 0.5f;
		if (ImGui::Button("Begin", ImVec2(ButtonWidth, 0.0f))) {
			PlayerLineXpos.x = 0.0f;
			PlayerLineXpos.y = 0.0f;
		}
		ImGui::SameLine();
		if (ImGui::Button("End", ImVec2(ButtonWidth, 0.0f))) {
			PlayerLineXpos.x = track_length;
			PlayerLineXpos.y = track_length;
		}
		ImGui::Spacing();

		float SpaceLeftItem = SpaceLeft - IMGUI_ITEM_SPC * 2.0f;
		const char* PlayerButtonName = {};
		if (PlayerPlayFlag) {
			PlayerLineXpos.x += player_step;
			PlayerButtonName = "STOP";
		}
		else
			PlayerButtonName = "PLAY";
		if (ImGui::Button(PlayerButtonName, ImVec2(SpaceLeftItem, 0.0f)))
			PlayerPlayFlag = !PlayerPlayFlag;

		ImGui::Spacing();
		if (ImGui::Button("RE", ImVec2(32.0f, 0.0f)))
			TrackValueScale = 1.0f;
		ImGui::SameLine();
		ImGui::SetNextItemWidth(SpaceLeftItem - 32.0f - IMGUI_ITEM_SPC);
		ImGui::SliderFloat("##HIGHSCALE", &TrackValueScale, 0.05f, 2.0f);

		ImGui::Spacing();
		ImGui::SetNextItemWidth(SpaceLeftItem);
		ImGui::InputFloat("##SETPOS", &PlayerLineXpos.x, 0.0f, 0.0f, "%.1f");

		SpacingLine(SpaceLeft, EditorColorTimeLines);
		{
			ImGui::TextColored(AsixColors[0], "Xaxis: %.2f", PlayerRunCoord.AnimGenVector[0]);
			ImGui::TextColored(AsixColors[1], "Yaxis: %.2f", PlayerRunCoord.AnimGenVector[1]);
			ImGui::TextColored(AsixColors[2], "Zaxis: %.2f", PlayerRunCoord.AnimGenVector[2]);
		}
		SpacingLine(SpaceLeft, EditorColorTimeLines);

		ImGui::Text("Nodes: %u", sample.size());
		size_t BakedSampleSize = NULL;
		for (const auto& item : sample)
			BakedSampleSize += item.BakeSamplingRate;
		ImGui::Text("Baked: %u", BakedSampleSize);
	}
	ImGui::End();
	ImGui::PopID();
}