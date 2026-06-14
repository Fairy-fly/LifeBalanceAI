# LifeBalanceAI - UI升级设计规范文档

## 📋 文档概述
这是LifeBalanceAI项目的全面UI升级设计规范，用于指导开发实现健康生活规划主题的现代化界面。本文档包含完整的设计系统、组件规范、布局方案和交互细节。

---

## 第一部分：设计系统基础

### 1.1 配色系统（完整规范）

#### 主色板

**主色系（代表健康、活力）**
```
Primary Dark:     #2D7A5E  (最深，用于强调和悬停)
Primary:          #4CAF7F  (主色，按钮、标题强调)
Primary Light:    #6BC48F  (悬停状态)
Primary Lighter:  #A8E6C3  (背景层级)
Primary Lightest: #E8F8F2  (卡片背景、弱背景)
```

**次色系（代表热情、坚持）**
```
Accent Dark:      #D97E3A  (深强调)
Accent:           #F4A261  (按钮、警告)
Accent Light:     #F8B88D  (悬停)
Accent Lightest:  #FFF1E6  (背景)
```

**强调色系（代表目标、清晰）**
```
Blue:             #6BA3D4  (信息、目标色)
Blue Light:       #A8C8E1  (背景)
Blue Lightest:    #EEF4F9  (弱背景)
```

**状态色系**
```
Success:          #52C41A  (成功、完成)
Success BG:       #F6FFED  (成功背景)
Success Border:   #B7EB8F  (成功边框)

Active:           #FFB84D  (进行中、活跃)
Active BG:        #FFFBE6  (活跃背景)
Active Border:    #FFD591  (活跃边框)

Rest:             #A0AEC0  (休息、调整)
Rest BG:          #F2F5FB  (休息背景)
Rest Border:      #D1D9E8  (休息边框)

Warning:          #FF7875  (警告、预警)
Warning BG:       #FFF1F0  (警告背景)
Warning Border:   #FFCCCB  (警告边框)

Disabled:         #BFBFBF  (禁用)
Disabled BG:      #F5F5F5  (禁用背景)
```

**中性色系**
```
Text Title:       #1A1A1A  (标题文本)
Text Primary:     #333333  (主文本)
Text Secondary:   #666666  (次要文本)
Text Tertiary:    #999999  (辅助文本)
Text Disabled:    #CCCCCC  (禁用文本)

BG Window:        #FAFAFA  (窗口背景)
BG Page:          #FFFFFF  (页面背景)
BG Card:          #FEFEFE  (卡片背景)
BG Soft:          #F5F5F5  (软背景)
BG Lighter:       #EFEFEF  (更浅背景)

Border:           #E8E8E8  (标准边框)
Border Light:     #F0F0F0  (浅边框)
Border Dark:      #D9D9D9  (深边框)
Divider:          #F0F0F0  (分割线)
```

**健康维度色彩映射（用于不同功能区分）**
```
Diet/Nutrition:      #FF6B6B  (饮食 - 红色)
Exercise/Fitness:    #4ECDC4  (运动 - 绿松石)
Sleep/Rest:          #6C5CE7  (睡眠 - 紫色)
Mindfulness:         #A29BFE  (心理 - 浅紫)
Social:              #FD79A8  (社交 - 粉色)
Work/Productivity:   #74B9FF  (工作 - 天蓝)
```

---

### 1.2 排版系统

#### 字体栈
```
中文优先：MiSans, MiSans-Regular, MiSans-Bold, MiSans-Medium
英文优先：-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif
装饰用：Smiley Sans Oblique (仅用于特殊标题或品牌元素)
```

#### 字体大小和权重定义

**大标题 (Display)**
```
Size:      32px
Weight:    700 (Bold)
Line-height: 1.3 (41.6px)
Letter-spacing: -0.5px
用途：主页标题、页面标题
```

**标题1 (Heading 1)**
```
Size:      28px
Weight:    700 (Bold)
Line-height: 1.4 (39.2px)
Letter-spacing: -0.3px
用途：主要区域标题
```

**标题2 (Heading 2)**
```
Size:      24px
Weight:    700 (Bold)
Line-height: 1.4 (33.6px)
Letter-spacing: 0px
用途：卡片标题、重要标题
```

**标题3 (Heading 3)**
```
Size:      20px
Weight:    600 (Semi-Bold)
Line-height: 1.4 (28px)
Letter-spacing: 0px
用途：小节标题
```

**副标题 (Subtitle)**
```
Size:      16px
Weight:    500 (Medium)
Line-height: 1.5 (24px)
Letter-spacing: 0px
用途：卡片副标题、说明文字
```

**正文 (Body)**
```
Size:      14px
Weight:    400 (Regular)
Line-height: 1.6 (22.4px)
Letter-spacing: 0.3px
用途：主要内容文本
```

**小文本 (Small)**
```
Size:      12px
Weight:    400 (Regular)
Line-height: 1.5 (18px)
Letter-spacing: 0px
用途：辅助文本、标签
```

**超小文本 (Tiny)**
```
Size:      11px
Weight:    400 (Regular)
Line-height: 1.4 (15.4px)
Letter-spacing: 0px
用途：标记、提示文字
```

**数字/统计 (Numeric)**
```
Size:      18px
Weight:    700 (Bold)
Font-feature-settings: 'tnum' (表格数字)
Line-height: 1.2
用途：数字强调、统计值、百分比
```

---

### 1.3 间距系统（统一的8px基准）

```
xs:   4px   (微小间距，内部元素间)
sm:   8px   (小间距，相关元素间)
md:   12px  (标准间距，卡片内)
lg:   16px  (大间距，区域间)
xl:   24px  (特大间距，主要区域间)
xxl:  32px  (超大间距，页面段落间)
```

#### 应用示例
```
卡片内边距：       lg (16px)
卡片间距：         lg (16px) / xl (24px)
按钮内边距：       sm (8px) 上下，lg (16px) 左右
标题下边距：       md (12px)
段落下边距：       lg (16px) / xl (24px)
列表项间距：       sm (8px) / md (12px)
```

---

### 1.4 圆角系统

```
xs:   4px   (输入框、小按钮)
sm:   6px   (普通元素)
md:   8px   (卡片、大按钮)
lg:   12px  (模态框、大容器)
xl:   16px  (圆形或药丸状)
pill: 50%   (完全圆形)
```

---

### 1.5 阴影系统

