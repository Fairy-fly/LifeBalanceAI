# 项目结构说明

LifeBalanceAI 以 Qt Widgets 应用为主体，按入口界面、UI 组件、服务层、资源文件、Android 包配置和测试代码组织。

## 根目录

```text
LifeBalanceAI/
|-- LifeBalanceAI_demo/     # 主 Qt / C++ 应用
|-- docs/                   # 项目说明、结构说明、构建说明和历史资料
|-- screenshots/            # 展示截图占位与补充清单
|-- tools/                  # 本地辅助脚本
|-- .gitattributes
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

## 入口与页面组织

- `main.cpp`：应用启动、资源初始化、全局样式加载和主窗口启动。
- `mainwindow.cpp/.h/.ui`：登录注册、首页、AI 分析、健康周报、个人中心等主要页面流程。
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
- `calendargridview.cpp/.h`：日历 / 打卡视图。
- `uifactory.cpp/.h`：通用 UI 创建辅助。
- `designtokens.h`：颜色、间距、圆角等设计常量。
- `thememanager.cpp/.h`：主题管理。

## 服务层

`services/` 目录用于将业务流程从界面代码中拆分出来：

- `authflowservice`：登录注册流程。
- `authsessionservice`：本机会话。
- `accountflowservice`：账号相关流程。
- `userservice`：用户数据操作。
- `profileflowservice`：个人资料流程。
- `homeplanservice`：首页健康计划组织。
- `checkinservice`：打卡逻辑。
- `freezecardservice`：补签 / 冻结卡相关逻辑。
- `deepanalysisservice`：深度分析流程。
- `reportservice`：健康报告流程。
- `airesponseparser`：AI 响应解析。

## 数据库相关文件

- `databasemanager.cpp/.h`：SQLite 表结构、初始化、查询和持久化访问。
- `models/dto.h`：用户、资料、报告等共享数据模型。
- `services/*`：围绕数据库和界面流程组织服务层逻辑。

数据库文件由应用运行时在本地生成，不进入版本库。

## AI 相关文件

- `aimanager.cpp/.h`：AI 网络请求、API Key 检查、图片生成和结果信号。
- `services/airesponseparser.cpp/.h`：AI 返回内容解析。
- `services/deepanalysisservice.cpp/.h`：深度分析流程。
- `services/reportservice.cpp/.h`：报告生成流程。

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

`tests/` 目录包含 Qt Test 单元测试，主要覆盖用户、登录、资料、首页计划、AI 解析和账号流程等服务层逻辑。Android 构建时测试目标不会启用。

## 文档目录

- `docs/UI_REFACTOR_SUMMARY.md`：移动端 UI 优化总结。
- `docs/ANDROID_BUILD_GUIDE.md`：Android 构建说明。
- `docs/PROJECT_STRUCTURE.md`：项目结构说明。
- `docs/architecture/`：阶段性架构和重构记录。
- `docs/specs/`：课程设计相关规格文档。
- `docs/archive/`：历史归档资料。

## 仓库清理规则

以下内容不应进入版本库：

- 构建目录
- APK / AAB 产物
- Qt Creator 用户配置
- 本地 `.env`
- SQLite 数据库文件
- Python 缓存与本地依赖目录
- 运行日志
- 临时修复脚本
