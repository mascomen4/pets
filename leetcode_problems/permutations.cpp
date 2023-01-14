//
// Created by pi on 12/31/22.
//

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace std;

class Solution {
    int find_not_used_element(const vector<int> & nums, const vector<int> & state, const bool * used){
        int res = -11;

        for (int i=0;i<nums.size();++i){
            bool is_used = false;
            for(int j=0;j<state.size();++j){
                if (nums[i] == state[j]) is_used=true;
            }
            if (!is_used && !used[i]) return nums[i];
        }
        return res;
    }

    void helper(vector<vector<int>> & res, int begin, int end, vector<int>& nums){
        if (nums.size() == res[begin].size()) return;
        int partition_length = (end-begin)/( nums.size()-res[begin].size() );
        int i = begin;
        bool used[nums.size()];
        for (int j=0;j<nums.size();++j){used[j]=false;}
        while(i < end){
            for (int c = 0; c < partition_length; ++c){
                int element_to_push = find_not_used_element(nums, res[i+c], used);
                res[i+c].push_back(element_to_push);
            }
            helper(res, i, i+partition_length, nums);
            used[i/partition_length] = true;
            i += partition_length;
        }
    }

public:
    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> res( static_cast<int>(tgamma(nums.size() + 1) ), vector<int>() );
        helper( res, 0, res.size(), nums );
        return res;
    }
};

int main(){
    vector<int> v1{1,2,3,4};
    Solution sol;
    auto res = sol.permute(v1);
    auto print = [](const vector<int>& var){for (int i : var){cout << i << " ";} cout<<endl;};
    for_each(res.cbegin(), res.cend(), print);
}

