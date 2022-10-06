// TestApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <list>
#include <math.h>
using namespace std;

//group sliding algorithm
int findTotalImbalance(const vector<int>& input)
{
    if (input.size() < 2)
    {
        return 0;
    }

    int imbalance = 0;
    for (int i = 2; i <= input.size(); i++) //group size (can be made concurrent)
    {
        vector<int> sorted_rank;
        for (int j = 0; j < input.size() - i+1; j++) //Let's move through vector forming groups
        {
            sorted_rank.clear();
            for (int k = 0; k < i; k++) //fill the group
            {
                sorted_rank.push_back(input[j + k]);
            }
            sort(sorted_rank.begin(), sorted_rank.end());// q-sort
            for (int k = 1; k < i; k++)
            {
                if (sorted_rank[k] - sorted_rank[k - 1] > 1)//check group for imbalance
                    imbalance++;
            }
        }
    }
    return imbalance;
}

//increment group sizing algorithm
int findTotalImbalance2(const vector<int>& input, int threadNum)
{
    if (input.size() < 2)
    {
        return 0;
    }

    int imbalance = 0;

    for (int i = 0; i < input.size()-1; i++)
    {
        vector<int> sorted_rank;
        for (int j = i; j < input.size(); j++)
        {
            sorted_rank.push_back(input[j]);
            sort(sorted_rank.begin(), sorted_rank.end());
            int prev = INT_MAX;
            for (int k : sorted_rank)
            {
                if (k - prev > 1)
                    imbalance++;
                prev = k;
            }
        }
    }
    return imbalance;
}


//increment group sizing algorithm parallel 
int findTotalImbalanceParallel(const vector<int>& input)
{
    //TODO for large group sizes if needed
    return 0;
}

int main()
{
    vector<int> groupInit{ 4,1,3,2,5,7,9,6,8, 10 };
    vector<int> group;
    for (int i = 1; i <= 1000; i++)//Unique ranks
    {
        for(auto j: groupInit)
            group.push_back(j*i);
    }

    auto start = std::chrono::system_clock::now();
    int imbalance = findTotalImbalance(group);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "findTotalImbalance elapsed_seconds: " << elapsed_seconds.count() << std::endl;
    std::cout << "findTotalImbalance imbalance: " << imbalance <<endl;

    start = std::chrono::system_clock::now();
    imbalance = findTotalImbalance2(group);
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "findTotalImbalance2 elapsed_seconds: " << elapsed_seconds.count() << std::endl;
    std::cout << "findTotalImbalance2 imbalance: " << imbalance << endl;

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