```
Elevation 1 (浅浮起)
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.08)
  用途：普通卡片

Elevation 2 (中等浮起)
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.10)
  用途：悬停卡片、普通模态框

Elevation 3 (深层浮起)
  box-shadow: 0 8px 16px rgba(0, 0, 0, 0.12)
  用途：模态框、下拉菜单

Elevation 4 (最高浮起)
  box-shadow: 0 12px 24px rgba(0, 0, 0, 0.15)
  用途：顶层模态、通知提示

Focus Shadow (聚焦阴影)
  box-shadow: 0 0 0 3px rgba(76, 175, 127, 0.2)
  用途：获得焦点的元素

Inset Shadow (内凹)
  inset 0 1px 3px rgba(0, 0, 0, 0.08)
  用途：输入框、按下状态
```

---

### 1.6 动画系统

#### 动画时长
```
Fast:     150ms  (快速反馈，如按钮悬停)
Base:     250ms  (标准动画，如模态出现)
Slow:     400ms  (缓慢动画，如页面过渡)
Slower:   600ms  (很慢的动画，如首屏加载)
```

#### 缓动函数
```
Linear:       ease-linear (匀速)
Ease In Out:  cubic-bezier(0.4, 0, 0.2, 1) (标准)
Ease Out:     cubic-bezier(0, 0, 0.2, 1) (快速进入)
Ease In:      cubic-bezier(0.4, 0, 1, 1) (快速离开)
Bounce:       cubic-bezier(0.68, -0.55, 0.265, 1.55) (弹簧效果)
```

#### 具体动画定义

**按钮交互**
```
Hover: background 150ms ease-in-out + scale(1.02)
Press: background 50ms linear + scale(0.98)
Disabled: opacity 150ms ease-in-out
```

**卡片交互**
```
Hover: transform translateY(-2px) + shadow Elevation 3, 200ms ease-out
Click: transform scale(0.98), 100ms ease-in
```

**页面过渡**
```
进入: opacity 0->1, 300ms ease-out + transform translateY(20px)->0
退出: opacity 1->0, 200ms ease-in + transform translateY(0)->(-20px)
```

**列表项动画**
```
出现: opacity 0->1 + translateX(-20px)->0, 300ms ease-out, 级联延迟
```

**加载动画**
```
旋转: rotate(0->360deg), 2s linear 无限循环
呼吸: opacity 0.4->1->0.4, 2s ease-in-out 无限循环
```

---

## 第二部分：组件设计规范

### 2.1 按钮组件

#### 按钮类型和状态矩阵

**主按钮 (Primary)**
```
默认 (Default):
  Background:  #4CAF7F
  Text Color:  #FFFFFF
  Height:      44px
  Padding:     12px 24px
  Border:      none
  Border-radius: 8px
  Font-size:   14px
  Font-weight: 600

悬停 (Hover):
  Background:  #3A9B6F
  Elevation:   Elevation 2
  Transform:   translateY(-1px)

按下 (Pressed):
  Background:  #2D7A5E
  Transform:   translateY(0)
  Elevation:   Elevation 1

禁用 (Disabled):
  Background:  #BFBFBF
  Text Color:  #FFFFFF
  Opacity:     0.6
  Cursor:      not-allowed

加载中 (Loading):
  显示旋转加载图标
  Disabled 状态
```

**次级按钮 (Secondary)**
```
默认 (Default):
  Background:  #E8F8F2
  Text Color:  #4CAF7F
  Border:      1px solid #A8E6C3
  Height:      44px
  Padding:     12px 24px
  Border-radius: 8px

悬停 (Hover):
  Background:  #D4F0E5
  Border:      1px solid #6BC48F

按下 (Pressed):
  Background:  #C0EDD9
  Border:      1px solid #4CAF7F
```

**强调按钮 (Accent)**
```
默认:
  Background:  #F4A261
  Text Color:  #FFFFFF
  Height:      44px
  Padding:     12px 24px
  Border-radius: 8px

悬停:
  Background:  #E8914D

按下:
  Background:  #DC7D3A
```

**危险按钮 (Danger)**
```
默认:
  Background:  #FF7875
  Text Color:  #FFFFFF

悬停:
  Background:  #FF5252

按下:
  Background:  #E63946
```

**文字按钮 (Text)**
```
默认:
  Background:  transparent
  Text Color:  #4CAF7F
  Font-weight: 600

悬停:
  Background:  rgba(76, 175, 127, 0.08)
  Border-radius: 4px

按下:
  Background:  rgba(76, 175, 127, 0.15)
```

**尺寸变体**
```
Large (lg):
  Height:      48px
  Padding:     14px 32px
  Font-size:   16px

Medium (md):
  Height:      44px
  Padding:     12px 24px
  Font-size:   14px (默认)

Small (sm):
  Height:      36px
  Padding:     8px 16px
  Font-size:   12px

Tiny (xs):
  Height:      32px
  Padding:     6px 12px
  Font-size:   11px
```

---

### 2.2 输入框组件

#### 文本输入框 (TextInput)

**默认状态**
```
Height:           38px
Padding:          8px 12px
Font-size:        14px
Background:       #FFFFFF
Border:           1px solid #E8E8E8
Border-radius:    8px
Text Color:       #333333
Placeholder Color: #999999
Transition:       border 150ms ease-out, box-shadow 150ms ease-out
```

**焦点状态 (Focus)**
```
Border:      1px solid #4CAF7F
Box-shadow:  0 0 0 3px rgba(76, 175, 127, 0.2)
Background:  #FFFFFF
```

**错误状态 (Error)**
```
Border:      1px solid #FF7875
Box-shadow:  0 0 0 3px rgba(255, 120, 117, 0.2)
Background:  #FFF1F0
```

**禁用状态 (Disabled)**
```
Background:  #F5F5F5
Border:      1px solid #D9D9D9
Text Color:  #CCCCCC
Cursor:      not-allowed
```

**带标签的输入 (Labeled Input)**
```
Label:
  Font-size:   12px
  Font-weight: 600
  Color:       #333333
  Margin-bottom: 6px
  Display:     block

Label (必填):
  后缀 "*" 用 #FF7875 表示
```

**带前缀/后缀的输入**
```
Icon/文本置于输入框内部
前缀: 左侧 8px 位置
后缀: 右侧 8px 位置
颜色: #999999
```

#### 文本区域 (TextArea)

