#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include"getopt.h"    //注意使用方法
using namespace std;

int space[9][9];
bool flag, single_flag = false;
FILE* input, * output_solve;
int empty_location[60];

// 生成数独终盘
// 1、基本想法：
// 数独的规则是每行、每列、每个小九宫格内不出现重复的数字，其实只要严格遵守了这些规则然后按照一定的次序打乱即可。
// 2、具体实现：
// 经过分析可知只需将a在1、4、7行分别放置在1、2、3列的位置，然后2、5、8行将列数加3, 3、6、9行将列数加6，然后依次排列a~i这些数字即可。
// 将小九宫格内任意两行或两列交换仍可使整个大九宫格满足要求（当然这里除了第一行第一列不可以交换，因为左上角数值固定）。
// 为满足题目要求达到终局数量要求，只交换4~6、7~9行（列）即可。
void sudoku_endgame_create(int sudoku_num) {
    // 打开输出文件
    FILE* output_create = fopen("sudoku_endgame.txt", "w");
    // 如果打开失败，输出错误信息并退出程序
    if (!output_create) {
        printf("Cannot open the output file!\n");
        exit(1);
    }

    // 记录第一行的数字排列
    int record[9] = {0, 3, 6, 1, 4, 7, 2, 5, 8};

    // 枚举第一行的数字排列
    for (int i = 0; i < 6 && sudoku_num != 0; ++i) {
        // next_permutation是一个算法函数，用于生成给定序列的下一个排列。它通过重排列序列中的元素来生成下一个排列，如果存在的话。
        // next_permutation这个函数在C++ 的标准库中提供。
        // 如果不是第一个排列，生成下一个排列
        if (i != 0) {
            std::next_permutation(record + 3, record + 6);
        }
        // 枚举第二行的数字排列
        for (int j = 0; j < 6 && sudoku_num != 0; ++j) {
            // 如果不是第一个排列，生成下一个排列
            if (j != 0) {
                std::next_permutation(record + 6, record + 9);
            }
            // 初始化数独第一行的数字
            char num[10] = "123456789"; // 此处可以改为随机的数字
            // 枚举数独第一行的数字排列
            for (int k = 0; k < sudoku_num; ++k) {
                // 如果不是第一个排列，生成下一个排列
                if (k != 0) {
                    std::next_permutation(num + 1, num + 9);
                }
                // 遍历数独每个格子，并写入输出文件
                for (int m : record) {
                    // 写入当前格子的数字
                    fputc(num[m % 9], output_create);
                    // 写入当前格子的候选数字
                    for (int n = 1; n < 9; ++n) {
                        fputc(' ', output_create);
                        fputc(num[(n + m) % 9], output_create);
                    }
                    // 换行
                    fputc('\n', output_create);
                }
                // 如果还需要生成更多数独，加入空行
                if (--sudoku_num != 0) {
                    fputc('\n', output_create);
                }
            }
        }
        // 重置第一行的数字排列
        record[6] = 2;
        record[7] = 5;
        record[8] = 8;
    }
    // 关闭输出文件
    fclose(output_create);
}

// 生成未完成的数独
// 1、基本思想：
// 从生成数独终盘中删去部分元素，这样必定可以完成最终数独形成。
// 2、具体实现：
// 先保证随机不重复生成位置，再打印输出相关数组

// （1）保证随机不重复
void random_empty_num_location_generate(int num_empty)
{
    for(int i=1; i<=num_empty; i++)
    {
        recreate_location:
        empty_location[i]=(rand() % (80));
        for(int j=0; j<i;j++)
            //保证不重复
            if(empty_location[i]==empty_location[j])
                goto recreate_location;
    }
}

