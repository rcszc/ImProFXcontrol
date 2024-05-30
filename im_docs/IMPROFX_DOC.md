# ImProFXcontrol

> improfx_control_src/improfx_control.h

```RCSZ```
- [x] __Update:__ 2024.05.30
---

- 标准库: ```string``` ```vector``` ```cstdio``` ```chrono``` ```functional```
- ImGui: ```imgui_glfw/imgui.h``` ```imgui_glfw/imgui_internal.h```
  
> 使用时记得前往 improfx_control_base.h 把imgui替换为自己的路径, 然后引入 improfx_control.h 即可 ~

__说明:__ ImProFXcontrol 是独立于ImProFX框架的ImGui自制控件, 只依赖于ImGui核心和标准库, 可以容易的集成到自己的C++ImGui项目中.

| Control | Time | Developers | Status |
| :---: | :---: | :---: | :---: | 
| SmoothMenuChildWindow | 2023.12.21 | RCSZ | √
| AnimAxisEditorWindow | 2023.12.21 | RCSZ | √
| FlatEditorWindow | 2023.12.25 | RCSZ | √
| DashboardChildWindow | 2024.02.18 | RCSZ | x
| ShortcutKeyEditorWindow | 2024.05.30 | RCSZ | √

---

## 平滑(纵向)菜单
> - 子窗口: 函数内会调用 ImGui::BeginChild - ImGui::EndChild
> - Update: 2023.12.24 修改类名,优化样式

__类定义:__
```cpp
class IMFXC_CWIN::SmoothMenuChildWindow;
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
	float                           text_scale = 1.2f                               // 菜单子窗口内字体缩放
);
```

---

## 动画轴编辑器
> - 窗口: 函数内会调用 ImGui::Begin - ImGui::End
> - Update: 2024.04.21 修复了许多Bug, 每个按钮引入渐变动画
__类定义:__
```cpp
class IMFXC_WIN::AnimAxisEditorWindow;
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

// 编辑器色系, 所有控件的颜色会围绕这个颜色进行变换
ImVec4 EditorColorSystem = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
```

---

## 2D平面编辑器
> - 窗口: 函数内会调用 ImGui::Begin - ImGui::End
> - Update: 2024.02.18 修改类名, 修复一些bug

__类定义:__
```cpp
class IMFXC_WIN::FlatEditorWindow;
```

__数据结构:__
```cpp
// 函数对象传入坐标系信息.
struct CoordSystemInfo {
	ImVec2 CenterPosition; // 虚拟坐标系网格中心位置坐标
	ImVec2 MouseCoordPos;  // 虚拟坐标系虚拟鼠标坐标(当前鼠标在网格上的位置)

	ImVec2 SelectionBoxPoint0; // 选择框矩形对角点A(网格虚拟坐标系)
	ImVec2 SelectionBoxPoint1; // 选择框矩形对角点B(网格虚拟坐标系)

	float ScaleCoord; // 虚拟坐标系缩放
}
```

__调用成员:__
```cpp
// 绘制编辑器.
void DrawEditorWindow(
	uint32_t                             unqiue_id,  // ImGui帧内唯一ID
	const char*                          name,       // 窗口名称
	std::function<void(CoordSystemInfo)> draw,       // 坐标系子窗口绘制回调函数
	ImTextureID                          poswin_image  = nullptr,                  // 工具定位窗口背景图片
	bool                                 fixed_window  = true,                     // 固定窗口(大小&位置)
	float                                speed         = 1.0f,                     // 动画速度缩放
	const ImVec2&                        coord_size    = ImVec2(1200.0f, 1200.0f), // 虚拟坐标系大小
	const ImVec2&                        coord_winsize = ImVec2(640.0f, 640.0f)    // 坐标系子窗口大小
	bool*                                p_open        = (bool*)0,                 // 与ImGui::Begin的"p_open"参数相同
	ImGuiWindowFlags                     flags         = 0                         // 与ImGui::Begin的"flags"参数相同
);
// 注: 在编辑器内会统一使用中心坐标, 就是说 size = [1000,1000] 那么 limit = [-500 ~ +500, -500 ~ +500]

// 工具栏子窗口绘制回调函数
std::function<void()> EditorToolbar = []() {};

// 编辑器色系, 所有控件的颜色会围绕这个颜色进行变换
ImVec4 EditorColorSystem = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
```

