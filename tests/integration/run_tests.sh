#!/bin/bash
# MyCppFormat 集成测试运行脚本
# 用法: ./run_tests.sh [MyCppFormat二进制路径]

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
FORMATTER="${1:-${BUILD_DIR}/src/MyCppFormat}"

CASES_DIR="${SCRIPT_DIR}/cases"

if [ ! -x "$FORMATTER" ]; then
    echo "错误: MyCppFormat 未找到或不可执行: $FORMATTER"
    echo "请先构建项目: cd build && cmake .. && make"
    exit 1
fi

echo "MyCppFormat 集成测试"
echo "格式化工具: $FORMATTER"
echo "测试用例目录: $CASES_DIR"
echo "========================================"

PASSED=0
FAILED=0

for case_dir in "$CASES_DIR"/*/; do
    case_name=$(basename "$case_dir")
    input="${case_dir}/input.cpp"
    expected="${case_dir}/expected.cpp"

    if [ ! -f "$input" ]; then
        continue
    fi
    if [ ! -f "$expected" ]; then
        echo "[SKIP] $case_name — 缺少 expected.cpp"
        continue
    fi

    # 复制 input 到临时文件进行格式化
    tmp=$(mktemp /tmp/mcppfmt_XXXXXX.cpp)
    cp "$input" "$tmp"

    # 运行格式化工具
    "$FORMATTER" "$tmp" > /dev/null 2>&1

    diff_output=""
    diff_passed=true
    if ! diff -u "$expected" "$tmp" > /tmp/mcppfmt_diff.txt 2>&1; then
        diff_passed=false
        diff_output=$(cat /tmp/mcppfmt_diff.txt)
    fi

    compile_passed=true
    if $diff_passed; then
        if ! g++ -std=c++20 -fcoroutines -fsyntax-only "$tmp" 2>/dev/null; then
            compile_passed=false
        fi
    fi

    if $diff_passed && $compile_passed; then
        echo "[PASS] $case_name"
        PASSED=$((PASSED + 1))
    else
        echo "[FAIL] $case_name"
        if ! $diff_passed; then
            echo "  diff 对比失败:"
            echo "$diff_output" | sed 's/^/    /'
        fi
        if $diff_passed && ! $compile_passed; then
            echo "  编译检查失败 (g++ -fsyntax-only)"
        fi
        FAILED=$((FAILED + 1))
    fi

    rm -f "$tmp" "$tmp.bak" /tmp/mcppfmt_diff.txt
done

echo "========================================"
echo "结果: $PASSED 通过, $FAILED 失败"
if [ $FAILED -gt 0 ]; then
    exit 1
fi