// （2）数独游戏挖空生成
void sudoku_game_creat(int num_empty)
{
    for(int i=1; i<=num_empty; i++)
    {
        int m = empty_location[i] / 9;
        int n = empty_location[i] % 9;
        space[m][n] = 0;
    }

    // 遍历整个数组，输出
    for (auto & i : space)
    {
        fputc(i[0] + '0', output_solve);
        for (int j = 1; j < 9; j++)
        {
            fputc(' ', output_solve);
            fputc(i[j] + '0', output_solve);
        }
        fputc('\n', output_solve);
    }
    fputc('\n', output_solve);
}


// 求解数独游戏
// 1、基本思路：
// 是一个DFS的过程，在九宫格中满足要求求解
// 2、具体实现：
// 用space[9][9]数组代表整个九宫格，从0~80依次编号，则第x个数在数组中的位置是space[x/9][x%9]，
// 然后只需判断这个位置是不是空，如果空则放置某个数，若该数合法则递归下一层，x = x+1；若该数不为空则直接递归下一层x = x+1。
// 在退出上一层递归时要将该位置数字“清零”，以免影响下一次放置。

// （1）该函数检查在数独棋盘上(m, n)位置是否可以放置数字x，遵守数独规则。
bool number_check_legality(int x, int m, int n)
{
    // 检查所在行
    for (int i = 0; i < 9; i++)
        if (space[m][i] == x) return false;

    // 检查所在列
    for (auto & i : space)
        if (i[n] == x) return false;

    // 检查所在宫
    int begin_i = m / 3 * 3;
    int begin_j = n / 3 * 3;
    for (int i = begin_i; i < begin_i + 3; i++)
        for (int j = begin_j; j < begin_j + 3; j++)
            if (space[i][j] == x) return false;

    // 如果可以在位置(m, n)上放置数字x，返回true
    return true;
}

// （2）数独游戏求解
void sudoku_game_solve(int count)
{
    if (flag) return;// 如果已经填满所有的格子，输出解并返回
    if (count == 81)
    {
        flag = true;
        // 遍历整个数组，输出解
        for (auto & i : space)
        {
//            if(space[i][0]==0)
//                fputc('$', output_solve);
//            else
//                fputc(space[i][0] + '0', output_solve);
            fputc(i[0] + '0', output_solve);
            for (int j = 1; j < 9; j++)
            {
                fputc(' ', output_solve);
//                if(space[i][j]==0)
//                    fputc('$', output_solve);
//                else
//                    fputc(space[i][j] + '0', output_solve);
                fputc(i[j] + '0', output_solve);
            }
            fputc('\n', output_solve);
        }
        fputc('\n', output_solve);
        return;
    }

    // 根据当前格子编号计算出行和列
    int m = count / 9;
    int n = count % 9;

    // 如果当前格子是空的，则尝试填入数字
    if (space[m][n] == 0)
    {
        // 枚举1~9，尝试填入数字
        for (int i = 1; i <= 9; i++)
        {
            // 检查当前数字是否合法
            if (number_check_legality(i, m, n))
            {
                // 如果合法，则填入数字并递归进入下一层
                space[m][n] = i;
                sudoku_game_solve(count + 1);
            }
        }
        // 如果1~9都不合法，则回溯，将当前格子重新置为空
        space[m][n] = 0;
    }
    else
    {
        // 如果当前格子不为空，则直接递归进入下一层
        sudoku_game_solve(count + 1);
    }
}

inline int to_integer(char* arg)
{
    int ret = 0;
    while (*arg)
    {
        if (!isdigit(*arg)) return -1;
        ret = ret * 10 + *arg - '0';
        arg++;
    }
    return ret;
}

// 根据给定的难度生成数独
// 1、基本思路：
// 根据不同挖空数量，确定难度系数，难度系数有： 1 2 3 ，共3档
void create_sudoku_with_difficulty(int sudoku_num, int difficulty)
{
    int num_empty = difficulty * 10 + 10 + rand() % 10;

    sudoku_endgame_create(sudoku_num);

    input = fopen("sudoku_endgame.txt", "r");
    output_solve = fopen("sudoku_game.txt", "w");

    while (fscanf(input, "%d", &space[0][0]) != EOF)
    {
        for (int j = 1; j < 9; j++)
            fscanf(input, "%d", &space[0][j]);
        for (int i = 1; i < 9; i++)
            for (int j = 0; j < 9; j++)
                fscanf(input, "%d", &space[i][j]);
        flag = false;
        random_empty_num_location_generate(num_empty);
        sudoku_game_creat(num_empty);
    }
}

