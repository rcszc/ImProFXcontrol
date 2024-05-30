// improfx_control_shortcut. RCSZ. [20240530]
// ImGui: [Window(Begin_End)], Shortcut Key Editor, Update: 20240530.
#include "improfx_control.h"
#include <iostream>
using namespace std;

namespace IMFXC_WIN {
	constexpr const char* MainKeys[] = { 
		"L-CTRL", "L-SHIFT", "L-ALT", "R-CTRL", "R-SHIFT", "R-ALT"
	};
	constexpr ImGuiKey    MainKeysFlags[] = { 
		ImGuiKey_LeftCtrl,  ImGuiKey_LeftShift,  ImGuiKey_LeftAlt,
		ImGuiKey_RightCtrl, ImGuiKey_RightShift, ImGuiKey_RightAlt
	};

	ImGuiKey ShortcutKeyEditorWindow::PressWhichKey(bool repeat) {
		for (int It = ImGuiKey_NamedKey_BEGIN; It < ImGuiKey_NamedKey_END; ++It) {
			// ignore mouse keys, get this_key.
			if (It >= ImGuiKey_MouseLeft && It <= ImGuiKey_MouseWheelY) continue;
			if (ImGui::IsKeyPressed((ImGuiKey)It, repeat)) return (ImGuiKey)It;
		}
		return ImGuiKey_None;
	}

	bool ShortcutKeyEditorWindow::IfCombinationKey(const std::vector<ImGuiKey>& keys) {
		bool ResultKeysFlag = true;
		if (keys.size() == CombinationKey.size() && keys.size() > 1) {
			for (size_t i = 0; i < keys.size(); ++i)
				if (keys[i] != CombinationKey[i])
					ResultKeysFlag = false;
			return ResultKeysFlag;
		}
		return false;
	}

