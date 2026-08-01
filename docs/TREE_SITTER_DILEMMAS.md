# tree-sitter 困境

MyCppFormat 使用 tree-sitter 作为 C++ 代码的 AST 解析器，但在实际开发中遇到了若干困境。本文档记录这些已知问题及当前的应对方案。

---

## 1. `#define` 宏体解析不完善

tree-sitter C++ 解析器对预处理指令的支持不完整。`#define` 宏体内的 token **可能不被归类为 `preproc_def` / `preproc_arg` 的子节点**，而是被当作普通 C++ token 直接挂在 `translation_unit` 层。

若不加处理直接格式化宏体，会误删宏体内合法的分隔空格，破坏宏语义。

### 当前方案：宏体掩码

1. 解析前扫描所有 `#define` 宏体范围
2. 将宏体内容替换为等长空白字符（掩码）
3. tree-sitter 解析掩码后的源码
4. 格式化完成后，从原始源码恢复宏体

---

## 2. 模板右尖括号 `>>` 产生 ERROR 节点

C++11 起允许的嵌套模板双右尖括号（如 `std::map<int, std::vector<int*>>`），tree-sitter-CPP 有时无法正确解析，在 `>>` 处产生 ERROR 子树。

### 当前方案：ERROR 子树守卫

通过 `in_error_subtree` 函数遍历祖先节点，若 token 处于 ERROR 子树内则**保持间隙原样不改动**，避免误格式化。

---

## 3. 特定 C++ 语法的 AST 结构不统一

不同 C++ 语法场景下，tree-sitter 产生的 AST 节点类型名不一致，导致同一逻辑需要适配多种类型名：

| 场景 | 实际节点类型 | 期望节点类型 |
|------|-------------|-------------|
| `Args&&...rest` 中的 `&&` | `variadic_parameter_declaration` | `parameter_declaration` |
| lambda `{ }` 的祖父节点 | `lambda_expression` | `function_definition` |

### 当前方案：多类型名匹配

在启发式规则的祖先遍历中，显式枚举所有可能的节点类型名：

```cpp
// in_parameter_context
if (std::strcmp(ptype, "parameter_declaration") == 0 ||
    std::strcmp(ptype, "variadic_parameter_declaration") == 0) return true;

// in_function_body
if (std::strcmp(gptype, "function_definition") == 0 ||
    std::strcmp(gptype, "lambda_expression") == 0) return true;
```

---

## 4. `&&` 转发引用识别不可靠

在 `Args&&...args` 这类参数包中，tree-sitter 不将 `&&` 识别为 `reference_declarator`，无法通过 AST 结构直接判断 `&&` 是否为转发引用/右值引用。

### 当前方案：启发式回退

项目维护了一套多层级的启发式逻辑，而非单一依赖 AST 节点类型：

- **主路径**：`in_pointer_or_ref_decl` 函数检测 `pointer_declarator` / `reference_declarator` 祖先
- **回退**：结合 `angle_depth`（尖括号深度，区分模板参数）、`in_parameter_context`（参数上下文）等多重条件推断
- **ERROR 守卫**：无法确定时，依靠 `in_error_subtree` 保守保持原样

引入分支多、条件耦合，新增语法覆盖时有回归风险。

---

## 5. 上游解析器的局限性

tree-sitter-CPP 是通用增量解析器，对 C++ 完整语法的覆盖仍有缺口：

- 新标准特性（C++20 Concept、C++23 语法糖等）解析可能不完整
- 遇到未知语法时产生 ERROR 节点，格式化工具只能**保守保留原样**
- 上游修复周期不可控，新语法需等待 tree-sitter-CPP 支持

---

## 总结

| 问题 | 影响 | 当前应对 | 风险 |
|------|------|---------|------|
| 宏体解析 | 破坏宏体空格 | 掩码回填 | 极端嵌套宏可能边界计算偏差 |
| `>>` ERROR | 误改模板代码 | 保持原样 | 该区域永远不格式化 |
| AST 类型不统一 | 同一逻辑需多分支 | 显式枚举类型名 | 新增语法需不断补全枚举 |
| `&&` 启发式 | 转发引用误判 | 多层回退逻辑 | 分支复杂，回归风险高 |
| 上游限制 | 新语法不支持 | 保守保持原样 | 依赖上游更新节奏 |