// 生成游戏的解唯一
// 1、基本思路：
// 先生成数独，然后找寻所有数独解，判断解是否唯一

void single_check(int count)
{
    if (!single_flag) return;// 如果已经填满所有的格子，输出解并返回
    if (count == 81)
    {
        if(!flag) {
            flag = true;
        }
        else {
            single_flag = false;
        }
        return;
    }

    // 根据当前格子编号计算出行和列
    int m = count / 9;
    int n = count % 9;

    // 如果当前格子是空的，则尝试填入数字
    if (space[m][n] == 0)
    {
        // 枚举1~9，尝试填入数字
        for (int i = 1; i <= 9; i++)
        {
            // 检查当前数字是否合法
            if (number_check_legality(i, m, n))
            {
                // 如果合法，则填入数字并递归进入下一层
                space[m][n] = i;
                single_check(count + 1);
            }
        }
        // 如果1~9都不合法，则回溯，将当前格子重新置为空
        space[m][n] = 0;
    }
    else
    {
        // 如果当前格子不为空，则直接递归进入下一层
        single_check(count + 1);
    }
}

void sudoku_generate(int num_empty) {
    for(int i=1; i<=num_empty; i++)
    {
        int m = empty_location[i] / 9;
        int n = empty_location[i] % 9;
        space[m][n] = 0;
    }
}

void create_sudoku_with_single_solution(int sudoku_num)
{
    int num_empty = 20;
    sudoku_endgame_create(sudoku_num);

    input = fopen("sudoku_endgame.txt", "r");
    output_solve = fopen("sudoku_game.txt", "w");

    int cnt = 1;

    while (fscanf(input, "%d", &space[0][0]) != EOF)
    {
        for (int j = 1; j < 9; j++)
            fscanf(input, "%d", &space[0][j]);
        for (int i = 1; i < 9; i++)
            for (int j = 0; j < 9; j++)
                fscanf(input, "%d", &space[i][j]);
        single_flag = false;
        while(!single_flag)
        {
            random_empty_num_location_generate(num_empty);
            sudoku_generate(num_empty);
            flag = false;
            single_flag = true;
            single_check(0);
        }
        // 遍历整个数组，输出
        for (auto & i : space)
        {
            fputc(i[0] + '0', output_solve);
            for (int j = 1; j < 9; j++)
            {
                fputc(' ', output_solve);
                fputc(i[j] + '0', output_solve);
            }
            fputc('\n', output_solve);
        }
        fputc('\n', output_solve);
        cnt++;
    }

}