__简单例子(坐标计算):__
```cpp
// 在编辑器坐标系中绘制中心点居中图片:
auto DemoFunc = [&](CoordSystemInfo INFO) {

	// 绘制图片大小
	ImVec2 RenderSize(1000.0f, 1000.0f);
	// 绘制图片位置
	ImVec2 ImgPos(INFO.CenterPosition.x - RenderSize.x * 0.5f * INFO.ScaleCoord, INFO.CenterPosition.y - RenderSize.y * 0.5f * INFO.ScaleCoord);
		
	// 绘制图片(或者其他控件也用同样方法计算)...
};
```

---

## 模拟仪表盘 
### [warning]: 已被列入弃用控件
__可以在引入头时定义"ENABLE_OLD_CONTROL_DASHBOAR"启用__

> - 子窗口: 函数内会调用 ImGui::BeginChild - ImGui::EndChild
> - Update: 2024.04.21 弃用停止维护控件

__类定义:__
```cpp
class IMFXC_CWIN::DashboardChildWindow;
```

__调用成员:__
```cpp
// 绘制仪表盘.
bool DrawDashboardWindow(
	const char*   name,                                    // 仪表盘窗口名称
	float         value,                                   // 输入值
	uint32_t      ruler = 10,                              // 主刻度数量
	const ImVec4& color = ImVec4(0.0f, 1.0f, 1.0f, 0.92f), // 仪表色系
	const ImVec2& size  = ImVec2(768.0f, 768.0f),          // 仪表子窗口大小
	float         speed = 1.0f                             // 动画速度缩放
);

// 设置仪表显示值范围(返回设置后的limit).
ImVec2 SetDashboardValueLimit(const ImVec2& limit);

// 仪表启动标志(启动动画)
bool DashboardStart = false;
```

---

## 组合快捷键编辑器
> - 窗口: 函数内会调用 ImGui::Begin - ImGui::End
> - Update: 2024.05.30

__类定义:__
```cpp
class IMFXC_WIN::ShortcutKeyEditorWindow;
```

__数据结构:__
```cpp
// 快捷键的所有主键, 也是头键:
// 0: ImGuiKey_LeftCtrl, 1: ImGuiKey_LeftShift, 2: ImGuiKey_LeftAlt,
// 3: ImGuiKey_RightCtrl, 4: ImGuiKey_RightShift, 5: ImGuiKey_RightAlt
struct ShortcutKeyInfo {
	int MainKeyIndex; // 快捷键主键索引
	std::string           KeyShortcutName; // 快捷键名称
	std::vector<ImGuiKey> KeyCombination;  // 快捷键组合
};
```

__调用成员:__
```cpp
// 获取快捷键源数据指针
std::vector<ShortcutKeyInfo>* GetSourceData();

// 上一次命中索引(ID).
int32_t LAST_HIT = -1;
// 更新快捷键(运行判断系统), 快捷键判定可以在不绘制窗口情况下运行
int32_t UpdateShortcutKey();

void DrawEditorWindow(
	uint32_t         unqiue_id,               // ImGui帧内唯一ID
	const char*      name,                    // 窗口名称
	float            interval,                // 快捷键组合按键时间间隔, 单位:秒(s), 精度:毫秒(ms)
	bool             fixed_window = false,    // 固定窗口(大小&位置)
	bool*            p_open       = (bool*)0, // 与ImGui::Begin的"p_open"参数相同
	ImGuiWindowFlags flags        = NULL      // 与ImGui::Begin的"flags"参数相同
);

// 组合键命名最大字符串长度
size_t KeyNameLength = 256;

// 编辑器色系, 所有控件的颜色会围绕这个颜色进行变换
ImVec4 EditorColorSystem = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
```

__界面操作:__ "+"添加键, "-"删除键, 使用鼠标左键长按"SET"按钮同时按下键盘想设置的键.

---

```END```