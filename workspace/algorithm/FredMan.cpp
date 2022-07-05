#include <bits/stdc++.h>

using namespace std;

/* 构建第一层h(x) */
void Hash(vector<int> & S, int k, int N, int n, vector<int> & hx)
{
    for(auto x : S)
    {
        hx.push_back(((k * x) % N) % n);
    }
}

/* 计算是否冲突，同时计算b */
int HashValue(vector<int> & hx, vector<int> & b)
{
    unordered_map<int,int> h;
    for(auto x : hx) ++h[x];
    int sum = 0;
    for(auto & [k,v] : h) 
    {
        sum += v * v;
        b[k] = v;
    }
    return sum;
}

/* 计算C */
void GetC(vector<int> & b, vector<int> & c)
{
    for(int i = 0; i < (int)b.size(); ++i)
    {
        // c[i] = (b[i] - 1) * b[i] + 1;
        c[i] = b[i] * b[i];
    }
}

/* 得到每个值在对应块的下标 */
void GetS_i(vector<vector<int>> & subS, vector<int> & hx, vector<int> & S)
{
    for(int i = 0; i < (int)hx.size(); ++i)
    {
        subS[hx[i]].push_back(S[i]);
    }
}

/* 计算ki */
void NewS_i(vector<int> & S, vector<int> & NewS, int N, int n)
{
    vector<int> temp(S.size());
    int k = 1;
    while(1)
    {
        set<int> t;
        for(int i = 0; i < (int)S.size(); ++i)
        {
            temp[i] = ((k * S[i]) % N) % n;
            t.insert(((k * S[i]) % N) % n);
        }
        if(temp.size() == t.size()) break;
        ++k;
    }
    NewS = temp;
    cout << k << endl;
}

/* 计算每个块 */
void UpadteS_i(vector<vector<int>> & subS, vector<int> & c, int N, int n, vector<vector<int>> & finalS)
{
    for(int i = 0; i < n; ++i)
    {
        if(int(subS[i].size()))
        {
            cout << i << endl;
            NewS_i(subS[i],finalS[i],N,c[i]);
        }
    }
}

/* 实现Fredman哈希算法 */
void FredMan(vector<int> & S, int N, int n)
{
    vector<int> hx;
    int k = 1;
    int hv = 0;
    vector<int> b;
    do
    {
        ++k;
        hx.clear();
        b = vector<int>(n,0);
        Hash(S,k,N,n,hx);
        hv = HashValue(hx,b);

    } while (hv > 3 * n);
    vector<int> c(n);
    GetC(b,c);

    vector<vector<int>> subS(n);
    GetS_i(subS,hx,S);
    for(auto v : subS)
    {
        for(auto x : v)
        {
            cout << x << " ";
        }
        cout << endl;
    }
    // cout << "b:" << endl;
    // for(auto x : b) cout << x << " ";
    // cout << endl;
    // cout << "c:" << endl;
    // for(auto x : c) cout << x << " ";
    // cout << endl; 
    vector<vector<int>> finalS(n);
    UpadteS_i(subS,c,N,n,finalS);
    for(auto v : finalS)
    {
        for(auto x : v)
        {
            cout << x << " ";
        }
        cout << endl;
    }
    /* 输出被注释 */
}


int main()
{
    // vector<int> S = {2,3,4,11,12,17,21,30,35};
    // vector<int> S = {2,4,9,12,17,18,26,35};
    vector<int> S = {2,4,5,15,18,30};
    int N = 31;
    int n = S.size();
    FredMan(S,N,n);
    return 0;
}