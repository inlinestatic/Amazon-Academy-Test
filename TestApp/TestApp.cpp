// TestApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int CheckImbalanse(const vector<int>& input)
{
    if (input.size() < 2)
    {
        return 0;
    }

    int imbalance = 0;
    for (int i = 2; i <= input.size(); i++) //group size (can be made concurrent)
    {
        vector<int> group;
        for (int j = 0; j < input.size() - i+1; j++) //Let's move through vector forming groups
        {
            group.clear();
            for (int k = 0; k < i; k++) //fill the group
            {
                group.push_back(input[j + k]);
            }
            sort(group.begin(), group.end());// q-sort
            for (int k = 1; k < i; k++)
            {
                if (group[k] - group[k - 1] > 1)//check group for imbalance
                    imbalance++;
            }
        }
    }
    return imbalance;
}

int main()
{
    vector<int> arr{ 4,1,3,2 };
    int imbalance = CheckImbalanse(arr);
    std::cout << imbalance;
    getchar();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
