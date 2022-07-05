#include <bits/stdc++.h>

using namespace std;

int n = 3;
int W = 7;
vector<int> w = {3,4,2};
vector<int> v = {3,5,3};

int main()
{
    vector<vector<int>> dp(n + 1, vector<int>(W + 1));
    for(int i = 1; i <= n; ++i)
    {
        for(int j = 0; j <= W; ++j)
        {
            if(j < w[i])
            {
                dp[i][j] = dp[i - 1][j];
            }
            else
            {
                dp[i][j] = max(dp[i - 1][j], dp[i][j - w[i - 1]] + v[i - 1]);
            }
        }
    }
    return 0;
}