**默认**
```
Height:           120px (最小)
Padding:          12px
Font-size:        14px
Line-height:      1.6
Border:           1px solid #E8E8E8
Border-radius:    8px
Resize:           vertical (允许用户调整高度)
```

**其他状态同 TextInput**

#### 下拉选择框 (Dropdown)

**默认状态**
```
Height:           38px
Padding:          8px 12px
Background:       #FFFFFF
Border:           1px solid #E8E8E8
Border-radius:    8px
Arrow Icon:       右侧, #666666, 16x16px
```

**展开状态**
```
Border:           1px solid #4CAF7F
Box-shadow:       0 4px 8px rgba(0, 0, 0, 0.10)
Arrow Icon:       旋转 180°
```

**下拉菜单项**
```
Item Height:      36px
Item Padding:     10px 12px
Item Font-size:   14px
Hover Background: #F0F0F0
Selected:
  Background:    #E8F8F2
  Text Color:    #4CAF7F
  Font-weight:   600
```

#### 复选框 (Checkbox)

**默认状态**
```
Size:             18x18px
Border:           2px solid #D9D9D9
Border-radius:    4px
Background:       #FFFFFF
```

**选中状态 (Checked)**
```
Border:           2px solid #4CAF7F
Background:       #4CAF7F
Checkmark:        #FFFFFF, 2px stroke width
Checkmark Icon:   SVG 或 font icon
```

**焦点状态**
```
Box-shadow:       0 0 0 3px rgba(76, 175, 127, 0.2)
```

**禁用状态**
```
Border:           2px solid #D9D9D9
Background:       #F5F5F5
Opacity:          0.6
Cursor:           not-allowed
```

**标签**
```
Font-size:        14px
Color:            #333333
Margin-left:      8px
Vertical-align:   center
```

#### 单选框 (Radio Button)

**默认状态**
```
Size:             18x18px
Border:           2px solid #D9D9D9
Border-radius:    50%
Background:       #FFFFFF
```

**选中状态**
```
Border:           2px solid #4CAF7F
Inner Circle:     8px, #4CAF7F
```

**其他状态同 Checkbox**

#### 开关 (Toggle/Switch)

**关闭状态 (Off)**
```
Width:            44px
Height:           24px
Border-radius:    12px
Background:       #E8E8E8
Knob:
  Size:           20x20px
  Position:       2px from left
  Background:     #FFFFFF
  Border-radius:  50%
  Box-shadow:     Elevation 1
```

**打开状态 (On)**
```
Background:       #4CAF7F
Knob:
  Position:       22px from left
Transition:       all 200ms ease-out
```

**禁用状态**
```
Opacity:          0.5
Cursor:           not-allowed
```

---

### 2.3 卡片组件

#### 标准卡片 (Card)

```
Padding:           16px
Background:        #FFFFFF
Border:            1px solid #E8E8E8
Border-radius:     12px
Box-shadow:        Elevation 1
Transition:        all 200ms ease-out

悬停:
  Box-shadow:      Elevation 2
  Transform:       translateY(-2px)
```

#### 可点击卡片 (Interactive Card)

```
基于标准卡片，添加:
Cursor:            pointer
Active/Press:
  Transform:       scale(0.98)
  Box-shadow:      Elevation 1
Transition:        all 150ms ease-out
```

#### 带颜色条的卡片 (Colored Bar Card)

```
结构:
┌─────────────────────┐
│ █ [4px] | 内容     │
├─────────────────────┤
│ 左边框: 4px         │
│ 颜色: 依据维度      │
└─────────────────────┘

尺寸:
  Bar Width:    4px
  Padding:      16px
  Border-radius: 12px (外) / 0 (内)
  
应用:
  Diet:    #FF6B6B
  Exercise: #4ECDC4
  Sleep:   #6C5CE7
  Mindfulness: #A29BFE
```

#### 健康卡片 (Health Card)

```
最小高度: 100px
结构:
┌──────────────────────┐
│ [图标 16x16] 标题    │ (Header: 24px, weight 600)
├──────────────────────┤
│ 主要内容             │ (Body: 14px)
├──────────────────────┤
│ [小徽章] [小徽章]    │ (Status badges)
└──────────────────────┘

配色: 依据类型不同
```

#### 空状态卡片 (Empty State Card)

```
Padding:        24px
Text-align:     center

结构:
┌─────────────────────┐
│                     │
│    [图标 64x64]     │
│                     │
│    标题 (20px)      │
│    描述 (14px)      │
│                     │
│    [主操作按钮]     │
│                     │
└─────────────────────┘

图标颜色: #A8E6C3 (Primary Light)
标题颜色: #333333
描述颜色: #999999
```

---

### 2.4 进度条和进度指示器

#### 线性进度条 (Progress Bar)

**基础进度条**
```
高度:              8px
背景:              #F0F0F0
进度条颜色:        #4CAF7F
边框半径:          4px (pill style)
阴影:              无

标准进度条 (40%):
┌─────────────────────────────┐
│████████░░░░░░░░░░░░░░░░░░░░│
└─────────────────────────────┘

带动画的进度条 (Loading):
  进度条:  repeating-linear-gradient 横向条纹动画
  持续时间: 1.5s
  方向:    从左到右
```

**带百分比标签的进度条**
```
上方显示百分比: 14px, 600 weight, #333333
右对齐显示
```

**彩色进度条 (Status-based)**
```
成功 (100%):     背景 #52C41A
活跃 (进行中):   背景 #FFB84D
警告 (接近完成): 背景 #FF7875
```

#### 圆形进度条 (Circular Progress)

```
大小:              120px (standard)
线条宽度:          8px
背景圆:            #F0F0F0
进度圆:            #4CAF7F
渲染方式:          SVG

中心显示:
  百分比数字: 32px, 700 weight
  百分号:     14px
  说明文本:   12px, 颜色 #999999

旋转方向:          顺时针
初始角度:          -90° (从上方开始)

动画:
  进度更新: stroke-dashoffset 300ms ease-out
  旋转:     rotate 0->360, 2s linear (loading 状态)
```

**大尺寸圆形进度 (180px)**
```
字号增大 20%
用于首页仪表盘
```

#### 步骤指示器 (Stepper)

```
标准布局 (水平):
  ○ → — → ● → — → ○
  │      │      │
 未开始  进行中  完成

每个步骤:
  圆圈大小:      32px
  未激活色:      #E8E8E8
  激活色:        #FFB84D
  完成色:        #52C41A
  文字色:        #666666

连接线:
  高度:          2px
  未激活:        #E8E8E8
  激活:          #FFB84D
  完成:          #52C41A

标签:
  字体:          12px
  位置:          圆圈下方
  颜色:          #666666
```

