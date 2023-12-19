// improfx_control_animeditor. RCSZ.
// 2023_12_20 Animation Editor, Update: 20231220.

#ifndef _IMPROFX_CONTROL_ANIMEDITOR_HPP
#define _IMPROFX_CONTROL_ANIMEDITOR_HPP
#include "improfx_control_base.hpp"

#define IMVEC2_ADD2(v1, v2) ImVec2((v1).x + (v2).x, (v1).y + (v2).y)
#define IMVEC2_SUB2(v1, v2) ImVec2((v1).x - (v2).x, (v1).y - (v2).y)
#define IMVEC2_MUL1(v, s)   ImVec2((v).x * (s), (v).y * (s))

struct AnimCoordSample {
	float AnimSamplePoints[3];
	float TimePosition;

	uint32_t PlaySample;
	uint32_t GenerateSample;

	AnimCoordSample() : AnimSamplePoints{}, TimePosition(0.0f), PlaySample(128), GenerateSample(256) {}
	AnimCoordSample(float x, float y, float z, float t, uint32_t ps = 128, uint32_t gs = 256) :
		AnimSamplePoints{ x, y, z }, TimePosition(t), PlaySample(ps), GenerateSample(gs)
	{}
};

struct AnimGenCoord {
	float AnimGenVector[3];
};

