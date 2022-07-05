#include <bits/stdc++.h>

using namespace std;


/* 求一个数需要多少位 */
int Length(int val)
{
    int k = 1;
    val /= 2;
    while(val > 0)
    {
        ++k;
        val /= 2;
    }
    return k;
}

/* 图像变位压缩算法 */
void Compress(vector<int> & P, int n, vector<int> & S, vector<int> & l, vector<int> & b)
{
    int lMax = 256; //规定的每个段的像素最多为256个像素
    int header = 3 + 8; //根据上述的规定每个头部的长度为11位
    S[0] = 0; // 第0个分段的最优化长度为0
    for(int i = 1; i <= n; ++i)
    {
        b[i] = Length(P[i - 1]);//求该像素需要多少位表示
        int bMax = b[i];
        S[i] = S[i - 1] + bMax;
        l[i] = 1;
        for(int j = 2; j <= i && j <= lMax; ++j) //最后一段从两个像素一组开始往上增加
        {
            bMax = max(bMax, b[i - j + 1]);
            if(S[i] > S[i - j] + j * bMax)
            {
                S[i] = S[i - j] + j * bMax;
                l[i] = j;
            }
        }
        S[i] += header;
    }
}


/* 计算B_i */
int myLog(int a, int b)
{
    if(a == b) return ceil(log2(a));//如果是0的话就按照0的边界条件处理
    if(a < b) return myLog(b,a);
    return ceil(log2(a - b));//计算差值的log()
}

/* 修改后的图像变位压缩算法 */
void ModifyCompress(vector<int> & P, int n, vector<int> & S, vector<int> & l)
{
    int lMax = 256;
    int header = 3 + 8;
    S[0] = 0;
    //使用优先队列的时间复杂度为O(log(n))
    for(int i = 1; i <= n; ++i)
    {
        int curB = Length(P[i - 1]); // 单个是log(0)的特殊处理方法
        priority_queue<int,vector<int>,greater<int>> minQ; //维护最小值的优先队列
        priority_queue<int,vector<int>,less<int>> maxQ; //维护最大值的优先队列
        //保存优先队列的值
        maxQ.push(P[i - 1]);
        minQ.push(P[i - 1]);

        S[i] = S[i - 1] + curB;
        l[i] = 1;
        for(int j = 2; j <= i && j <= lMax; ++j)
        {
            //需要求两个以上分组的最大值，通过优先队列
            //首先将像素添加进优先队列
            maxQ.push(P[j - 1]);
            minQ.push(P[j - 1]);

            int maxP = maxQ.top();
            int minP = minQ.top();
            int newB = myLog(maxQ.top(),minQ.top()); //使用最大最小值
            if(S[i] > S[i - j] + j * newB)
            {
                S[i] = S[i - j] + j * newB;
                l[i] = j; 
            }
        }
        S[i] = header; 
    }
}



int main()
{
    // vector<int> P = {10,12,15,255,1,2,1,1,2,2,1,1};//69
    vector<int> P = {10,12,15,255,1,2};//57
    int n = P.size();
    vector<int> S(n + 1,0);
    vector<int> l(n + 1,0);
    vector<int> b(n + 1,0);
    // Compress(P,n,S,l,b);
    // cout << S[n] << endl;
    ModifyCompress(P,n,S,l);
    cout << S[n] << endl;

    // for(int i = 0; i <= 20; ++i)
    // {
    //     cout << "i:" << i << " log:" << myLog(i) << endl;
    // }

    // priority_queue<int,vector<int>,greater<int>> Q;
    // Q.push(3);
    // Q.push(4);
    // cout << Q.top() << endl;
    // priority_queue<int,vector<int>,less<int>> Q1;
    // Q1.push(4);
    // Q1.push(3);
    // cout << Q1.top() << endl;

    return 0;
}

/* 
方法1: 1 = {10,12,15}; 2 = {255}; 3 = {1,2,1,1,2,2,1,1}
11 * 3 + 4 * 3 + 8 * 1 + 2 * 8 = 69
方法2：1 = {10,12,15,255,1,2,1,1,2,2,1,1} 
11 * 1 + 8 * 12 = 107
方法3：分成12组，每组一个数
11 * 12 + 4 * 3 + 8 * 1 + 1 * 5 + 2 * 3 = 163
*/