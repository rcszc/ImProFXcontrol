// improfx_control. RCSZ.
// ImProFX Framework ImGui-ExtControl
// Update: 2023.12.21

#ifndef _IMPROFX_CONTROL_H
#define _IMPROFX_CONTROL_H
#include "improfx_control_base.h"

// ######################################## AnimMenuChildWindow ########################################
// [ChildWindow]: ¶¯»­×ÝÏò²Ëµ¥. 2023_12_16 RCSZ.
class AnimMenuChildWindow {
protected:
	float TextDrawHeight = 0.0f;

	ImVec2 AnimBufferPosyType  = {};
	ImVec2 AnimBufferWidthType = {};

	ImVec2 AnimBufferPosyItem  = {};
	ImVec2 AnimBufferWidthItem = {};

	void DrawMenuTypeRect(float rect_height, const ImVec4& color);
	void DrawMenuItemRect(float rect_height, const ImVec4& color);
	void MenuInterCalc(ImVec2& posy_calc, ImVec2& width_calc, float speed);

public:
	void DrawMenuWindow(
		const char*                     name,
		const std::vector<std::string>& items,
		uint32_t&                       count,
		const ImVec4&                   color     = ImVec4(0.55f, 0.12f, 1.0f, 0.72f),
		const ImVec2&                   size      = ImVec2(256.0f, 384.0f),
		float                           speed     = 1.0f,      
		float                           textscale = 1.2f
	);
};

// ######################################## AnimTrackEditorWindow ########################################
// animation point coordinates
struct AnimCoordSample {
	float AnimSamplePoints[3];
	float TimePosition;

	uint32_t PlayerSamplingRate;
	uint32_t BakeSamplingRate;

	AnimCoordSample() : AnimSamplePoints{}, TimePosition(0.0f), PlayerSamplingRate(128), BakeSamplingRate(256) {}
	AnimCoordSample(float x, float y, float z, float t, uint32_t ps = 128, uint32_t gs = 256) :
		AnimSamplePoints{ x, y, z }, TimePosition(t), PlayerSamplingRate(ps), BakeSamplingRate(gs)
	{}
};

struct AnimGenCoord {
	float AnimGenVector[3];
};

// [Window]: ¶¯»­(¹ìµÀ)±à¼­Æ÷. 2023_12_19 RCSZ.
class AnimTrackEditorWindow {
protected:
	ImVec4 EditorColorPlayer    = ImVec4(0.0f, 0.72f, 0.72f, 1.0f);
	ImVec4 EditorColorTimeLines = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);

	float EdirorScaleTrack      = 1.0f;
	float EditorScaleLinesWidth = 1.0f;

	ImVec2 TrackWindowXpos = {};   // x:pos, y:max.
	float  TrackXpos       = 0.0f; // tick inter_smooth.

	float* NodeSetValuePointer = nullptr;

	ImVec2 PlayerLineXpos   = {}; // x:pos, y:smooth
	bool   PlayerSetposFlag = false;
	bool   PlayerPlayFlag   = false;

	float TrackValueScale = 1.0f;

	AnimGenCoord PlayerRunCoord = {};
	std::vector<AnimCoordSample>* AnimDataIndex = nullptr;

	void MouseSetPointValue();
	void SpacingLine(float space_left, const ImVec4& color);

	bool RenderCubicBezierCurve(
		const ImVec2& point0, const ImVec2& point1, const ImVec4& color, 
		float offset, int sample, float& value, float playerpos
	);
	void RenderAnimationPoints(const ImVec2& position, float size, const ImVec4& color, float& value);
	void RenderPlayerLine(ImVec2& position, float offset, float max, const ImVec4& color);

public:
	bool PlayerRunSample(AnimGenCoord& CoordParam);
	std::vector<AnimGenCoord> GenerateBakedBezierCurve();

	void DrawEditorWindow(
		uint32_t                      unqiue_id,
		const char*                   name,
		float                         track_length,
		float                         player_step,
		std::vector<AnimCoordSample>& sample,
		bool                          fixed_window = false,
		bool*                         p_open       = (bool*)0,
		ImGuiWindowFlags              flags        = 0
	);
};

#endif