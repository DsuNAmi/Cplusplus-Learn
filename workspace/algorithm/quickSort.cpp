#include <iostream>
#include <vector>


using namespace std;


void QuickSort(vector<int> & nums,int l, int r)
{
    if(l >= r) return;
    int key = nums[l];
    int left = l;
    int righ = r;
    while(left < righ)
    {
        while (nums[righ] >= key && left < righ)
        {
            --righ;
        }
        nums[l] = nums[r];
        while(nums[left] <= key && left < righ)
        {
            ++left;
        }
        nums[r] = nums[l];
    }
    nums[l] = key;
    QuickSort(nums,l,left - 1);
    QuickSort(nums,left + 1,r);
}


int main()
{

    return 0;
}