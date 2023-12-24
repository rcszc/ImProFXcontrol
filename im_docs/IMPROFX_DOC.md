# ImProFXcontrol

> improfx_control_src/improfx_control.h

```RCSZ```
- [x] __Update:__ 2023.12.24
---

- 标准库: ```string``` ```vector``` ```cstdio```
- ImGui: ```imgui_glfw/imgui.h``` ```imgui_glfw/imgui_internal.h```
  
> 使用时记得前往 improfx_control_base.hpp 把imgui替换为自己的路径, 然后引入 improfx_control.h 即可 ~

__说明:__ 这些控件全部是 ImProFX框架 同步开发的, 只不过它们可以不依赖框架只依赖ImGui, 轻松的集成到自己的项目中 ~

| Control | Time | Developers | 
| :---: | :---: | :---:
| SmoothMenuChildWindow | 2023.12.21 | RCSZ |
| AnimNodesEditorWindow | 2023.12.21 | RCSZ |

### 平滑(纵向)菜单
> - 子窗口: 函数内会调用 ImGui::BeginChild - ImGui::EndChild
> - Update: 2023.12.24 修改类名,优化样式

__类定义:__
```cpp
class SmoothMenuChildWindow;
```

__调用成员:__
```cpp
// 绘制菜单.
void DrawMenuWindow(
	const char*                     name,                                          // 菜单子窗口名称
	const std::vector<std::string>& items,                                         // 菜单选项
	uint32_t&                       count,                                         // 菜单选项位置(索引)
	const ImVec4&                   color     = ImVec4(0.55f, 0.12f, 1.0f, 0.72f), // 菜单色系
	const ImVec2&                   size      = ImVec2(256.0f, 384.0f),            // 菜单子窗口大小
	float                           speed     = 1.0f,                              // 动画速度缩放 
	float                           textscale = 1.2f                               // 菜单子窗口内字体缩放
);
```

---

### 动画(节点)编辑器
> - 窗口: 函数内会调用 ImGui::Begin - ImGui::End
> - Update: 2023.12.24 新增俯仰轴,偏航轴,横滚轴

__类定义:__
```cpp
class AnimNodesEditorWindow;
```

__数据结构:__
```cpp
#define ANE_COORD_PARAM 6 
// 输入引用动画采样点.
struct AnimCoordSample {
	// 0:x轴, 1:y轴, 2:z轴, 3:pitch俯仰轴, 4:yaw偏航轴, 5:roll横滚轴
	float AnimSamplePoints[ANE_COORD_PARAM]; // 动画坐标采样点
	float TimePosition;                      // 采样点时间位置(虚拟Tick)

	uint32_t PlayerSamplingRate; // 播放(视图)绘制曲线节点采样率
	uint32_t BakeSamplingRate;   // 烘焙曲线节点采样率
};

// 输出烘焙曲线变换数据.
struct AnimGenCoord {
	// 0:x轴, 1:y轴, 2:z轴, 3:pitch俯仰轴, 4:yaw偏航轴, 5:roll横滚轴
	float AnimGenVector[ANE_COORD_PARAM]; // 输出采样点
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
// 采样根据以上讲解"AnimCoordSample"的"BakeSamplingRate".
std::vector<AnimGenCoord> GenerateBakedBezierCurve();

float TrackWidthValueScale = 1.0f;  // 动画轨道宽度缩放
float TrackHeightValueScale = 1.0f; // 动画轨道高度缩放
```

---

```END```