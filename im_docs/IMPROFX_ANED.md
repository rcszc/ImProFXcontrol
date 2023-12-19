# Animation Nodes Editor

> improfx_control_src/improfx_control_animeditor.hpp

```RCSZ```
- [x] Update: 2023.12.20

ImGui扩展简单的动画节点编辑器, 类型:完整窗口(内绘制Begin-End).

- 标准库: ```vector``` ```cstdio```
- ImGui: ```imgui_glfw/imgui.h``` ```imgui_glfw/imgui_internal.h```
> 使用时记得前往 improfx_control_base.hpp 把imgui替换为自己的路径 ~

__类定义:__
```cpp
class AnimNodesEditorWindow;
```

__数据结构:__
```cpp
// 输入引用动画采样点.
struct AnimCoordSample {
	float AnimSamplePoints[3]; // 三维坐标采样点, 0:x 1:y 2:z
	float TimePosition;        // 采样点时间位置(虚拟Tick)

	uint32_t PlaySample;       // 播放(视图)绘制曲线节点采样
	uint32_t GenerateSample;   // 烘焙曲线节点采样
};

// 输出烘焙曲线变换数据.
struct AnimGenCoord {
	float AnimGenVector[3]; // 三维坐标点, 0:x 1:y 2:z
};

```

__调用成员:__
```cpp
// 绘制编辑器.
void DrawEditorWindow(
	uint32_t                      unqiue_id,               // ImGui帧内唯一ID
	const char*                   name,                    // 窗口名称
	float                         track_length,            // 动画轨道长度
	float                         player_step,             // 播放动画步长(每帧)
	std::vector<AnimCoordSample>& sample,                  // 输入动画采样点
	bool                          fixed_window = false,    // 固定窗口(大小&位置)
	bool*                         p_open       = (bool*)0, // 与ImGui::Begin的"p_open"参数相同
	ImGuiWindowFlags              flags        = 0         // 与ImGui::Begin的"flags"参数相同
);

// 获取播放器运行时变换位置.
// @param  "CoordParam" 获取位置
// @return 播放状态, true:播放中, false:停止播放
bool PlayerRunSample(AnimGenCoord& CoordParam);

// 生成烘焙的贝塞尔曲线点.
// 采样根据以上讲解"AnimCoordSample"的"GenerateSample".
std::vector<AnimGenCoord> GenerateBakedBezierCurve();
```

---

```END```