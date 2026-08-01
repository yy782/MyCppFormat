# MyCppFormat - 项目需求文档

## 项目概述

MyCppFormat 是一个 C++ 代码格式化命令行工具，用于将 C++ 源代码按指定规范自动格式化。

## 命令行执行方式

```
./MyCppFormat filename.cpp
```

- 直接修改原文件
- 自动备份原文件为 `filename.cpp.bak`

## 格式规范

### 规范 1：指针/引用对齐

在变量声明/定义、函数形参中，`*` 和 `&` 紧贴变量名。

| 输入 | 输出 |
|------|------|
| `int* a;` | `int *a;` |
| `int * a;` | `int *a;` |
| `double& b;` | `double &b;` |
| `void func(int* a, double& b);` | `void func(int *a, double &b);` |
| `char** argv;` | `char **argv;` |
| `int*& p;` | `int *&p;` |

### 规范 2：逗号分隔

逗号后面不加空格。

| 输入 | 输出 |
|------|------|
| `func(a, b, c)` | `func(a,b,c)` |
| `int a, b, c;` | `int a,b,c;` |

### 规范 3：分号分隔

分号后面不加空格。

| 输入 | 输出 |
|------|------|
| `for (int i = 0; i < 10; ++i)` | `for (int i = 0;i < 10;++i)` |

### 规范 4：短函数体

单行函数体去除大括号内部首尾空格。

| 输入 | 输出 |
|------|------|
| `{ return true; }` | `{return true;}` |

### 规范 5：括号内空格

去除 `if`、`for`、`while` 等括号内首尾空格。

| 输入 | 输出 |
|------|------|
| `if ( x > 0 )` | `if (x > 0)` |

### 规范 6：关键字空格

`if`、`for`、`while` 等与左括号之间保留一个空格。

| 输入 | 输出 |
|------|------|
| `if(condition)` | `if (condition)` |

## 正确性保障

- 不修改字符串字面量、字符字面量、注释内的内容
- 不修改预处理器指令（`#include`、`#define` 等）
- 不改变代码语义（如区分 `int *a` 声明和 `a * b` 乘法运算）

### 已知难点：`#define` 宏体处理

#### 困境描述

tree-sitter C++ 解析器对预处理指令的支持不完善。在解析 `#define` 时，宏体中的 token 可能**不被归类为 `preproc_def` / `preproc_arg` 的子节点**，而是被 tree-sitter 当作普通 C++ token 直接挂在 `translation_unit` 层。

这导致格式化工具无法可靠区分"这是宏体内的 token"还是"这是普通代码的 token"。如果对所有 token 一视同仁地应用空白符规则（如逗号去空格），会将宏体内原本合法的分隔空格错误删除，例如：

```
// 原始 #define 宏体
#define FU2_DEFINE_FUNCTION_TRAIT(CONST, VOLATILE, NOEXCEPT, OVL_REF, REF)

// 被格式化工具错误处理为（逗号规则将空格删除）
#define FU2_DEFINE_FUNCTION_TRAIT(CONST,VOLATILE,NOEXCEPT,OVL_REF,REF)
//                                   ↑ 宏体参数之间的分隔空格丢失
```

更严重的案例是相邻标识符之间的空格被删除，如 `VOLATILE OVL_REF` → `VOLATILEOVL_REF`，导致宏编译失败。

#### 当前方案：宏体掩码（Macro Body Masking）

**暂时不解析宏体**，在 tree-sitter 解析前将宏体替换为等长空白（保留 `\` 续行符和换行符），格式化完成后再从原始源码还原宏体内容。

流程：
1. **预扫描**：用行扫描器找出所有 `#define` 的 body 字节范围（支持 `\` 续行）
2. **掩码**：将 body 中非 `\`、非换行的字符替换为等长空格
3. **解析**：tree-sitter 对掩码源解析，宏体区域不会产生 C++ token
4. **格式化**：在输出重建阶段，若当前 token gap 与任意宏体范围有交集，则保持原始空白不变；否则应用格式化规则
5. **还原**：所有 token 文本和 gap 均从原始源码读取，宏体被完整保留

#### 当前限制

- `#if` / `#ifdef` / `#ifndef` / `#else` / `#endif` 条件编译块不受影响（不含 `#define` 宏体，无需特殊处理）
- 宏体内部的代码（如内联模板、函数定义）不会被格式化，保留原始排版
- 函数式宏的形参列表（`(...)`）仍会被格式化（属于可接受行为）

## 测试方案

### 单元测试（Google Test）

每个规则函数独立测试，验证各种边界情况。

目录：`tests/unit/`

### 集成测试（diff 对比 + 编译检查）

每个规范单独准备一个测试用例目录，包含 `input.cpp` 和 `expected.cpp`。

目录：`tests/integration/cases/`

每个 `input.cpp` 是完整可编译的 C++ 程序：
- 格式化后先 `diff` 对比 `output.cpp` 和 `expected.cpp`
- diff 通过后，再用 `g++ -fsyntax-only` 做编译检查
- 两种检查都通过才算该测试用例通过

## 开发顺序

| 序号 | 模块 |
|------|------|
| 1 | 项目初始化 |
| 2 | 主程序入口（命令行参数、文件读写、备份） |
| 3 | 指针/引用对齐 |
| 4 | 逗号规则 |
| 5 | 分号规则 |
| 6 | 括号空格规则 |
| 7 | 关键字空格规则 |
| 8 | 短函数体规则 |
| 9 | 整体联调 |
