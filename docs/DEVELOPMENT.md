# MyCppFormat - 开发规范文档

## 分支策略

- `main`：稳定分支，所有代码通过 PR 合并
- 开发分支从 `main` 拉出，命名遵循规则

### 分支命名规则

| 类型 | 格式 | 示例 |
|------|------|------|
| 新功能 | `feature/<功能名>` | `feature/comma`、`feature/semicolon` |
| 文档 | `docs/<文档名>` | `docs/add-documentation` |
| 修复 | `fix/<修复名>` | `fix/pointer-alignment` |
| CI/配置 | `ci/<配置名>` | — |

## 开发流程

```
main ──→ git checkout -b feature/xxx main
  │
  ├── 编写代码 + 测试
  │
  ├── git commit（提交信息使用中文）
  │
  ├── git push origin feature/xxx
  │
  └── 创建 PR：feature/xxx → main
       │
       └── 审查通过后合并到 main
```

1. 从 `main` 拉出新分支
2. 在分支上按 TDD 方式开发（先写测试，再实现功能）
3. 每个模块独立测试、独立提交
4. 推送到远端，创建 PR 合并到 `main`

## 提交信息格式

提交信息使用中文，格式为 `<type>: <description>`。

| 类型 | 说明 | 示例 |
|------|------|------|
| `chore` | 项目初始化、构建配置 | `chore: 项目初始化` |
| `feat` | 新功能 | `feat(pointer): 实现指针/引用对齐功能` |
| `test` | 添加测试 | `test(pointer): 添加指针/引用对齐测试用例` |
| `fix` | 修复 bug | `fix(pointer): 修复指针对齐边界情况` |
| `docs` | 文档变更 | `docs: 添加项目需求文档和开发规范文档` |
| `ci` | CI 配置 | `ci: 添加 GitHub CI 工作流配置` |

## 代码规范

- 使用 **C++17** 标准
- 采用 **TDD（测试驱动开发）** 方式
- CMake 管理构建
- 每个模块独立开发、独立测试、独立提交
- 每个 `feat` 提交之前，确保对应的单元测试已经通过

## CI 要求

GitHub Actions 自动运行三个 job：

| Job | 说明 |
|------|------|
| `build` | 构建项目，安装依赖，上传构建产物 |
| `unit-test` | 运行 GTest 单元测试（`ctest -R "unit.*"`) |
| `integration-test` | 运行集成测试（diff + g++ 编译检查） |

触发条件：
- `push` 到 `main` 或 `develop` 分支
- `pull_request` 到 `main` 或 `develop` 分支

## 项目结构

```
MyCppFormat/
├── CMakeLists.txt
├── .gitignore
├── .github/workflows/ci.yml
├── docs/
│   ├── REQUIREMENTS.md
│   └── DEVELOPMENT.md
├── src/
│   ├── main.cpp
│   ├── formatter.hpp
│   └── formatter.cpp
└── tests/
    ├── unit/
    │   ├── CMakeLists.txt
    │   └── test_pointer.cpp
    └── integration/
        ├── run_tests.sh
        └── cases/
            ├── 01_pointer/
            ├── 02_comma/
            ├── ...
            └── 07_mixed/
```
