# Co-minesweeper 合作扫雷

基于 EGE 图形库的扫雷游戏，支持单人模式和局域网双人联机合作模式。

## 功能特性

- **单人模式** — 经典扫雷玩法，左键翻开、右键插旗、双击数字自动翻开周围
- **联机合作模式** — 一人创建主机，另一人通过 IP 加入，双方共同扫雷
- **多种难度** — 简单 / 中等 / 困难 / 自定义
- **图形界面** — 基于 EGE 的鼠标交互界面

## 环境要求

| 依赖 | 版本/说明 |
|------|----------|
| 操作系统 | Windows |
| 编译器 | MinGW-w64 (GCC 15.2+) |
| 构建工具 | CMake 3.10+ |
| 图形库 | [EGE](https://github.com/wysaid/ege) 25.11.0 |
| 网络库 | Winsock2 (系统自带) |

## 构建步骤

1. **安装 EGE**

   下载 EGE 25.11.0，放到指定路径（默认为 `E:/Code/Cpp/lib/ege-25.11.0`）。如果路径不同，修改 `CMakeLists.txt` 中第 7 行的 `EGE_LIB_PATH`。

2. **配置 CMake**

   ```bash
   cmake -S . -B build -G "MinGW Makefiles"
   ```

3. **编译**

   ```bash
   cmake --build build -j 16
   ```

4. **运行**

   可执行文件生成在 `bin/Minesweeper.exe`。

## 项目结构

```
Co-minesweeper/
├── include/                # 头文件
│   ├── button.h            # 按钮 UI 组件
│   ├── func.h              # 通用工具函数与常量
│   ├── gamemode.h          # 游戏模式（单人/主机/客户端）
│   ├── gamerender.h        # 游戏棋盘渲染
│   ├── gamesystem.h        # 控制台版游戏系统
│   ├── minesweeper.h       # 扫雷核心逻辑
│   ├── network.h           # 网络通信（TCP）
│   └── startscreen.h       # 启动画面与菜单
├── src/                    # 源文件
│   ├── button.cpp
│   ├── func.cpp
│   ├── gamemode.cpp
│   ├── gamerender.cpp
│   ├── gamesystem.cpp
│   ├── main.cpp            # 程序入口，仅含 main()
│   ├── Minesweeper.cpp
│   ├── network.cpp
│   └── startscreen.cpp
├── CMakeLists.txt
└── README.md
```

## 游戏操作

| 操作 | 效果 |
|------|------|
| 左键单击 | 翻开格子 |
| 右键单击 | 插旗 / 取消插旗 |
| 双击数字格 | 若周围旗数等于数字，自动翻开剩余格子 |

## 联机说明

1. 主机和客户端需在同一局域网
2. 主机选择「多人游戏」→「创建房间」
3. 客户端选择「多人游戏」→「加入房间」，输入主机 IP 地址
4. 双方共享同一棋盘，共同完成扫雷
5. 默认端口：`12345`

## 难度参数

| 难度 | 行 × 列 | 雷数 |
|------|---------|------|
| 简单 | 9 × 9 | 10 |
| 中等 | 16 × 16 | 40 |
| 困难 | 16 × 30 | 99 |
| 自定义 | 自定义 | 自定义 |