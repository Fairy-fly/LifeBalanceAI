# 项目结构说明

LifeBalanceAI 以 Qt Widgets 应用为主体，按界面组件、业务服务、资源文件、Android 包配置和测试代码组织。

## 根目录

```text
LifeBalanceAI/
|-- LifeBalanceAI_demo/     # 主 Qt/C++ 应用
|-- docs/                   # 项目文档
|-- screenshots/            # 后续补充的展示截图
|-- .gitignore
`-- README.md
```

## 应用目录

```text
LifeBalanceAI_demo/
|-- android/
|-- models/
|-- resources/
|-- services/
|-- tests/
|-- third_party/
|-- CMakeLists.txt
|-- main.cpp
|-- mainwindow.cpp
|-- mainwindow.h
|-- mainwindow.ui
|-- databasemanager.cpp
|-- databasemanager.h
|-- aimanager.cpp
`-- aimanager.h
```

## 入口与移动端外壳

- `main.cpp`：应用启动、资源初始化、全局样式加载和主窗口启动。
- `mainwindow.cpp/.h/.ui`：主页面组织、登录注册、首页、AI 分析、健康周报、个人中心等主要界面流程。
- `mobileshellcontroller.cpp/.h`：移动端外壳行为。
- `platformlayoutpolicy.cpp/.h`：平台相关布局策略。

## UI 组件

- `bottomnavbar.cpp/.h`：底部导航。
- `sidedrawer.cpp/.h`：侧边栏。
- `animateddialog.cpp/.h`：动画弹窗基础能力。
- `loadingoverlay.cpp/.h`：加载遮罩。
- `feedbackdialog.cpp/.h`：反馈弹窗。
- `deepanalysisdialog.cpp/.h`：AI 深度分析弹窗。
- `completionfeedback.cpp/.h`：任务完成反馈组件。
- `circularprogressbar.cpp/.h`：圆形进度组件。
- `trendlinechart.cpp/.h`：趋势图组件。
- `calendargridview.cpp/.h`：日历/打卡视图。
- `uifactory.cpp/.h`：通用 UI 创建辅助。
- `designtokens.h`：颜色、间距、圆角等设计常量。
- `thememanager.cpp/.h`：主题管理。

## 数据与 AI

- `databasemanager.cpp/.h`：SQLite 表结构、查询、用户数据、健康计划、反馈、报告、AI 请求日志和本机会话。
- `aimanager.cpp/.h`：AI 网络请求、API Key 检查、图片生成和结果信号。
- `models/dto.h`：共享数据模型。

## 服务层

`services/` 目录用于将业务流程从界面代码中拆分出来：

- `authflowservice`
- `authsessionservice`
- `accountflowservice`
- `userservice`
- `profileflowservice`
- `homeplanservice`
- `checkinservice`
- `freezecardservice`
- `deepanalysisservice`
- `reportservice`
- `airesponseparser`

## 资源目录

```text
resources/
|-- assets/       # 应用图片与图标
|-- fonts/        # 内置字体
|-- licenses/     # 字体与资源来源说明
|-- resources.qrc
`-- style.qss     # 全局 QSS 样式
```

## Android 目录

```text
android/
|-- AndroidManifest.xml
`-- res/
```

该目录由 CMake 作为 Qt Android 包资源目录使用。

## 测试

`tests/` 目录包含 Qt Test 单元测试。当前测试主要覆盖用户、登录、资料、首页计划、AI 解析和账户流程等服务层逻辑。Android 构建时测试目标不会启用。

## 仓库清理规则

以下内容不应进入版本库：

- 构建目录
- APK / AAB 产物
- Qt Creator 用户配置
- 本地 `.env`
- SQLite 数据库文件
- Python 缓存与本地依赖目录
- 日志文件
- 临时修复脚本
