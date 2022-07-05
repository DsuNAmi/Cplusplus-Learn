#include <bits/stdc++.h>

using namespace std;


void InsertSort(vector<int> & arr)
{
    int n = arr.size();
    for(int i = 1; i < n; ++i)
    {
        int j = i - 1;
        int temp = arr[i];
        for(; j >= 0; --j)
        {
            if(arr[j] > temp) arr[j + 1] = arr[j];
            else break; //默认是已经排好序
        }
        arr[j + 1] = temp; //将待排序的对象放入指定的位置
    }
}

void BubbleSort(vector<int> & arr)
{
    int n = arr.size();
    for(int i = 0; i < n; ++i)
    {
        for(int j = 1; j < n; ++j)
        {
            if(arr[j] < arr[j - 1])
            {
                swap(arr[j],arr[j - 1]); //如果逆序，就交换相应的元素
            }
        }
    }
}

void ModifyBubbleSort(vector<int> & arr)
{
    int n = arr.size();
    bool flag = true; //判断有序无序的标记
    for(int i = 0; i < n; ++i)
    {
        flag = true;
        for(int j = 1; j < n; ++j)
        {
            if(arr[j] < arr[j - 1])
            {
                flag = false; //排序后，表示后面的序列都是不是有序的，仍然需要排序
                swap(arr[j],arr[j - 1]);
            }
        }
        if(flag) break; //序列有序，退出排序过程
    }
}


int main()
{
    vector<int> arr = {5,3,45,1,2};
    // vector<int> arr = {3,6,8,1,4};
    InsertSort(arr);
    // BubbleSort(arr);
    // ModifyBubbleSort(arr);
    for(auto x : arr) cout << x << " ";
    return 0;
}