---

### 2.5 标签和徽章

#### 标签 (Tag/Chip)

**基础标签**
```
Padding:          6px 12px
Height:           28px
Font-size:        12px
Border-radius:    14px (pill)
Border:           1px solid #E8E8E8
Background:       #F5F5F5
Text Color:       #666666
```

**主色标签 (Primary Tag)**
```
Background:       #E8F8F2
Border:           1px solid #A8E6C3
Text Color:       #4CAF7F
Font-weight:      500
```

**强调标签 (Accent Tag)**
```
Background:       #FFF1E6
Border:           1px solid #FFD591
Text Color:       #E8914D
```

**成功标签 (Success Tag)**
```
Background:       #F6FFED
Border:           1px solid #B7EB8F
Text Color:       #52C41A
```

**可关闭标签 (Closeable)**
```
右侧加关闭图标 (×): 12px, 颜色同文字
间距:          8px
Hover:         background 深一级
Click to remove: 淡出动画 150ms
```

#### 徽章 (Badge)

**数字徽章**
```
最小宽度:         24px
高度:            24px
显示方式:        圆形 (如果数字<=99)
背景:            #FF7875 (默认) 或 #4CAF7F (成功)
文字:            #FFFFFF, 12px, 700 weight
位置:            右上角, 相对容器绝对定位
```

**文字徽章**
```
Padding:         4px 8px
Height:          20px
Font-size:       11px
Border-radius:   3px
Background:      #FFB84D
Text Color:      #FFFFFF
Font-weight:     600
```

**状态徽章**
```
成功:   背景 #52C41A, 文字 #FFFFFF
进行中: 背景 #FFB84D, 文字 #FFFFFF
待开始: 背景 #E8E8E8, 文字 #666666
完成:   背景 #4CAF7F, 文字 #FFFFFF

尺寸: 18x18px (小) / 24x24px (中等)
```

---

### 2.6 通知和提示

#### 提示条 (Alert/Banner)

**四个等级**

**信息提示 (Info)**
```
背景:      #EEF4F9
边框:      1px solid #A8C8E1 (左边框 4px)
填充:      12px 16px
图标:      ⓘ #6BA3D4, 16x16px
标题:      14px, 600 weight, #333333
文本:      13px, 400 weight, #666666
关闭按钮:  × #999999

嵌套按钮:  文字按钮或小按钮
```

**成功提示 (Success)**
```
背景:      #F6FFED
边框:      1px solid #B7EB8F (左边框 4px)
图标:      ✓ #52C41A
标题颜色:  #333333
```

**警告提示 (Warning)**
```
背景:      #FFFBE6
边框:      1px solid #FFD591 (左边框 4px)
图标:      ⚠ #FFB84D
标题颜色:  #FF7875
```

**错误提示 (Error)**
```
背景:      #FFF1F0
边框:      1px solid #FFCCCB (左边框 4px)
图标:      ✕ #FF7875
标题颜色:  #FF7875
```

#### 浮动通知 (Toast)

**位置:** 右下角, 16px 距离屏幕边缘

```
宽度:             320px
最大宽度:         90vw
最小高度:         50px
Padding:         12px 16px
Background:      #333333 (深) 或 #FFFFFF (浅)
Border:          1px solid
Border-radius:   8px
Box-shadow:      Elevation 4
Z-index:         9999

文本:
  颜色:          #FFFFFF (深) / #333333 (浅)
  Font-size:     14px
  Line-height:   1.5

进入动画:
  Transform:     translateX(400px) -> 0
  Opacity:       0 -> 1
  Duration:      300ms ease-out

退出动画:
  Transform:     0 -> translateX(400px)
  Opacity:       1 -> 0
  Duration:      200ms ease-in

持续时间:        3000ms
可手动关闭:      左滑 或 点击关闭按钮
```

---

## 第三部分：页面布局和组件组合

### 3.1 首页/主仪表盘布局

#### 整体结构

```
┌─────────────────────────────────────────────┐
│  [3] 顶部欢迎区域                           │
├─────────────────────────────────────────────┤
│  [4] 今日健康指数卡片                        │
├─────────────────────────────────────────────┤
│  [5] 快速打卡区域                           │
├─────────────────────────────────────────────┤
│  [6] 今日规划摘要卡片                       │
├─────────────────────────────────────────────┤
│  [7] 本周总结数据                           │
├─────────────────────────────────────────────┤
│  [8] 最近提醒/通知                          │
├─────────────────────────────────────────────┤
│  [底部导航栏]                               │
└─────────────────────────────────────────────┘
```

#### 3.1.1 顶部欢迎区域

```
高度:               80px
背景:               线性渐变 (从 #E8F8F2 到 #FFF1E6)
Padding:            16px

布局:
┌─────────────────────────────────────┐
│ 👋 早上好，[用户名]                  │ (20px, 600 weight)
│ 📍 星期三 • 5月23日                 │ (14px, 400 weight, color #999999)
└─────────────────────────────────────┘

问候文案动态变化:
  06:00-11:59   早上好 + ☀️
  12:00-17:59   下午好 + 🌤️
  18:00-23:59   晚上好 + 🌙
  00:00-05:59   夜深了 + ⭐
```

#### 3.1.2 今日健康指数卡片

```
类型:               带色条的卡片 + 圆形进度
Padding:            20px
Margin:             16px (横向) / 12px (纵向)
高度:               240px

布局 (垂直分布):

┌────────────────────────────────┐
│ █ 今日健康指数                  │ (16px, 700 weight)
├────────────────────────────────┤
│                                │
│          ◯ 75%                 │ (圆形进度, 120px)
│          健康指数              │
│                                │
├────────────────────────────────┤
│ 🍎 饮食    ●●●○  达成度 75%   │ (12px, 5颗星, 颜色 #FF6B6B)
│ 🏃 运动    ●●●●○ 达成度 80%   │ (颜色 #4ECDC4)
│ 😴 睡眠    ●●●●● 达成度 100%  │ (颜色 #6C5CE7)
│ 🔥 连续    28天                │ (32px 数字, 颜色 #F4A261)
└────────────────────────────────┘

圆形进度条中心:
  上: 75 (32px, 700 weight)
  下: 健康指数 (12px, 400 weight, color #999999)

色条:
  颜色:  #4CAF7F (主色绿)
  宽度:  4px
  位置:  左侧
```

