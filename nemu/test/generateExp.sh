gcc randomExpression.c -o randomExpression
: > calculatorTestFile

# 执行文件，读取输出
./randomExpression $1 | while IFS= read -r line; do
    # 判断是否有错误
    errorIF=$(echo "$line" | bc 2>&1 1>/dev/null)
    # 如果有错误，则输出;没错误，则写入文件
    if [[ -n "$errorIF" ]]; then
        continue
    fi
    result=$(echo "$line" | bc)
    echo "$result $line" >> calculatorTestFile
done

