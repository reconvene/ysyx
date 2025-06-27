gcc randomExpression.c -o randomExpression
: > calculatorTestFile
i=0
totalAttempts=0

# 循环直至生成指定数量的表达式为止
while (( i<$1 )); do
  # 调用函数生成表达式
  currentExpression=$(./randomExpression)
  # 如果返回非0则为生成失败
  if [[ $? -ne 0 ]]; then
    continue
  fi

  result=$(echo "$currentExpression" | bc 2>&1)
  # 计算失败或结果小于0则跳过
  if ! [[ "$result" =~ ^-?[0-9]+$ ]] || (( result < 0 )); then
    ((totalAttempts++))
    continue
  fi

  echo "$result $currentExpression" >> calculatorTestFile
  ((i++))
  ((totalAttempts++))
done

echo "Number of attempts:$totalAttempts"
echo "Final generated test cases:$i"