#### 3.1.3 快速打卡区域

```
Padding:            16px
显示方式:           4列网格 (响应式: 2列/3列/4列)
间距:               12px

每个打卡按钮:
┌─────────┐
│   🍎    │ (图标 32x32)
│  饮食   │ (标签 12px, 600 weight)
│ 更新    │ (小文字 11px, 灰色)
└─────────┘

按钮:
  宽度:           auto (均分)
  高度:           80px
  背景:           #FFFFFF + border
  Border:         1px solid #E8E8E8
  Border-radius:  8px
  Hover:
    背景:         #E8F8F2
    Border:       1px solid #A8E6C3
  Click:
    动画:         shake (10px 左右晃动, 200ms)
    反馈:         Toast 提示

打卡类型:
  🍎 饮食
  🏃 运动
  😴 睡眠
  🧘 冥想
```

#### 3.1.4 今日规划摘要卡片

```
高度:               120px
Padding:            16px
背景:               #FFFFFF

标题: 今日规划 (16px, 700 weight)

内容 (上下两行):
  早餐 │ 工作  → 完成度 [进度条 30%]
  午餐 │ 运动  → 完成度 [进度条 60%]

操作按钮: 
  [查看详情] (文字按钮, 右对齐)
  点击进入完整规划页面
```

#### 3.1.5 本周总结数据

```
类型:               网格式卡片布局
显示:               7个数据项 (周一至周日)

┌─────────────────────────────┐
│ 📊 本周完成情况             │
├─────────────────────────────┤
│ 周一 周二 周三 周四 周五 周六 周日
│  ✓   ✓   ✓   ○   ✓   ✓   -
│ (颜色: 绿色✓ / 灰色○ / 白色-)
│                             │
│ 完成率: 83.3% 🎉            │
└─────────────────────────────┘

说明:
  ✓ = 完成
  ○ = 部分完成
  - = 未完成/未来日期
  
颜色编码:
  完成: #52C41A
  部分: #FFB84D
  未完成: #E8E8E8
  未来: 白色
```

#### 3.1.6 最近通知/提醒

```
高度:               auto (min 100px)
显示:               最多3条通知

单条通知样式:
┌─────────────────────────────┐
│ ⓘ 标题              [右上角时间]
│ 通知描述文本 14px   
│ [关闭] [操作]
└─────────────────────────────┘

通知类型:
  绿色提示: 成就/完成提醒
  橙色提示: 未完成提醒
  红色提示: 重要提醒
  蓝色提示: 信息通知
```

---

### 3.2 打卡记录页面

#### 整体结构

```
┌────────────────────────────────┐
│ 📋 打卡记录                     │ (标题)
├────────────────────────────────┤
│ [日期选择] [筛选]              │ (工具栏)
├────────────────────────────────┤
│ 日期: 2026-05-23 (周五)        │
│                                │
│ 🍎 早餐                        │ (时间卡片)
│ 09:15 | 完成 ✓                │
│ 鸡蛋、燕麦、牛奶               │
│                                │
│ 🏃 运动                        │
│ 10:00-11:00 | 进行中 ●        │
│ 跑步 5km                       │
│                                │
│ ...更多项目                    │
│                                │
└────────────────────────────────┘
```

#### 3.2.1 日期选择器

```
类型:               浮动日期选择条
位置:               顶部
高度:               50px
显示方式:           7天滑动视图

当前日期:
  背景:            #4CAF7F
  颜色:            #FFFFFF
  大小:            48px x 48px

其他日期:
  背景:            #F0F0F0
  颜色:            #666666
  大小:            40px x 40px

日期格式:
  上: 日期数字 (20px, 700 weight)
  下: 星期 (11px, 400 weight)

交互:
  左右箭头可滑动
  点击切换日期
  左滑/右滑手势支持
```

#### 3.2.2 打卡项目卡片

```
结构:
┌─────────────────────────────┐
│ █ [图标] 类别    [完成度徽章]│
├─────────────────────────────┤
│ 时间 | 详细信息              │
│ 内容描述                     │
│ [编辑] [删除]               │
└─────────────────────────────┘

色条:
  饮食:    #FF6B6B
  运动:    #4ECDC4
  睡眠:    #6C5CE7
  心理:    #A29BFE

完成度徽章:
  完成:    绿色 ✓ + "已完成"
  进行:    橙色 ● + "进行中"
  未开始:  灰色 ○ + "待开始"

时间格式:
  格式: HH:MM (14px, 600 weight)
  完成时间: 09:15
  或时间段: 10:00-11:00

内容:
  最多2行
  字体: 13px, 400 weight
  颜色: #666666

操作按钮:
  编辑: 文字按钮
  删除: 文字按钮, 颜色 #FF7875
  点击卡片可展开详情
```

---

### 3.3 规划详情页面

#### 整体结构

```
┌────────────────────────────────┐
│ [返回] 2026-05-23 的规划       │ (标题栏)
├────────────────────────────────┤
│ 【今日目标】4/6 完成           │ (总进度)
│ [进度条 66%]                  │
├────────────────────────────────┤
│ 📌 上午                        │ (时段)
│   🍎 早餐 [8:00-9:00]         │ (规划项)
│      鸡蛋、燕麦、牛奶          │
│      [✓ 完成] [调整] [删除]  │
│                                │
│   🏃 晨运 [9:00-10:00]        │
│      瑜伽 30分钟               │
│      [◯ 未开始] [调整]        │
│                                │
│ ────────────────────────────  │
│ 📌 下午                        │
│   ...                          │
│                                │
│ ────────────────────────────  │
│ 📌 晚间                        │
│   ...                          │
│                                │
│ [+ 添加规划项] [AI重新规划]   │
│                                │
└────────────────────────────────┘
```

#### 3.3.1 规划项目卡片（详细版）