int main(int argc, char* argv[])
{
    int option_first;
    int option_second;
    cout <<"argc:"<<argc<<endl;
    if (argc <= 1 || argc >= 6){
        cout<<"argc out of range"<<endl;
        exit(1);
    }

    option_first = getopt(argc, argv, "c:s:n:");
    if ((option_first!='c')&&(option_first!='s')&&(option_first!='n')){
        printf("The input parameter is invalid or the input mode is incorrect!\n");
        exit(1);
    }
    else{
        printf("%c\n", option_first);
    }

    int sudoku_num;
    sudoku_num = to_integer(optarg);
    cout<<"sudoku_num = "<<sudoku_num<<endl;

    switch (option_first) {
        case 'c':{
            if (sudoku_num > 0 && sudoku_num <= 1000000)
                sudoku_endgame_create(sudoku_num);
            else printf("Please input a NUMBER from 1 to 1000000!\n");
            break;
        }
        case'n':{
            if(argc!=3){
                option_second = getopt(argc, argv, "m:r:u");

                if ((option_second!='m')&&(option_second!='r')&&(option_second!='u')){
                    printf("The input parameter is invalid or the input mode is incorrect!\n");
                    exit(1);
                }
                else{
                    printf("%c\n", option_second);
                }
                switch(option_second){
                    case'm':{
                        int difficulty = to_integer(optarg);
                        cout<<" difficulty = "<< difficulty<<endl;
                        if ((difficulty!=1)&&(difficulty!=2)&&(difficulty!=3))
                        {
                            printf("Invalid difficulty level!\n");
                            exit(1);
                        }
                        create_sudoku_with_difficulty(sudoku_num, difficulty);
                        break;
                    }
                    case'r':{
                        string str=argv[--argc];   //格式化输出

                        int num_empty_min = atoi(str.substr(0,2).c_str());
                        int num_empty_max = atoi(str.substr(3,2).c_str());
                        cout<<"num_empty_min = "<<num_empty_min<<endl;
                        cout<<"num_empty_max = "<<num_empty_max<<endl;
                        if (num_empty_min<20||num_empty_max>55)
                        {
                            printf("The amount of hollowing is invalid!\n");
                            exit(1);
                        }
                        sudoku_endgame_create(sudoku_num);


                        input = fopen("sudoku_endgame.txt", "r");
                        output_solve = fopen("sudoku_game.txt", "w");
                        if (!input)
                        {
                            printf("CANNOT find the input file!\n");
                            exit(1);
                        }
                        if (!output_solve)
                        {
                            printf("CANNOT open the output file!\n");
                            exit(1);
                        }
                        while (fscanf(input, "%d", &space[0][0]) != EOF)
                        {
                            for (int j = 1; j < 9; j++)
                                fscanf(input, "%d", &space[0][j]);
                            for (int i = 1; i < 9; i++)
                                for (int j = 0; j < 9; j++)
                                    fscanf(input, "%d", &space[i][j]);
                            flag = false;
                            int num_empty = (int)(num_empty_min + (rand()+1)%((num_empty_max+1) - num_empty_min));
                            random_empty_num_location_generate(num_empty);
                            sudoku_game_creat(num_empty_min);
                        }
                        fclose(input);
                        fclose(output_solve);
                        break;
                    }
                    case'u':{
                        create_sudoku_with_single_solution(sudoku_num);
                        break;
                    }
                }
            }
            else {
                sudoku_endgame_create(sudoku_num);

                input = fopen("sudoku_endgame.txt", "r");
                output_solve = fopen("sudoku_game.txt", "w");

                while (fscanf(input, "%d", &space[0][0]) != EOF)
                {
                    for (int j = 1; j < 9; j++)
                        fscanf(input, "%d", &space[0][j]);
                    for (int i = 1; i < 9; i++)
                        for (int j = 0; j < 9; j++)
                            fscanf(input, "%d", &space[i][j]);
                    flag = false;
                    random_empty_num_location_generate(30);
                    sudoku_game_creat(30);
                }
            }
            break;
        }
        case's':{
            input = fopen(optarg, "r");
            output_solve = fopen("sudoku_game_solve.txt", "w");
            if (!input)
            {
                printf("CANNOT find the input file!\n");
                exit(1);
            }
            if (!output_solve)
            {
                printf("CANNOT open the output file!\n");
                exit(1);
            }
            while (fscanf(input, "%d", &space[0][0]) != EOF)
            {
                for (int j = 1; j < 9; j++)
                    fscanf(input, "%d", &space[0][j]);
                for (int i = 1; i < 9; i++)
                    for (int j = 0; j < 9; j++)
                        fscanf(input, "%d", &space[i][j]);
                flag = false;
                sudoku_game_solve(0);
            }
            fclose(input);
            fclose(output_solve);
            break;
        }

    }
    return 0;
}






