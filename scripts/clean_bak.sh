#!/bin/bash
set -e

# clean_bak.sh - 清理指定目录下所有 .bak 文件
# 用法: bash scripts/clean_bak.sh [目录路径]
# 默认清理当前目录

TARGET_DIR="${1:-.}"

if [ ! -d "$TARGET_DIR" ]; then
    echo "错误: 目录不存在: $TARGET_DIR"
    exit 1
fi

echo "正在扫描 $TARGET_DIR 下的 .bak 文件..."

# 统计文件数量
count=$(find "$TARGET_DIR" -type f -name "*.bak" | wc -l)

if [ "$count" -eq 0 ]; then
    echo "未找到任何 .bak 文件。"
    exit 0
fi

echo "找到 $count 个 .bak 文件："
find "$TARGET_DIR" -type f -name "*.bak" -print

echo ""
read -p "确认删除以上文件？(y/N): " confirm
if [ "$confirm" != "y" ] && [ "$confirm" != "Y" ]; then
    echo "已取消。"
    exit 0
fi

find "$TARGET_DIR" -type f -name "*.bak" -delete
echo "已删除 $count 个 .bak 文件。"
