# 截图说明

本目录保存 LifeBalanceAI 的 GitHub 展示截图。当前截图来自 Android 真机运行的 `org.qtproject.example.LifeBalanceAI_demo`，通过 ADB `screencap` 采集并导出。

截图使用非真实演示账号采集：

- 昵称：`DemoUser`
- 手机号：`19900000000`
- 数据：演示画像与空状态健康记录

## 截图清单

| 页面 | 文件 | 说明 |
| --- | --- | --- |
| 登录页 | `login.png` | 登录表单与演示账号状态 |
| 首页 | `home.png` | 今日计划、长期目标、底部导航 |
| AI 分析页 | `ai-analysis.png` | AI 健康洞察入口与移动端布局 |
| AI 报告入口 | `ai-report-dialog.png` | 真机运行中的报告/导出能力入口状态，未伪造成已生成报告 |
| 健康周报页 | `weekly-report.png` | 周报说明、完成率与趋势图 |
| 个人中心 | `profile.png` | 演示账号资料与画像信息 |
| 侧边栏 | `side-drawer.png` | 移动端抽屉导航与账号入口 |
| 退出确认 | `logout-confirm.png` | 移动端退出登录确认弹窗 |

## 后续补充建议

- 如后续配置可用 AI Key 并生成真实报告，可重新采集 `ai-report-dialog.png`，但不要提交 API Key 或真实请求日志。
- 如补充更多演示数据，建议使用虚构账号和虚构健康记录，避免展示真实手机号、真实姓名、真实健康数据。
- 如发布 APK，请通过 GitHub Release 单独上传，不要直接提交到仓库。
