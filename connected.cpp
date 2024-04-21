#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <fstream>
#include <easyx.h>

namespace Space
{
    bool l = false, r = false, up = false, down = false;
}
using namespace std;
#define step 25
const int M = 4100, N = 2100;
char g[M][N];
int X, Y, Down = 10, Right = 10;
int All_color[M][N], f[M][N]; // 待填充记录为0，边界记录为1，已填充区域记录为2

struct Point
{
    int x, y; // 表示点的x, y坐标
    struct Point *next;
    Point() { x = 0, y = 0, next = NULL; }
    Point(int m_x, int m_y)
    {
        x = m_x, y = m_y;
        next = NULL;
    }
};

class Stack
{ // 把栈的属性和方法放在Stack类中
public:
    Point *p; // 栈顶指针
    Stack()
    { // 构造函数，为栈顶指针开辟空间
        p = new Point();
        p->next = NULL;
    }
    bool Is_Empty();     // 判断栈是否为空
    void Push(Point *t); // 入栈
    void Pop();          // 出栈
    Point Top();         // 取出栈顶元素
};

bool Stack::Is_Empty()
{
    if (p->next == NULL)
        return false;
    return true;
}
void Stack::Push(Point *t)
{
    Point *tt = new Point(*t);
    tt->next = p;
    p = tt;
}
void Stack::Pop()
{
    if (!Is_Empty())
        return;
    Point *t = p;
    p = p->next;
    delete t;
}
Point Stack::Top()
{
    Point t = *p;
    return t;
}
bool Check(int x, int y)
{                                                        // 检查像素点的合法性
    bool l = false, r = false, up = false, down = false; // 初始化上、下、左、右都不合法
    if (x < 0 || y < 0 || x >= M || y >= N || g[x][y] == '1' || g[x][y] == '*')
        return false; // 超出区域或者在区域里
    // 检查是否超出左右上下边界或者在区域内部
    int xt = x, yt = y;
    while (xt >= 0)
    {
        if (g[xt][y] == '1')
        {
            up = true;
            break;
        }
        xt--;
    }
    xt = x;
    while (xt < M)
    {
        if (g[xt][y] == '1')
        {
            down = true;
        }
        xt++;
    }
    while (yt >= 0)
    {
        if (g[x][yt] == '1')
        {
            l = true;
            break;
        }
        yt--;
    }
    yt = y;
    while (yt < N)
    {
        if (g[x][yt] == '1')
        {
            r = true;
            break;
        }
        yt++;
    }
    return up && down && l && r;
}
void Init_row(int row, string s)
{
    Down = max(Down, row), Right = max(Right, s.size() - 1); // 找边界
    int x = 0;
    while (s[x] == ' ')
    {
        x++;
    }
    for (int i = x; i < s.size(); i++)
    {
        g[row][i] = s[i];
        if (s[i] == ' ')
        {
            All_color[row][i] = 0;
        }
        else if (s[i] == '1')
        {
            All_color[row][i] = 1;
        }
        else
            All_color[row][i] = 2;
    }
}

void Drawn()
{
    // 绘图并填充的过程, 填充后用蓝色表示
    initgraph(10000, 10000); // 创建画图窗口
    setbkcolor(WHITE);       // 设置背景颜色
    cleardevice();
    for (int i = 0; i <= Down; i++)
    {
        for (int j = 0; j <= Right; j++)
        {
            if (All_color[i][j] == 1)
            {
                setfillcolor(BLACK);
                solidrectangle(j * step, i * step, j * step + step, i * step + step);
            }
            else if (All_color[i][j] == 2)
            {
                setfillcolor(RED);
                solidrectangle(j * step, i * step, j * step + step, i * step + step);
            }
            else
            {
                setfillcolor(WHITE);
                solidrectangle(j * step, i * step, j * step + step, i * step + step);
            }
        }
    }
}