```
结构:
┌──────────────────────────────┐
│ █ [图标 24x24] 标题  [徽章]  │ (标题行)
├──────────────────────────────┤
│ 🕐 时间段 HH:MM-HH:MM        │ (时间行)
│ 📝 详细描述内容               │
│ [目标] • [提示] • [重要性]    │
├──────────────────────────────┤
│ [✓完成] [◯开始] [调整] [删除]│
└──────────────────────────────┘

徽章:
  完成: 绿色背景 + ✓
  进行中: 橙色背景 + ●
  未开始: 灰色背景 + ○

时间格式:
  字体: 13px, 600 weight
  颜色: #666666

描述:
  字体: 14px, 400 weight
  颜色: #333333
  最多3行

标签:
  字体: 11px
  背景: 浅色
  间距: 8px

操作按钮:
  高度: 32px
  宽度: 自适应
  排列: 左对齐
  样式: 混合按钮（主+次级）
```

---

### 3.4 分析页面

#### 整体结构

```
┌─────────────────────────────────┐
│ 📊 数据分析                     │
├─────────────────────────────────┤
│ [时间范围选择] 近30天 ▼        │
├─────────────────────────────────┤
│ [健康指数趋势图]               │ (30天折线图)
├─────────────────────────────────┤
│ 【维度分析】                    │
│ 🍎 饮食     [圆形进度 75%]    │
│ 🏃 运动     [圆形进度 80%]    │
│ 😴 睡眠     [圆形进度 92%]    │
│ 🧘 心理     [圆形进度 68%]    │
├─────────────────────────────────┤
│ 【周对比图】                    │
│ [柱状图对比本周vs上周]         │
├─────────────────────────────────┤
│ 【关键指标】                    │
│ 完成率: 82%  | 连续天: 15天   │
│ 改进: +5%    | 目标: 90%      │
├─────────────────────────────────┤
│ [生成AI分析报告]               │
└─────────────────────────────────┘
```

#### 3.4.1 图表规范

**趋势图 (Trend Chart)**
```
类型:               折线图
时间跨度:           7天 / 30天 / 90天切换
X轴:                日期 (每3天显示一个标签)
Y轴:                指数 (0-100)

线条:
  颜色:             #4CAF7F
  宽度:             2px
  样式:             smooth curve

数据点:
  大小:             6px
  颜色:             #4CAF7F
  Hover 显示值

背景:
  渐变填充:         from rgba(76, 175, 127, 0.2) to transparent

网格:
  显示:             horizontal grid lines
  颜色:             #F0F0F0
  间距:             10 units
```

**圆形进度图 (Circular Progress)**
```
参考 2.4 节的定义
特殊：
  尺寸:             80px
  显示标题:         下方, 12px
  
布局 (4项):
  2x2 网格
  间距: 12px
```

**柱状对比图 (Bar Chart)**
```
类型:               分组柱状图
本周颜色:           #4CAF7F
上周颜色:           #A8E6C3 (Light)
间距:               12px
高度:               自适应

标签:
  X轴: 日期或维度
  Y轴: 完成度
  
Hover 显示具体数值
```

---

### 3.5 个人中心页面

#### 整体结构

```
┌────────────────────────────────┐
│ [×] 个人中心                   │ (顶部关闭)
├────────────────────────────────┤
│ [头像]                         │ (128x128)
│ 用户名                         │ (20px, 700 weight)
│ 等级: 青铜会员 | 连续: 28天   │
├────────────────────────────────┤
│ 【基础信息】                    │
│ [编辑个人资料]                 │
│ 性别: 女 | 年龄: 28            │
│ 身高: 165cm | 体重: 55kg      │
├────────────────────────────────┤
│ 【健康目标】                    │
│ 目标体重: 52kg  [编辑]        │
│ 每周运动: 5次   [编辑]        │
│ 目标睡眠: 8小时 [编辑]        │
├────────────────────────────────┤
│ 【偏好设置】                    │
│ 🔔 通知提醒   [开关]          │
│ 🌙 深色模式   [开关]          │
│ 🌍 语言      中文 ▼           │
├────────────────────────────────┤
│ 【关于】                        │
│ [反馈意见] [使用协议] [隐私]  │
│ 版本: v1.0.0                  │
├────────────────────────────────┤
│ [登出]                         │
└────────────────────────────────┘
```

#### 3.5.1 用户头像区域

```
结构:
┌──────────────────┐
│   [头像 128x128] │
│      用户名      │ (20px, 700 weight)
│  等级: 青铜 | 28天 │
│   [编辑] [升级]  │
└──────────────────┘

头像:
  大小:         128x128px
  Border:       4px solid #4CAF7F
  Border-radius: 50%
  Fallback:     用户首字+背景色

等级系统:
  铜级: 0-30天
  银级: 31-100天
  金级: 101-365天
  白金: 365+天

徽章:
  显示连续打卡天数
  使用火焰图标 🔥

操作按钮:
  编辑: 可修改头像和昵称
  升级: 显示升级条件 (modal)
```

#### 3.5.2 信息编辑卡片

```
类型:               可编辑的字段组
样式:               标签 + 值 + 编辑按钮

布局:
┌────────────────────────────┐
│ 性别        女      [编辑] │
│ 年龄        28      [编辑] │
│ 身高        165cm   [编辑] │
│ 体重        55kg    [编辑] │
└────────────────────────────┘

编辑行为:
  点击[编辑] 按钮
  行切换为可编辑状态
  显示保存/取消按钮
  
样式:
  标签: 12px, 600 weight, 颜色 #999999
  值:  14px, 400 weight, 颜色 #333333
  间距: 标签下方距值 4px
```

---

## 第四部分：交互行为和动画详解

### 4.1 页面过渡动画

#### 进入页面

```
名称: Page Entry
持续时间: 300ms
缓动: ease-out

动画层:
  背景淡入: opacity 0 -> 1 (300ms)
  内容上升: transform translateY(20px) -> 0 (300ms)
  
级联延迟:
  背景: 0ms
  标题: 30ms
  卡片/列表: 60ms+
  每项延迟: +30ms

初始状态:
  opacity: 0
  transform: translateY(20px)
  
最终状态:
  opacity: 1
  transform: translateY(0)
```

#### 离开页面

```
名称: Page Exit
持续时间: 200ms
缓动: ease-in

动画:
  背景淡出: opacity 1 -> 0 (200ms)
  内容下沉: transform translateY(0) -> (-20px) (200ms)
  
反向级联延迟 (列表从下到上消失)
```

#### 列表项级联入场

```
使用场景: 列表加载、搜索结果
时长: 每项 250ms
间隔: 40ms

示例:
项1: 0-250ms
项2: 40-290ms
项3: 80-330ms
...

动画参数同页面进入，但每项独立
```

### 4.2 按钮交互动画

#### 主按钮点击反馈

