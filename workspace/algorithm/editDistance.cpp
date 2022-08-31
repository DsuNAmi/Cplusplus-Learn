#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

int EditDistance(string word1, string word2)
{
    int n = word1.size();
    int m = word2.size();

    if(n * m == 0) return n + m;

    vector<vector<int>> D(n + 1, vector<int>(m + 1));

    for(int i = 0; i <= n; ++i)
    {
        D[i][0] = i;
    }
    for(int i = 0; i <= m; ++i)
    {
        D[0][i] = i;
    }

    for(int i = 1; i <= n; ++i)
    {
        for(int j = 1; j <= m; ++j)
        {
            if(word1[i - 1] == word2[j - 1])
            {
                D[i][j] = D[i - 1][j - 1];
            }
            else
            {
                D[i][j] = min(D[i - 1][j - 1], min(D[i - 1][j], D[i][j - 1])) + 1;
            }
        }
    }

    return D[n][m];
    
}



int main()
{

    return 0;
}