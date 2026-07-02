# LifeBalanceAI

LifeBalanceAI 是一个基于 Qt Widgets / C++ 的 AI 健康管理移动端 Demo。项目覆盖登录注册、首页健康计划、AI 饮食分析、健康周报、个人中心、SQLite 本地数据存储，以及 Android 真机适配等完整流程。

本仓库定位为正式课程项目展示仓库，重点展示 C++ 工程组织、Qt Widgets 移动端 UI 适配、QSS 全局样式整理、SQLite 数据持久化、AI 接口集成与 Android 打包部署能力。

## 核心功能

- 登录、注册、密码重置与本机自动登录
- 首页健康计划，包括饮食、运动、打卡与进度反馈
- AI 饮食分析与 AI 报告弹窗
- 健康周报、趋势展示与历史报告
- 个人中心、账户设置、反馈弹窗与退出登录
- 基于 SQLite 的本地数据存储
- 底部导航、侧边栏、移动端弹窗和紧凑卡片布局
- Android 真机运行与 HTTPS AI 请求支持

## 技术栈

- 开发语言：C++17
- UI 框架：Qt 6.5+ / Qt Widgets
- 构建系统：CMake
- 本地存储：SQLite / Qt SQL
- 网络请求：Qt Network
- UI 样式：QSS、Design Tokens、自定义 Widgets
- 目标平台：桌面端预览、Android 移动端 Demo
- 测试：Qt Test 服务层单元测试

## 项目结构

```text
LifeBalanceAI/
|-- LifeBalanceAI_demo/
|   |-- android/                    # Android Manifest 与图标资源
|   |-- models/                     # DTO 数据模型
|   |-- resources/                  # QSS、图片、字体与 qrc 资源
|   |-- services/                   # 登录、计划、报告、用户、AI 解析等服务层
|   |-- tests/                      # Qt Test 单元测试
|   |-- third_party/android_openssl/ # Android HTTPS 所需 OpenSSL 库
|   |-- main.cpp
|   |-- mainwindow.cpp/.h/.ui       # 主界面与页面流程
|   |-- databasemanager.cpp/.h      # SQLite 数据库管理
|   |-- aimanager.cpp/.h            # AI 请求管理
|   `-- CMakeLists.txt
|-- docs/
|   |-- ANDROID_BUILD_GUIDE.md
|   |-- PROJECT_STRUCTURE.md
|   `-- UI_REFACTOR_SUMMARY.md
|-- screenshots/
|   `-- README.md
|-- .gitignore
`-- README.md
```

详细结构说明见 [docs/PROJECT_STRUCTURE.md](docs/PROJECT_STRUCTURE.md)。

## 本地运行环境

推荐环境：

- Qt 6.5 或更新版本
- Qt Creator 11+，或兼容的 CMake 构建环境
- CMake 3.19+
- 支持 C++17 的编译器
- Windows 桌面预览 Kit，例如 MinGW 64-bit 或 MSVC
- Android Kit，例如 Qt 6.5 Android Clang arm64-v8a

AI 功能需要本地环境变量文件。复制示例文件后填写自己的 Key：

```powershell
Copy-Item LifeBalanceAI_demo\.env.example LifeBalanceAI_demo\.env
```

真实 API Key、数据库文件和本机配置文件均不应提交到 Git。

## 桌面端运行

Qt Creator 方式：

1. 打开 `LifeBalanceAI_demo/CMakeLists.txt`。
2. 选择桌面端 Qt Kit。
3. Configure、Build、Run。

命令行方式：

```powershell
cmake -S LifeBalanceAI_demo -B LifeBalanceAI_demo/build/Desktop-Debug
cmake --build LifeBalanceAI_demo/build/Desktop-Debug
```

## Qt / Android 构建说明

项目已包含 Android 包资源目录 `LifeBalanceAI_demo/android/`，并在 CMake 中配置了 `QT_ANDROID_PACKAGE_SOURCE_DIR`。Android 端 HTTPS 请求依赖 `LifeBalanceAI_demo/third_party/android_openssl/ssl_3/` 下的 OpenSSL 动态库。

推荐流程：

1. 安装 Qt Android 组件。
2. 在 Qt Creator 中配置 JDK、Android SDK、Android NDK 和 Gradle。
3. 选择 Android Kit，真机测试推荐 `arm64-v8a`。
4. 使用 Qt Creator 构建并部署到 Android 设备。
5. 如需测试 AI 功能，安装后使用根目录 `tools/android_push_env.ps1` 推送本地 `.env`。

详细步骤见 [docs/ANDROID_BUILD_GUIDE.md](docs/ANDROID_BUILD_GUIDE.md)。

## 数据库说明

项目使用 SQLite 作为本地数据库，并通过 `DatabaseManager` 与 Qt SQL 访问。数据库保存用户、资料、健康计划、每日任务、反馈、健康报告、AI 请求日志和本机登录会话等数据。

数据库文件由程序运行时在本地生成，不进入版本库，避免提交个人测试数据。

## 页面截图占位

正式展示前建议补充以下 Android 真机截图：

| 页面 | 状态 |
| --- | --- |
| 登录页 | 待补充 |
| 首页 | 待补充 |
| AI 分析页 | 待补充 |
| AI 报告弹窗 | 待补充 |
| 健康周报页 | 待补充 |
| 我的页面 | 待补充 |
| 侧边栏 | 待补充 |

截图清单见 [screenshots/README.md](screenshots/README.md)。

## 我的主要贡献

- 将仓库整理为更适合课程项目展示的结构
- 补充项目 README、Android 构建说明、项目结构说明和 UI 重构总结
- 梳理 Qt Widgets 移动端 Demo 的技术定位与运行说明
- 清理缓存、日志、本地 Python 依赖、构建目录、临时脚本和验收截图等误提交内容
- 修正 Android `.env` 推送脚本的本机绝对路径默认值

## UI 重构亮点

- Qt Widgets 移动端适配：围绕手机屏幕组织页面宽度、间距、导航与弹窗
- QSS 全局样式整理：统一按钮、输入框、卡片、弹窗、底部导航与字体资源
- 首页布局优化：强化健康计划、每日任务、打卡状态和进度反馈的层级
- AI 分析页优化：优化输入、加载状态、分析结果和报告弹窗体验
- 健康周报页优化：提升趋势图、历史报告和周报内容的信息层级
- 个人中心和弹窗体验优化：改善个人资料、反馈、退出登录和深度分析弹窗
- Android 真机适配：补充 Manifest、启动图标、OpenSSL 库和设备侧 `.env` 推送流程

## 后续优化计划

- 补充 `screenshots/` 下的最终真机截图
- 增加 README 演示 GIF 或短视频
- 在签名配置确定后补充 Release APK 打包说明
- 继续拆分过大的 UI 文件，沉淀更多页面级组件
- 扩展报告、AI 解析、资料编辑等流程的单元测试
- 补充模块关系图或架构图

## GitHub About 推荐

- Description: `AI health management mobile demo built with C++ and Qt Widgets, featuring SQLite storage, AI diet analysis, weekly reports, and Android UI adaptation.`
- Topics: `cpp`, `qt`, `qt-widgets`, `cmake`, `sqlite`, `android`, `mobile-ui`, `health-app`, `ai-demo`, `qss`
