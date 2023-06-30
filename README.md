# SuduKu
实现一个能够生成数独游戏并求解数独问题的控制台程序
| 测试样例                     | 测试原因         |
| ---------------------------- | ---------------- |
| SudoKu.exe -c 10             | 数独终盘测试     |
| SudoKu.exe -s                | 数独解生成       |
| SudoKu.exe -s sudoku_game.txt | 正常解数独             |
| SudoKu.exe -s abcdefg.txt     | 没有可解数独文件            |
| SudoKu.exe -n 10             | 所需数独游戏数量 |
| SudoKu.exe -n 10 -m 2        | 数独难度测试     |
| SudoKu.exe -n 0 -n 0          | 不符合要求的数独游戏生成           |
| SudoKu.exe -n 10 -r 20-55    | 数独挖空数量测试 |
| SudoKu.exe -n 10 -r 20-55 -u | 数独唯一解测试   |
