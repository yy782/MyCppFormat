# MyCppFormat

MyCppFormat 是一个基于 tree-sitter 的 C++ 代码格式化命令行工具，按照指定规范自动格式化 C++ 源代码。

## 特性

- **指针/引用对齐**：声明中的 `*` / `&` / `&&` 紧贴变量名
- **逗号去空格**：逗号后移除多余空格
- **分号去空格**：分号后移除多余空格（注释除外）
- **短函数体紧凑**：单行函数体大括号内去除首尾空格
- **括号内去空格**：控制流关键字括号内首尾空格
- **关键字空格**：`if` / `for` / `while` / `switch` / `catch` / `do` 与 `(` 之间保留一个空格
- **宏体保护**：`#define` 宏体内容不被格式化，通过掩码机制保留原始排版

## 快速开始

### 依赖

- CMake >= 3.14
- C++17 编译器（g++ / clang++）
- Git（用于拉取 tree-sitter 子模块）

### 构建

```bash
git clone https://github.com/yy782/MyCppFormat.git
cd MyCppFormat
cmake -B build
cmake --build build
```

### 使用

```bash
./build/src/codeformat filename.cpp
```

- 直接修改原文件
- 自动备份原文件为 `filename.cpp.bak`

### 清理备份文件

格式化后会在同级目录生成 `.bak` 备份文件，使用 `clean_bak.sh` 脚本批量清理：

```bash
# 清理当前目录下所有 .bak 文件
bash scripts/clean_bak.sh

# 清理指定目录下所有 .bak 文件
bash scripts/clean_bak.sh tests/
```

脚本会先列出所有待删除文件，确认后才执行删除。

## 格式规范

### 规范 1：指针/引用对齐

`*` 、 `&` 和 `&&` 在声明中紧贴变量名或标识符。

| 输入 | 输出 |
|------|------|
| `int* a;` | `int *a;` |
| `int * a;` | `int *a;` |
| `double& b;` | `double &b;` |
| `void func(int *a, double &b);` | `void func(int *a, double &b);` |
| `char** argv;` | `char **argv;` |
| `int*& p;` | `int *&p;` |
| `void f(T &&x)` | `void f(T &&x)` |

> 返回类型中的 `*` / `&` / `&&`（如 `V* operator->()`）不受此规则影响，仅参数声明和变量声明被格式化。

### 规范 2：逗号分隔

逗号后面不加水平空格。

| 输入 | 输出 |
|------|------|
| `func(a, b, c)` | `func(a,b,c)` |
| `int a, b, c;` | `int a,b,c;` |

> 模板参数列表（`<...>`）内的逗号不受此规则影响，保留原样。

### 规范 3：分号分隔

分号后面不加水平空格，注释例外。

| 输入 | 输出 |
|------|------|
| `for (int i = 0; i < 10; ++i)` | `for (int i = 0;i < 10;++i)` |

### 规范 4：短函数体

单行函数体（包括 lambda）去除大括号内部首尾空格。

| 输入 | 输出 |
|------|------|
| `{ return true; }` | `{return true;}` |
| `[](){ return 0; }` | `[](){return 0;}` |

### 规范 5：括号内空格

去除 `if`、`for`、`while` 等控制流括号内首尾空格。

| 输入 | 输出 |
|------|------|
| `if ( x > 0 )` | `if (x > 0)` |
| `for ( int i = 0; i < 10; ++i )` | `for (int i = 0;i < 10;++i)` |

### 规范 6：关键字空格

控制流关键字与左括号之间保留一个空格。

| 输入 | 输出 |
|------|------|
| `if(condition)` | `if (condition)` |
| `switch(val)` | `switch (val)` |
| `catch(...)` | `catch (...)` |
| `do{...}while(cond)` | `do{...}while (cond)` |

## 正确性保障

- 不修改字符串字面量、字符字面量、注释内的内容
- 不修改预处理器指令（`#include`、`#define` 宏体等）
- 不改变代码语义（区分声明上下文与表达式上下文）

### 宏体保护机制

tree-sitter C++ 解析器对 `#define` 预处理指令中的宏体 token 归类不完善。为避免误格式化宏体内容，采用**宏体掩码**方案：

1. 预扫描所有 `#define` 宏体字节范围（支持 `\` 续行）
2. 将宏体非换行字符替换为等长空格
3. tree-sitter 对掩码后源码解析，宏体区域不产生 C++ token
4. 格式化后从原始源码还原宏体内容

## 架构

基于 [tree-sitter](https://tree-sitter.github.io/) C++ 解析器，通过遍历 CST（具体语法树）的 token 序列，在 token 间隙（gap）中应用空白符格式化规则。

```
源码 → 宏体掩码 → tree-sitter 解析 → token 遍历 → 空白符规则 → 输出
```

## 测试

### 单元测试

```bash
cmake --build build
ctest --test-dir build -R "unit.*"
```

### 集成测试

```bash
bash tests/integration/run_tests.sh
```

每个测试用例目录包含 `input.cpp` 和 `expected.cpp`，格式化后通过 `diff` 对比 + `g++ -fsyntax-only` 编译检查。

## 开发

详见 [docs/DEVELOPMENT.md](docs/DEVELOPMENT.md)。
