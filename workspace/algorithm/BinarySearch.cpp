#include <bits/stdc++.h>

using namespace std;

int BinarySearch(vector<int> & nums, int target)
{
    int l = 0;
    int r = nums.size();
    while(l <= r)
    {
        int mid = l + ((r - l) >> 1);
        if(nums[mid] == target) return mid;
        else if(nums[mid] < target) l = mid + 1;
        else r = mid - 1; 
    }

    return -1;
}

int LeftBound(vector<int> & nums, int target)
{
    int l = 0;
    int r = nums.size();
    while(l < r)
    {
        int mid = l + ((r - l) >> 1);
        if(nums[mid] == target) r = mid;
        else if(nums[mid] < target) l = mid + 1;
        else r = mid;
    }

    return l;
}

int main()
{
    return 0;
}