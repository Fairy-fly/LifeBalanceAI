# Android 构建说明

本文档说明 LifeBalanceAI 的 Qt Android 构建环境、构建步骤和常见问题。项目使用 Qt Widgets / C++ 实现，Android 构建通过 Qt CMake 工程完成。

## 环境要求

- Qt 6.5 或更新版本，并安装 Android 组件。
- Qt Creator，且已配置 Android 开发环境。
- JDK。
- Android SDK。
- Android NDK。
- CMake 3.19+。
- Android 真机或模拟器。

推荐真机 ABI：

- `arm64-v8a`

## 项目入口

在 Qt Creator 中打开：

```text
LifeBalanceAI_demo/CMakeLists.txt
```

CMake 中已配置 Android 包资源目录：

```cmake
QT_ANDROID_PACKAGE_SOURCE_DIR = LifeBalanceAI_demo/android
```

Android 资源目录：

```text
LifeBalanceAI_demo/android/
|-- AndroidManifest.xml
`-- res/
    |-- drawable-nodpi/
    `-- mipmap-*/
```

Android HTTPS 请求使用以下 OpenSSL 库：

```text
LifeBalanceAI_demo/third_party/android_openssl/ssl_3/
|-- arm64-v8a/
|-- armeabi-v7a/
|-- x86/
`-- x86_64/
```

## Qt Creator 构建步骤

1. 打开 `LifeBalanceAI_demo/CMakeLists.txt`。
2. 选择 Android Qt Kit，例如 `Qt 6.5.x Clang arm64-v8a`。
3. Configure 项目。
4. 连接已开启 USB 调试的 Android 设备。
5. Build 并 Deploy `LifeBalanceAI_demo`。
6. 在 Qt Creator 或手机启动器中运行 App。

## AI Key 配置

真实 API Key 不应提交到仓库。桌面端本地运行时可复制：

```powershell
Copy-Item LifeBalanceAI_demo\.env.example LifeBalanceAI_demo\.env
```

然后填写：

```text
DEEPSEEK_API_KEY=
OPENAI_API_KEY=
```

Android 真机测试时，先安装 Debug 版本 App，再从仓库根目录执行：

```powershell
powershell -File tools/android_push_env.ps1 -EnvFile LifeBalanceAI_demo\.env
```

脚本会通过 `adb run-as` 将 `.env` 写入 App 私有目录。推送后请重启 App。

## 常见问题

- 网络请求失败：确认 `AndroidManifest.xml` 中包含 `android.permission.INTERNET`。
- HTTPS 请求失败：确认当前 ABI 对应的 OpenSSL 库存在。
- AI Key 未生效：确认 `.env` 没有提交到 Git，且已推送到 App 私有目录。
- UI 不是最新：卸载旧 APK 后重新部署。
- 桌面端显示正常但真机布局异常：以 Android 真机截图为准，继续检查安全区、密度和滚动区域。
- Release APK 打包：应在签名配置确定后单独补充发布说明，不要把 APK 产物提交到仓库。

## 不应提交的生成文件

- `build/`
- `*.apk`
- `*.aab`
- Gradle / CMake 生成目录
- Qt Creator 用户配置文件
- 本地 `.env`
- 运行日志
- 本地 SQLite 数据库文件