```
IDLE (静止):
  scale: 1
  opacity: 1
  box-shadow: Elevation 2

HOVER (悬停):
  transition: 150ms ease-out
  scale: 1.02
  box-shadow: Elevation 3
  background: lighter shade

PRESS (按下):
  transition: 50ms ease-in
  scale: 0.98
  box-shadow: Elevation 1

FOCUS (获焦):
  box-shadow: 0 0 0 4px rgba(76, 175, 127, 0.3)

DISABLED (禁用):
  opacity: 0.6
  scale: 1
  cursor: not-allowed
```

#### 次级按钮点击反馈

```
类似主按钮，但:
  hover 时 background 变深一级
  不改变 scale (可选，改变 scale 为 1.01)
  shadow 变化幅度小一些
```

### 4.3 卡片交互动画

#### 可点击卡片

```
IDLE:
  transform: translateY(0)
  box-shadow: Elevation 1

HOVER:
  transition: all 200ms ease-out
  transform: translateY(-4px)
  box-shadow: Elevation 3

PRESS:
  transition: all 100ms ease-in
  transform: scale(0.98)
  box-shadow: Elevation 1
```

#### 可展开卡片

```
点击前:
  max-height: 100px
  overflow: hidden

点击后:
  transition: max-height 300ms ease-out
  max-height: auto

箭头旋转:
  rotate(0) -> rotate(180deg)
  duration: 300ms ease-out
```

### 4.4 加载状态动画

#### 旋转加载器

```
name: spin
animation: rotate 360deg 2000ms linear infinite

SVG 使用:
<svg class="spinner">
  <circle cx="50" cy="50" r="45" stroke-dasharray="283" stroke-dashoffset="283"/>
</svg>

CSS:
@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}
```

#### 呼吸加载器（用于较长加载）

```
name: breathe
animation: breathe 2000ms ease-in-out infinite

@keyframes breathe {
  0%, 100% { opacity: 0.4; }
  50% { opacity: 1; }
}
```

#### 骨架屏加载

```
骨架元素:
  background: linear-gradient(
    90deg,
    #f0f0f0,
    #e0e0e0,
    #f0f0f0
  )
  background-size: 200% 100%
  animation: shimmer 2000ms infinite ease-in-out
  
@keyframes shimmer {
  0% { background-position: -100% 0; }
  100% { background-position: 100% 0; }
}
```

### 4.5 打卡完成反馈动画

#### 庆祝动画（点击完成时）

```
持续时间: 1500ms
序列:
  1. 0-300ms: 卡片放大 + 旋转
     transform: scale(1) -> scale(1.1) + rotate(0->5deg)
  
  2. 0-400ms: 彩纸飘落（粒子）
     30个粒子从卡片上方落下
     动画: translateY(0) -> (200px)
     旋转: 0 -> 360deg
     不透明: 1 -> 0
     缓动: ease-in
  
  3. 300-1500ms: 成功徽章弹出
     scale(0) -> scale(1)
     在卡片右下角
     缓动: cubic-bezier(0.34, 1.56, 0.64, 1)
  
  4. 最后: 回到正常状态
```

#### 连续打卡提醒

```
显示条件: 连续打卡天数 +1 时

动画:
  位置: 屏幕中上方
  背景: 渐变 #4CAF7F
  文字: "🔥 连续第28天!"
  
  进入: scale(0.8) -> 1 + fadeIn (300ms ease-out)
  显示: 2000ms
  退出: fadeOut (200ms ease-in)
  
缓动: cubic-bezier(0.34, 1.56, 0.64, 1) (弹簧)
```

### 4.6 进度条动画

#### 进度更新

```
持续时间: 300ms
缓动: ease-out
属性: stroke-dashoffset

从 0% 到 75%:
  0ms:   dashoffset = 283 (对应0%)
  300ms: dashoffset = 70 (对应75%)
```

#### 加载中的进度条

```
样式: 横向条纹动画

@keyframes progressMove {
  0% {
    background-position: -100px 0;
  }
  100% {
    background-position: 100px 0;
  }
}

background: 
  linear-gradient(
    90deg,
    #4CAF7F,
    #52C41A,
    #4CAF7F
  )
  
animation: progressMove 1.5s linear infinite
```

---

## 第五部分：响应式布局

### 5.1 断点定义

```
Mobile Small:      320px - 480px  (xs)
Mobile Large:      480px - 768px  (sm)
Tablet:            768px - 1024px (md)
Desktop:           1024px - 1440px (lg)
Desktop Large:     1440px+        (xl)
```

### 5.2 容器查询和栅格

#### 移动设备（< 768px）

```
宽度:            100% - 2 × 16px padding
列数:            单列
卡片间距:        12px
字体缩小:        -2px (可选)

布局:
┌──────────────┐
│    [Card]    │
├──────────────┤
│    [Card]    │
├──────────────┤
│  [4 Buttons] │ (排成 2x2)
└──────────────┘
```

#### 平板设备（768px - 1024px）

```
宽度:            100% - 2 × 24px padding
列数:            双列 (某些卡片)
卡片间距:        16px

布局:
┌────────────┬────────────┐
│   [Card]   │   [Card]   │
├────────────┼────────────┤
│   [Card 1] │ [Card 2]   │
└────────────┴────────────┘
```

#### 桌面设备（> 1024px）

```
宽度:            1200px (max-width) 或 100% (流动)
列数:            3列或更多
卡片间距:        20px

布局:
┌──────────┬──────────┬──────────┐
│ [Card]   │ [Card]   │ [Card]   │
├──────────┼──────────┼──────────┤
│ [C1]     │ [C2]     │ [C3]     │
└──────────┴──────────┴──────────┘
```

### 5.3 特殊响应

**列表到网格切换**
```
< 768px:  单列列表
768-1440px: 2列网格
> 1440px:  3列网格
```

**导航响应**
```
< 768px:  底部导航栏 (mobile)
> 768px:  侧边导航栏 (desktop)
```

**字体响应**
```
标题:     24px (mobile) -> 28px (desktop)
正文:     14px (both, 不变)
小文本:   12px (both, 不变)
```

---

## 第六部分：黑暗模式支持

### 6.1 颜色映射（暗色主题）

