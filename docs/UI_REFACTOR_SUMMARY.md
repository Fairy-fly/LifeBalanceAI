# 移动端 UI 重构总结

本文档总结 LifeBalanceAI 在移动端 UI 方向的整理与重构重点，用于课程项目展示。

## 重构目标

- 将项目从桌面端 Qt Demo 整理为更接近移动端 App 的健康管理 Demo。
- 保持核心业务逻辑、数据库逻辑和 AI 调用逻辑稳定。
- 通过 QSS、Design Tokens、通用组件和布局策略提升视觉一致性。
- 让首页、AI 分析、健康周报、个人中心和弹窗在 Android 真机上更易读、易操作。

## Qt Widgets 移动端适配

- 使用 Qt Widgets 构建移动端界面，保留桌面预览能力。
- 通过底部导航、侧边栏、移动端弹窗和紧凑卡片组织主流程。
- 对手机屏幕宽度、控件间距、页面滚动和触摸操作进行适配。

## QSS 全局样式整理

- 全局样式集中在 `LifeBalanceAI_demo/resources/style.qss`。
- 统一按钮、输入框、卡片、弹窗、底部导航等控件风格。
- 结合 `designtokens.h` 管理常用颜色、圆角、间距和字体风格。
- 使用 qrc 资源加载图片与字体，减少运行路径依赖。

## 首页布局优化

- 首页围绕健康计划和每日任务组织信息。
- 强化饮食、运动、打卡、进度反馈等模块的视觉层级。
- 减少移动端拥挤感，让关键信息更适合单手浏览。

## AI 分析页优化

- 优化从输入到 AI 分析结果的流程展示。
- 改善加载状态、错误提示和结果弹窗体验。
- AI 结果解析逻辑保留在服务层，UI 侧只负责展示与交互。

## 健康周报页优化

- 强化周报内容、趋势图、历史记录之间的信息层级。
- 使用自定义图表和进度组件提升报告可读性。
- 保持报告数据与 SQLite 本地存储逻辑解耦。

## 个人中心和弹窗体验优化

- 优化个人资料、账号操作、反馈入口和退出登录流程。
- 统一反馈弹窗、AI 报告弹窗、加载遮罩、深度分析弹窗的视觉风格。
- 提升移动端触摸区域、按钮层级和弹窗关闭体验。

## Android 真机适配

- 增加 Android Manifest 与启动图标资源。
- 配置 Android OpenSSL 动态库，支持 HTTPS AI 请求。
- 提供设备侧 `.env` 推送脚本，避免将真实 API Key 写入仓库。

## 代表文件

- `LifeBalanceAI_demo/mainwindow.cpp`
- `LifeBalanceAI_demo/resources/style.qss`
- `LifeBalanceAI_demo/designtokens.h`
- `LifeBalanceAI_demo/platformlayoutpolicy.cpp`
- `LifeBalanceAI_demo/mobileshellcontroller.cpp`
- `LifeBalanceAI_demo/bottomnavbar.cpp`
- `LifeBalanceAI_demo/sidedrawer.cpp`
- `LifeBalanceAI_demo/animateddialog.cpp`
- `LifeBalanceAI_demo/deepanalysisdialog.cpp`
- `LifeBalanceAI_demo/feedbackdialog.cpp`

## 说明

本次整理只处理仓库展示、文档、忽略规则和误提交文件清理，不重写核心业务逻辑、数据库逻辑或 AI 调用逻辑。
