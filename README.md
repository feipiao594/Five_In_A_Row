# Five_In_A_Row
NJUPT 程序设计周 · 五子棋（Gomoku）项目（Qt6 + CMake）。

| Windows | macOS | Linux |
| :-----: | :---: | :---: |
|   ✅    |  ✅   |  ✅   |

## 功能
- 双人对战
- 人机对战（启发式评估 + 简单推演）
- 悔棋、重新开始

## 构建与运行
依赖：Qt6（Core、Widgets）+ CMake（>= 3.14）+ C++17。

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

运行产物默认输出到：`bin/release/`（Release）或 `bin/debug/`（Debug）。

## 文档（翻新/重构用）
- [docs/项目文档.md](docs/项目文档.md)
- [docs/翻新计划.md](docs/翻新计划.md)
- [docs/开发指南.md](docs/开发指南.md)

## 备注
- v0.0.1-beta 为程序设计周阶段版本（3 人、4 天）。
- Windows 可能需要 Microsoft Visual C++ 运行时库（缺失时会无法启动）。