```
Light Mode         Dark Mode
─────────────────────────────────
#FFFFFF (bg)    -> #1A1A1A
#FAFAFA (bg)    -> #252525
#F0F0F0 (bg)    -> #333333
#E8E8E8 (border)-> #555555

#1A1A1A (text)  -> #FFFFFF
#333333 (text)  -> #E0E0E0
#666666 (text)  -> #B0B0B0
#999999 (text)  -> #808080

#4CAF7F (primary) -> #66BB6A (略微调亮)
#F4A261 (accent)  -> #F8B88D (保持温暖)
```

### 6.2 切换机制

```
检测系统偏好:
  媒体查询: prefers-color-scheme: dark
  
用户选择:
  设置中添加 [深色模式] 开关
  存储到 localStorage
  优先级: 用户选择 > 系统偏好

实现方式:
  CSS 变量 或 主题类名
  动画过渡: 200ms ease-out
```

---

## 第七部分：无障碍 (A11y)

### 7.1 键盘导航

```
Tab 键:         依次聚焦交互元素
Shift+Tab:      反向聚焦
Enter:          激活按钮
Space:          切换复选框
Escape:         关闭模态框
```

### 7.2 焦点样式

```
标准焦点:
  box-shadow: 0 0 0 3px rgba(76, 175, 127, 0.3)
  outline: 2px solid #4CAF7F
  outline-offset: 2px
```

### 7.3 屏幕阅读器

```
ARIA labels:
  按钮: aria-label="完成打卡"
  图标: role="img" aria-label="成功"
  进度: aria-valuenow="75" aria-valuemin="0" aria-valuemax="100"
```

---

## 第八部分：代码实现建议

### 8.1 Qt/QSS 文件结构建议

```
resources/
├── style.qss (主样式表)
├── colors.qss (颜色变量定义)
├── animations.qss (动画规则)
└── components/
    ├── buttons.qss
    ├── cards.qss
    ├── inputs.qss
    └── progress.qss
```

### 8.2 C++ 代码建议

```cpp
// designtokens.h 扩展
namespace DesignTokens {
  // 新增颜色
  inline QString primaryDark() { return "#2D7A5E"; }
  inline QString primaryLight() { return "#6BC48F"; }
  inline QString successColor() { return "#52C41A"; }
  
  // 新增动画时长
  inline constexpr int AnimationFast = 150;
  inline constexpr int AnimationBase = 250;
  inline constexpr int AnimationSlow = 400;
  
  // 新增间距和圆角
  inline constexpr int RadiusMedium = 12;
  inline constexpr int SpaceXl = 24;
}
```

### 8.3 QML/动画示例

```qml
// 按钮悬停动画
PropertyAnimation on scale {
  to: 1.02
  duration: 150
  easing.type: Easing.OutQuad
}

// 列表入场动画
SequentialAnimationGroup {
  NumberAnimation { target: item; property: "opacity"; from: 0; to: 1; duration: 250 }
  NumberAnimation { target: item; property: "y"; from: 20; to: 0; duration: 250 }
}
```

---

## 第九部分：设计验收标准

### 9.1 视觉一致性检查清单

- [ ] 所有颜色使用来自色板定义
- [ ] 所有间距使用 8px 的倍数
- [ ] 所有圆角使用预定义值 (xs/sm/md/lg/xl)
- [ ] 所有字体和字重符合排版系统
- [ ] 所有阴影符合阴影系统定义
- [ ] 所有动画使用预定义的时长和缓动

### 9.2 交互一致性检查清单

- [ ] 所有按钮有清晰的 hover/press 反馈
- [ ] 所有输入框有焦点样式和验证反馈
- [ ] 所有加载状态有明确的动画提示
- [ ] 所有成功/错误状态有对应的颜色和图标
- [ ] 所有页面过渡有相同的动画模式

### 9.3 响应式检查清单

- [ ] 在 320px 宽度显示正确
- [ ] 在 768px 宽度显示正确
- [ ] 在 1440px 宽度显示正确
- [ ] 所有文本可读，无截断（除非必要）
- [ ] 所有交互元素可点击（最小 44px）

### 9.4 无障碍检查清单

- [ ] 所有交互元素有焦点样式
- [ ] 颜色对比度 >= 4.5:1
- [ ] 所有图标有替代文本
- [ ] 页面支持键盘导航

---

## 附录：开发任务拆分建议

### 阶段1：基础设计系统实现（2-3天）

```
Task 1.1: 升级 designtokens.h
  - 添加所有新颜色定义
  - 添加动画时长常量
  - 添加间距和圆角常量

Task 1.2: 重写 style.qss
  - 应用新的配色方案
  - 优化排版规则
  - 添加响应式规则

Task 1.3: 创建公共动画规则
  - 在 motionhelper 中实现动画定义
  - 测试基础交互动画
```

### 阶段2：核心组件升级（3-4天）

```
Task 2.1: 升级按钮组件
  - 实现所有按钮类型
  - 添加完整的状态反馈
  - 优化尺寸变体

Task 2.2: 升级输入组件
  - 重新设计输入框样式
  - 添加焦点/错误状态
  - 优化可用性

Task 2.3: 创建新卡片组件
  - 带色条的卡片
  - 健康维度卡片
  - 空状态卡片
```

### 阶段3：页面布局优化（2-3天）

```
Task 3.1: 重构首页仪表盘
  - 实现欢迎区域
  - 集成健康指数卡片
  - 添加快速打卡区域

Task 3.2: 优化数据可视化
  - 集成趋势图表
  - 实现圆形进度条
  - 添加统计视图

Task 3.3: 完善打卡记录页面
  - 日期选择器
  - 时间卡片列表
  - 编辑交互
```

### 阶段4：交互和动画打磨（2-3天）

```
Task 4.1: 页面过渡动画
  - 实现所有页面进入/离开动画
  - 级联延迟
  - 测试性能

Task 4.2: 微交互完善
  - 按钮反馈
  - 卡片交互
  - 加载状态

Task 4.3: 成就反馈
  - 庆祝动画
  - 连续提醒
  - Toast 通知
```

### 阶段5：响应式和黑暗模式（1-2天）

```
Task 5.1: 响应式适配
  - 测试各个断点
  - 优化布局
  - 完善触摸交互

Task 5.2: 黑暗模式
  - CSS 变量或主题类
  - 颜色映射
  - 用户切换选项
```

---

**文档版本:** v1.0  
**最后更新:** 2026-05-23  
**维护者:** UI设计团队

---

此文档是完整的UI升级规范，包含所有必要的设计细节、尺寸、颜色值、动画参数等。开发团队可直接依据此规范进行实现。