	int32_t ShortcutKeyEditorWindow::UpdateShortcutKey() {
		int32_t ReturnCountID = -1;
		bool MainKeyDownFlag = false;

		for (size_t i = 0; i < 6; ++i)
			if (ImGui::IsKeyPressed((ImGuiKey)MainKeysFlags[i], false))
				MainKeyDownFlag |= true;

		if (MainKeyDownFlag && !CombinationKeyFlag) {
			CombinationKeyFlag  = true;
			CombinationKeyTimer = std::chrono::steady_clock::now();
		}

		if (CombinationKeyFlag) {
			ImGuiKey KeyTemp = PressWhichKey(false);
			if (KeyTemp != ImGuiKey_None) {
				CombinationKey.push_back(KeyTemp);
				CombinationKeyTimer = std::chrono::steady_clock::now();
			}
		}
		if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - CombinationKeyTimer).count() >
			CombinationKeyDuration
		) {
			for (size_t i = 0; i < ShortcutKeys.size(); ++i) {
				if (IfCombinationKey(ShortcutKeys[i].KeyCombination)) {
					LAST_HIT = (int32_t)i;
					ReturnCountID = (int32_t)i;
					continue;
				}
			}
			CombinationKeyFlag = false;
			CombinationKey.clear();
		}
		return ReturnCountID;
	}

	void ShortcutKeyEditorWindow::DrawEditorWindow(
		uint32_t         unqiue_id,
		const char*      name,
		float            interval,
		bool             fixed_window,
		bool*            p_open,
		ImGuiWindowFlags flags
	) {
		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
		if (fixed_window)
			WindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		ImGui::PushID(unqiue_id);

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.62f));

		ImGui::PushStyleColor(ImGuiCol_FrameBg,        IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.5f));
		// button color.
		ImGui::PushStyleColor(ImGuiCol_Button,        IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.2f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  EditorColorSystem);

		CombinationKeyDuration = int64_t(interval * 1000.0f);
		ImGui::Begin(name, p_open, WindowFlags | flags);
		{
			ImGui::BeginChild("@DEBUG_IFNO", ImVec2(ImGui::GetWindowSize().x - IMGUI_ITEM_SPAC * 2.0f, 76.0f));

			ImGui::Spacing();
			ImGui::Text(" RealTimeStatus: ");
			for (auto& Item : CombinationKey) {
				ImGui::SameLine();
				std::string KeyName = ImGui::GetKeyName(Item);
				ImGui::SetNextItemWidth(ImGui::CalcTextSize(KeyName.c_str()).x + IMGUI_ITEM_SPAC);
				// dis input state.
				ImGui::BeginDisabled(true);
				ImGui::InputText("##DEBUGKEY", KeyName.data(), KeyName.size());
				ImGui::EndDisabled();
			}

			ImGui::SetCursorPosY(36.0f);
			ImGui::Separator();
			ImGui::Spacing();
			if (CombinationKeyFlag) ImGui::Text(" STATUS: INPUT");
			else                    ImGui::Text(" STATUS: NULL ");
			ImGui::SameLine();
			ImGui::Text("KeyTimer: %lld ms, Hit: %d ", CombinationKeyDuration, LAST_HIT);

			ImGui::SameLine();
			if (ImGui::Button("Clear Last Hit", ImVec2(160.0f, 0.0f))) LAST_HIT = -1;
			ImGui::EndChild();

			size_t IDCount = NULL;
			for (auto& Item : ShortcutKeys) {
				// hit debug background color.
				if (IDCount == LAST_HIT)
					ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_CONTROL_BASE::ColorBrightnesScale(EditorColorSystem, 0.72f));

				ImGui::BeginChild(std::to_string(IDCount).c_str(), ImVec2(ImGui::GetWindowSize().x - IMGUI_ITEM_SPAC * 2.0f, 72.0f), true);
				ImGui::Spacing();
				std::string TextTemp = "Mode: ";

				ImGui::Text(" NAME:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				ImGui::InputText("##NAME", Item.KeyShortcutName.data(), KeyNameLength);

				ImGui::SameLine(); ImGui::Text(" KEYS:");
				size_t ButtonID = NULL;
				for (size_t i = 0; i < Item.KeyCombination.size(); ++i) {
					ImGui::SameLine();
					std::string KeyTextTemp = ImGui::GetKeyName(Item.KeyCombination[i]);
					TextTemp += KeyTextTemp + " + ";

					ImGui::SetNextItemWidth(ImGui::CalcTextSize(KeyTextTemp.c_str()).x + IMGUI_ITEM_SPAC);
					// dis input state.
					ImGui::BeginDisabled(true);
					ImGui::InputText("##KEY", KeyTextTemp.data(), KeyTextTemp.size());
					ImGui::EndDisabled();

					ImGui::PushID((int)ButtonID);
					ImGui::SameLine();
					if (i == 0) {
						ImGui::SetNextItemWidth(
							ImGui::CalcTextSize(ImGui::GetKeyName(MainKeysFlags[Item.MainKeyIndex])).x + IMGUI_ITEM_SPAC * 2.0f
						);
						// main_key(header_key).
						ImGui::Combo("##MAINKEY", &Item.MainKeyIndex, MainKeys, IM_ARRAYSIZE(MainKeys));
						Item.KeyCombination[i] = MainKeysFlags[Item.MainKeyIndex];
						continue;
					}
					ImGui::Button("SET");
					if (ImGui::IsItemActive()) {
						ImGuiKey FlagTemp = PressWhichKey();
						if (FlagTemp != ImGuiKey_None) Item.KeyCombination[i] = FlagTemp;
					}
				}
				ImGui::PopID();
				++ButtonID;

				ImGui::SameLine();
				if (ImGui::Button("+"))
					Item.KeyCombination.push_back(ImGuiKey_None);
				ImGui::SameLine();
				if (ImGui::Button("-") && !Item.KeyCombination.empty())
					Item.KeyCombination.pop_back();

				TextTemp.pop_back(); TextTemp.pop_back();
				ImGui::Text(" ID: %u, %s Number: %u", IDCount, TextTemp.c_str(), Item.KeyCombination.size());

				ImGui::EndChild();
				if (IDCount == LAST_HIT) ImGui::PopStyleColor();
				++IDCount;
			}

			if (ImGui::Button("ShortcutKey[+]", ImVec2(160.0f, 32.0f))) {
				ShortcutKeyInfo CreateShoryKey = {};
				CreateShoryKey.KeyShortcutName.resize(KeyNameLength);
				ShortcutKeys.push_back(CreateShoryKey);
			}

			ImGui::SameLine();
			if (ImGui::Button("ShortcutKey[-]", ImVec2(160.0f, 32.0f)) && !ShortcutKeys.empty())
				ShortcutKeys.pop_back();
		}
		ImGui::End();
		ImGui::PopStyleColor(7);
	}
}