// [Window]: 动画(轨道)编辑器. 2023_12_19 RCSZ.
class AnimTrackEditorWindow {
protected:
	ImVec4 EditorPlayerColor    = ImVec4(0.0f, 0.72f, 0.72f, 1.0f);
	ImVec4 EditorTickLinesColor = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);

	float EdirorTrackScale      = 1.0f;
	float EditorLinesWidthScale = 1.0f;

	ImVec2 TrackWindowXpos = {};   // x:pos, y:max.
	float  TrackXpos       = 0.0f; // tick trans smooth.

	float* NodeSetValuePointer = nullptr;

	ImVec2 PlayerPositionX    = {}; // x:pos, y:smooth
	bool   PlayerSetValueFlag = false;
	bool   PlayerPlayFlag     = false;

	float TrackYscale = 1.0f;

	AnimGenCoord PlayerSample = {};
	std::vector<AnimCoordSample>* NodesDataIdx = nullptr;

	// 鼠标拖动设置节点值.
	void MouseSetNodeValue() {
		if (NodeSetValuePointer && ImGui::IsMouseDown(0))
			*NodeSetValuePointer -= ImGui::GetIO().MouseDelta.y;
		else
			NodeSetValuePointer = nullptr;
	}

	void SpacingLine(float space_left, const ImVec4& color) {
		ImGui::Spacing();
		ImExtBaseControl::ExtDrawLine(ImGui::GetCursorPos(), ImVec2(ImGui::GetCursorPosX() + space_left - IMGUI_ITEM_SPC * 2.0f, ImGui::GetCursorPosY()), color, 2.0f);
		ImGui::Spacing();
	}

	bool RenderCubicBezierCurve(const ImVec2& point0, const ImVec2& point1, const ImVec4& color, int sample, float& value, float player, float offset) {
		// calculate control_points (bessel_curve).
		ImVec2 DrawPoint0(point0.x - offset, point0.y);
		ImVec2 DrawPoint1(point1.x - offset, point1.y);

		ImVec2 MidPoint = IMVEC2_MUL1(IMVEC2_ADD2(DrawPoint0, DrawPoint1), 0.5f);
		ImVec2 ControlPointA = IMVEC2_SUB2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(DrawPoint1, MidPoint).y));
		ImVec2 ControlPointB = IMVEC2_ADD2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(DrawPoint1, MidPoint).y));

		ImVec2 LinePointTemp = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, 0.0f);
		for (int i = 0; i < sample; ++i) {

			ImVec2 DrawPoint = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, (float)i / (float)sample);
			ImExtBaseControl::ExtDrawLine(LinePointTemp, DrawPoint, color, EditorLinesWidthScale * 2.0f);
			LinePointTemp = DrawPoint;
		}
		if (DrawPoint0.x + offset < player && DrawPoint1.x + offset > player) {
			value = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, float((int)player % 100) / (DrawPoint1.x - DrawPoint0.x)).y;
			return true;
		}
		else {
			value = 0.0f;
			return false;
		}
	}

	void RenderSamplePoint(const ImVec2& position, float size, const ImVec4& color, float& value) {
		ImExtBaseControl::ExtDrawCircleFill(position, size, color);

		ImGui::SetCursorPos(ImVec2(position.x - size, position.y - size));
		ImGui::InvisibleButton("@SETVALUE", ImVec2(size * 2.0f, size * 2.0f));

		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0) && !NodeSetValuePointer && !PlayerSetValueFlag)
			NodeSetValuePointer = &value;
	}

	void RenderPlayerLine(ImVec2& position, float offset, float max, const ImVec4& color) {
		float PlayerRectWidth = IMGUI_ITEM_SPC * 3.2f;
		float PlayerLineWidth = 2.2f;

		ImExtBaseControl::ExtDrawLine(
			ImVec2(position.y - offset - PlayerLineWidth * 0.5f, IMGUI_ITEM_SPC + PlayerRectWidth * 0.5f),
			ImVec2(position.y - offset - PlayerLineWidth * 0.5f, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
			color,
			EditorLinesWidthScale * 2.2f
		);
		ImExtBaseControl::ExtDrawLine(
			ImVec2(position.y - offset + IMGUI_ITEM_SPC, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
			ImVec2(position.y - offset - IMGUI_ITEM_SPC, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
			color,
			EditorLinesWidthScale * 2.2f
		);

		ImExtBaseControl::ExtDrawRectangleFill(
			ImVec2(position.y - offset - PlayerRectWidth * 0.5f, IMGUI_ITEM_SPC),
			ImVec2(PlayerRectWidth, IMGUI_ITEM_SPC * 2.0f),
			color
		);
		ImGui::SetCursorPos(ImVec2(position.y - offset - PlayerRectWidth * 0.5f, IMGUI_ITEM_SPC));
		ImGui::InvisibleButton("@SETPLAYER", ImVec2(PlayerRectWidth, IMGUI_ITEM_SPC * 2.0f));

		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0) && !NodeSetValuePointer)
			PlayerSetValueFlag = true;
		if (PlayerSetValueFlag && !ImGui::IsMouseDown(0))
			PlayerSetValueFlag = false;
		if (PlayerSetValueFlag)
			position.x += ImGui::GetIO().MouseDelta.x;

		position.x = position.x < 0.0f ? 0.0f : position.x;
		position.x = position.x > max ? max : position.x;
	}

public:
	bool PlayerRunSample(AnimGenCoord& CoordParam) {
		CoordParam = PlayerSample;
		return PlayerPlayFlag;
	};

	std::vector<AnimGenCoord> GenerateBakedBezierCurve() {
		std::vector<AnimGenCoord> GenerateTemp = {};

		for (size_t i = 0; i < NodesDataIdx->size() - 1; ++i) {
			AnimCoordSample DatTempA = (*NodesDataIdx)[i];
			AnimCoordSample DatTempB = (*NodesDataIdx)[i + 1];

			for (uint32_t smp = 0; smp < (*NodesDataIdx)[i].GenerateSample; ++smp) {
				AnimGenCoord CoordTemp = {};

				for (size_t j = 0; j < 3; ++j) {
					ImVec2 DrawPoint0(DatTempA.TimePosition, DatTempA.AnimSamplePoints[j]);
					ImVec2 DrawPoint1(DatTempB.TimePosition, DatTempB.AnimSamplePoints[j]);

					ImVec2 MidPoint = IMVEC2_MUL1(IMVEC2_ADD2(DrawPoint0, DrawPoint1), 0.5f);
					ImVec2 ControlPointA = IMVEC2_SUB2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(DrawPoint1, MidPoint).y));
					ImVec2 ControlPointB = IMVEC2_ADD2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(DrawPoint1, MidPoint).y));

					ImVec2 DrawPoint = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, (float)smp / (float)DatTempA.GenerateSample);
					CoordTemp.AnimGenVector[j] = DrawPoint.x;
				}
				GenerateTemp.push_back(CoordTemp);
			}
		}
		return GenerateTemp;
	}

	void DrawEditorWindow(
		uint32_t                      unqiue_id,
		const char*                   name,
		float                         track_length,
		float                         player_step,
		std::vector<AnimCoordSample>& sample,
		bool                          fixed_window = false,
		bool*                         p_open       = (bool*)0,
		ImGuiWindowFlags              flags        = 0
	) {
		const float SpaceLeft = 160.0f;
		const float SpaceBelow = 32.0f;
		NodesDataIdx = &sample;

		ImGuiWindowFlags WindowFlags = 0;
		if (fixed_window)
			WindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		ImGui::PushID(unqiue_id);
		ImGui::Begin(name, p_open, WindowFlags | flags);

		ImVec2 DarwChildSize = ImVec2(ImGui::GetWindowSize().x - SpaceLeft, ImGui::GetWindowSize().y - SpaceBelow);

		ImGui::SetCursorPos(ImVec2(SpaceLeft, ImGui::GetWindowSize().y - SpaceBelow - IMGUI_ITEM_SPC * 0.5f));
		ImGui::SetNextItemWidth(DarwChildSize.x - ImGui::CalcTextSize("TRACK").x - IMGUI_ITEM_SPC * 2.0f);

		TrackWindowXpos.y = track_length - DarwChildSize.x * 0.85f;
		TrackWindowXpos.y = TrackWindowXpos.y < 0.0f ? 0.0f : TrackWindowXpos.y;

		// frame background color.
		ImGui::PushStyleColor(ImGuiCol_FrameBg, EditorTickLinesColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, EditorTickLinesColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, EditorTickLinesColor);
		// block color.
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImExtBaseControl::ExtSubColorGrayscale(EditorTickLinesColor, -0.32f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImExtBaseControl::ExtSubColorGrayscale(EditorTickLinesColor, -0.32f));
		{
			ImGui::SliderFloat("TRACK", &TrackWindowXpos.x, 0.0f, TrackWindowXpos.y);
		}
		ImGui::PopStyleColor(5);

		ImGui::SetCursorPos(ImVec2(SpaceLeft, IMGUI_ITEM_SPC * 4.32f));
		// draw animation_track editor window.
		ImGui::BeginChild("@TIMETRACK", ImVec2(DarwChildSize.x - IMGUI_ITEM_SPC, DarwChildSize.y - IMGUI_ITEM_SPC * 5.5f), true);

		float MedianY = ImGui::GetWindowHeight() * 0.5f;
		TrackXpos += (TrackWindowXpos.x - IMGUI_ITEM_SPC * 4.0f - TrackXpos) * 0.1f;

		for (float i = 0.0f; i <= track_length; i += 50.0f) {
			ImExtBaseControl::ExtDrawLine(
				ImVec2(i - TrackXpos, IMGUI_ITEM_SPC),
				ImVec2(i - TrackXpos, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
				EditorTickLinesColor,
				EditorLinesWidthScale
			);
		}

		for (float i = 0.0f; i <= track_length; i += 100.0f) {
			char TimeTickText[32] = {};
			sprintf_s(TimeTickText, "%.0f", i);

			// draw time_tick text.
			ImExtBaseControl::ExtDrawText(
				ImVec2(i - TrackXpos - ImGui::CalcTextSize(TimeTickText).x * 0.5f, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 3.5f),
				EditorTickLinesColor, "%.0f", i
			);

			ImExtBaseControl::ExtDrawLine(
				ImVec2(i - TrackXpos, IMGUI_ITEM_SPC),
				ImVec2(i - TrackXpos, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
				EditorTickLinesColor,
				EditorLinesWidthScale * 2.0f
			);
		}

		// draw centerline[value: 0.0f].
		ImExtBaseControl::ExtDrawLine(
			ImVec2(0.0f, MedianY),
			ImVec2(ImGui::GetWindowWidth(), MedianY),
			EditorTickLinesColor,
			EditorLinesWidthScale * 1.8f
		);

		const ImVec4 AsixColors[3] = {
			ImVec4(1.0f, 0.2f, 0.2f, 1.0f), ImVec4(0.2f, 1.0f, 0.2f, 1.0f), ImVec4(0.5f, 0.5f, 1.0f, 1.0f)
		};

		for (size_t i = 0; i < sample.size() - 1; ++i) {
			size_t PointCount = size_t(sample.size() >= 2);
			float SamplePointValue[3] = {};

			for (size_t j = 0; j < 3; ++j) {
				if (RenderCubicBezierCurve(
					ImVec2(sample[i].TimePosition, (MedianY - sample[i].AnimSamplePoints[j] * TrackYscale)),
					ImVec2(sample[i + PointCount].TimePosition, (MedianY - sample[i + PointCount].AnimSamplePoints[j] * TrackYscale)),
					AsixColors[j], sample[i].PlaySample, SamplePointValue[j], PlayerPositionX.y, TrackXpos
				))
					PlayerSample.AnimGenVector[j] = MedianY - SamplePointValue[j];

				RenderSamplePoint(
					ImVec2(sample[i].TimePosition - TrackXpos, MedianY - sample[i].AnimSamplePoints[j] * TrackYscale),
					4.2f,
					AsixColors[j],
					sample[i].AnimSamplePoints[j]
				);
			}
		}
		size_t LastPoint = sample.size() - 1;
		for (size_t j = 0; j < 3; ++j)
			RenderSamplePoint(
				ImVec2(sample[LastPoint].TimePosition - TrackXpos, MedianY - sample[LastPoint].AnimSamplePoints[j] * TrackYscale),
				4.2f,
				AsixColors[j],
				sample[LastPoint].AnimSamplePoints[j]
			);

		MouseSetNodeValue();
		RenderPlayerLine(PlayerPositionX, TrackXpos, track_length, EditorPlayerColor);

		ImGui::EndChild();

		PlayerPositionX.y += (PlayerPositionX.x - PlayerPositionX.y) * 0.12f;
		PlayerPositionX.y = abs(PlayerPositionX.x - PlayerPositionX.y) < 0.01f ? PlayerPositionX.x : PlayerPositionX.y;

		ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC * 4.32f));
		ImGui::Text("TICK: %.1f", PlayerPositionX.y);
		ImGui::Spacing();

		float ButtonWidth = (SpaceLeft - IMGUI_ITEM_SPC * 3.0f) * 0.5f;
		if (ImGui::Button("Begin", ImVec2(ButtonWidth, 0.0f))) {
			PlayerPositionX.x = 0.0f;
			PlayerPositionX.y = 0.0f;
		}
		ImGui::SameLine();
		if (ImGui::Button("End", ImVec2(ButtonWidth, 0.0f))) {
			PlayerPositionX.x = track_length;
			PlayerPositionX.y = track_length;
		}
		ImGui::Spacing();

		float SpaceLeftItem = SpaceLeft - IMGUI_ITEM_SPC * 2.0f;
		const char* PlayerButtonName = {};
		if (PlayerPlayFlag) {
			PlayerPositionX.x += player_step;
			PlayerButtonName = "STOP";
		}
		else
			PlayerButtonName = "PLAY";
		if (ImGui::Button(PlayerButtonName, ImVec2(SpaceLeftItem, 0.0f)))
			PlayerPlayFlag = !PlayerPlayFlag;

		ImGui::Spacing();
		if (ImGui::Button("RE", ImVec2(32.0f, 0.0f)))
			TrackYscale = 1.0f;
		ImGui::SameLine();
		ImGui::SetNextItemWidth(SpaceLeftItem - 32.0f - IMGUI_ITEM_SPC);
		ImGui::SliderFloat("##YSCALE", &TrackYscale, 0.05f, 2.0f);

		ImGui::Spacing();
		ImGui::SetNextItemWidth(SpaceLeftItem);
		ImGui::InputFloat("##SETPOS", &PlayerPositionX.x, 0.0f, 0.0f, "%.1f");

		SpacingLine(SpaceLeft, EditorTickLinesColor);

		ImGui::TextColored(AsixColors[0], "Xaxis: %.2f", PlayerSample.AnimGenVector[0]);
		ImGui::TextColored(AsixColors[1], "Yaxis: %.2f", PlayerSample.AnimGenVector[1]);
		ImGui::TextColored(AsixColors[2], "Zaxis: %.2f", PlayerSample.AnimGenVector[2]);

		SpacingLine(SpaceLeft, EditorTickLinesColor);

		ImGui::Text("Nodes: %u", sample.size());
		size_t BakedDataSize = NULL;
		for (const auto& item : sample)
			BakedDataSize += item.GenerateSample;
		ImGui::Text("Baked: %u", BakedDataSize);

		ImGui::End();
		ImGui::PopID();
	}
};

#endif