void Change(int x, int y)
{
    All_color[x][y] = 3;
    setfillcolor(BLUE);
    solidrectangle(y * step, x * step, y * step + step, x * step + step);
    Sleep(10);
}
void Solve(Stack *stk)
{            // 扫描线种子填充算法
    Drawn(); // 绘制可视化区域
    // cout << "依次出栈的种子点" << endl;
    while (stk->Is_Empty())
    {
        auto t = stk->Top();
        // cout << t.x << ' ' << t.y << endl;//输入种子点
        stk->Pop();
        int x = t.x, y = t.y;
        int left = t.y, right = t.y + 1;
        // 填充时间到
        // 向左填充
        while (left >= 0 && f[x][left] == 1)
        {
            Change(x, left); // 填充该像素点
            // All_color[x][left] = 3;
            left--;
        }
        // 向右填充
        while (f[x][right] == 1)
        {
            Change(x, right); // 填充该像素点
            right++;
        }
        // 向上扫描
        for (int i = left; x - 1 >= 0 && i <= right; i++)
        {
            if (f[x - 1][i] == 1)
            {
                int j = i;
                while (j <= right && f[x - 1][j] == 1)
                {
                    j++;
                }
                i = j;
                if (All_color[x - 1][j - 1] == 0 && f[x - 1][j - 1] == 1)
                { // 最右边可以涂色并且没有涂过色
                    Point *Next_Seed = new Point(x - 1, j - 1);
                    stk->Push(Next_Seed);
                }
            }
        }
        // 向下扫描
        for (int i = left; x + 1 < M && i <= right; i++)
        {
            if (f[x + 1][i] == 1)
            {
                int j = i;
                while (j <= right && f[x + 1][j] == 1)
                {
                    j++;
                }
                i = j;
                if (All_color[x + 1][j - 1] == 0 && f[x + 1][j - 1] == 1)
                { // 最右边可以涂色并且没有涂过色
                    Point *Next_Seed = new Point(x + 1, j - 1);
                    stk->Push(Next_Seed);
                }
            }
        }
    }
    char ch = getchar();
    ch = getchar(); // 卡住画图区域
}

bool Read()
{
    // 用数字1代表图形边界, 字符'*'表示区域;
    memset(g, ' ', sizeof g);
    memset(All_color, 0, sizeof All_color);
    memset(g, ' ', sizeof g);
    memset(f, 0, sizeof f);

    cout << "请输入你要打开的文件名称" << endl;
    string filename;
    cin >> filename;
    ifstream ifs(filename);
    if (!ifs.is_open())
    {
        cout << "文件打开失败, 请重新输入你的选择!\n"
             << endl;
        return false;
    }
    else
    {
        cout << "文件打开成功!\n"
             << endl;
        cout << "用字符'1'代表图形边界, 字符'*'表示区域!!!" << endl;
        string s;
        int cnt = 0;
        while (getline(ifs, s))
        {
            Init_row(cnt++, s);
        }
    }
    for (int i = 0; i <= Down; i++)
    {
        for (int j = 0; j <= Right; j++)
        {
            cout << g[i][j];
        }
        cout << endl;
    }
    // 预处理像素点的合法性
    for (int i = 0; i <= Down; i++)
    {
        for (int j = 0; j <= Right; j++)
        {
            if (Check(i, j))
                f[i][j] = 1;
        }
    }
    ifs.close();
    return true;
}
void Show_menu()
{
    cout << "**************************" << endl;
    cout << "******输入1 创建区域******" << endl;
    cout << "*******输入2 填充 ********" << endl;
    cout << "********输入3 清屏********" << endl;
    cout << "***退出请输入任意字符!!***" << endl;
    cout << "**************************" << endl;
}
void Fill()
{
    while (1)
    {
        cout << "请输入种子的横坐标" << endl;
        cin >> X;
        cout << "请输入种子的纵坐标" << endl;
        cin >> Y;
        if (!f[X][Y])
        {
            cout << "你输入的种子坐标超出或者在边界上或者在区域内，请重新输入!!!\n";
            continue;
        }
        else
            break;
    }
    Point *Seed = new Point(X, Y);
    Stack *stk = new Stack();
    stk->Push(Seed);
    Solve(stk);
    closegraph(); // 关闭画布
}
int main()
{
    Show_menu();
    int order;
    bool is_out = true, Ok = false; // Ok记录是否有合法区域
    while (1)
    {
        cout << "请输入你的选择:\n";
        cin >> order;
        switch (order)
        {
        case 1:
        {
            if (Read())
            {
                Ok = true;
            }
            else
            {
                cout << "你输入的文件名不正确或者你选择的区域不合法！\n";
                Ok = false;
            }
        }
        break;
        case 2:
        {
            if (Ok)
            {
                Fill();
            }
            else
            {
                cout << "你还没有选择合适的区域，无法填充, 请重新选择!\n";
            }
        }
        break;
        case 3:
        {
            system("cls");
            Show_menu();
        };
        break;
        default:
        {
            is_out = false;
            break;
        }
        }
        if (!is_out)
        {
            break;
        }
    }
    return 0;
}