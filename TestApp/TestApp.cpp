// TestApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <list>

using namespace std;


template <class _RanIt>
void countImbalance(const _RanIt& sorted_rank, int& imbalance)
{
    int prev = INT_MAX;
    for (auto k : sorted_rank)
    {
        if (k - prev > 1)
            imbalance++;
        prev = k;
    }
}

//group sliding algorithm
int findTotalImbalance_AsPerTask(const vector<int>& input)
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
            countImbalance(sorted_rank, imbalance);
        }
    }
    return imbalance;
}

//increment group sizing algorithm
int findTotalImbalance_QuickSort(const vector<int>& input)
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
            countImbalance(sorted_rank, imbalance);
        }
    }
    return imbalance;
}

//Sort while inserting
int findTotalImbalance_InsertSort(const vector<int>& input)
{
    if (input.size() < 2)
    {
        return 0;
    }

    int imbalance = 0;
    for (int i = 0; i < input.size()-1; i++)
    {
        list<int> sorted_rank;
        int position = 0;
        sorted_rank.push_back(input[i]);

        for (int j = i+1; j < input.size(); j++)
        {
            position = 0;
            auto front = sorted_rank.front();
            auto back = sorted_rank.back();
            if (input[j] > back) // add to end
            {
                sorted_rank.push_back(input[j]);
                countImbalance(sorted_rank, imbalance);
            }
            else if (input[j] <= front)// add to begin
            {
                sorted_rank.push_front(input[j]);
                countImbalance(sorted_rank, imbalance);
            }
            else // insert somewhere in the middle
            {
                auto it = sorted_rank.begin();
                int div = sorted_rank.size()/2;
                bool right = true;
                while(div>=1) 
                { 
                    int prev = it._Ptr->_Myval;
                    int next = (++it)._Ptr->_Myval;
                    if (next < input[j])
                    {
                        for (int k = 0; k < div; k++)
                            it = it++;
                        div = std::ceil((double)div / 2);
                    }
                    else if (prev > input[j])
                    {
                        for (int k = 0; k < div; k++)
                            it = it--;
                        div = std::ceil((double)div / 2);
                    }
                    else
                    {
                        sorted_rank.insert(it, input[j]);
                        countImbalance(sorted_rank, imbalance);
                        break;
                    }
                }
            }
        }
    }
    return imbalance;
}

int findTotalImbalanceParallel(const vector<int>& input, short threads)
{
    if (input.size() < 2)
    {
        return 0;
    }

    int imbalance = 0;//atomic datatype
    std::vector<std::thread> workers;
    int* imbalances = new int[threads];
    for (int k = 0; k < threads; k++)
    {
        imbalances[k] = 0;
    }
    for (int k = 1; k <= threads; k++)
    {
        int* im = &imbalances[k - 1];
        //lampda thread does not seem to execute atomic operations for value incrementation safely
        //using separate memory for each thread for calculating imbalance
        //Not going to go into std::promise and std::future or std::atomic here
        //locks and mutexes used for more complex data types then int
        workers.push_back(std::thread([&input, k, threads, im]()
        {
            bool invert = false;
            int iter = 0;
            //This for loop update rule implements load balancing for each thread
            for (int i = k; i <= input.size(); i = ((invert == false) ? (iter * threads + k) : ((iter + 1) * threads + 1 - k)))
            {
                invert != invert;
                iter++;
                list<int> sorted_rank;
                int position = 0;
                sorted_rank.push_back(input[i-1]);

                for (int j = i; j < input.size(); j++)
                {
                    position = 0;
                    auto front = sorted_rank.front();
                    auto back = sorted_rank.back();
                    if (input[j] > back) // add to end
                    {
                        sorted_rank.push_back(input[j]);
                        countImbalance(sorted_rank, *im);
                    }
                    else if (input[j] <= front)// add to begin
                    {
                        sorted_rank.push_front(input[j]);
                        countImbalance(sorted_rank, *im);
                    }
                    else // insert somewhere in the middle
                    {
                        auto it = sorted_rank.begin();
                        int div = sorted_rank.size() / 2;
                        bool right = true;
                        while (div >= 1)
                        {
                            int prev = it._Ptr->_Myval;
                            int next = (++it)._Ptr->_Myval;
                            if (next < input[j])
                            {
                                for (int k = 0; k < div; k++)
                                    it = it++;
                                div = std::ceil((double)div / 2);
                            }
                            else if (prev > input[j])
                            {
                                for (int k = 0; k < div; k++)
                                    it = it--;
                                div = std::ceil((double)div / 2);
                            }
                            else
                            {
                                sorted_rank.insert(it, input[j]);
                                countImbalance(sorted_rank, *im);
                                break;
                            }
                        }
                    }
                }
            }
        }));
    }
    std::for_each(workers.begin(), workers.end(), [](std::thread& t)
    {
        t.join();
    });
    for (int k = 0; k < threads; k++)
    {
        imbalance+=imbalances[k];
    }
    delete[]imbalances;
    return imbalance;
}

int main()
{
    vector<int> groupInit{ 4,1,3,2,5,7,9,6,8,10 };
    vector<int> group;
    int decade = 1;
    std::cout << "Input number of sudent decades" << endl;
    cin >> decade;
    for (int i = 1; i <= decade; i++)//Unique ranks
    {
        for(auto j: groupInit)
            group.push_back(j*i);
    }

    auto start = std::chrono::system_clock::now();
    int imbalance = findTotalImbalance_AsPerTask(group);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "findTotalImbalance_AsPerTask elapsed_seconds: " << elapsed_seconds.count() << std::endl;
    std::cout << "findTotalImbalance_AsPerTask imbalance: " << imbalance <<endl;

    start = std::chrono::system_clock::now();
    imbalance = findTotalImbalance_QuickSort(group);
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "findTotalImbalance_QuickSort elapsed_seconds: " << elapsed_seconds.count() << std::endl;
    std::cout << "findTotalImbalance_QuickSort imbalance: " << imbalance << endl;

    start = std::chrono::system_clock::now();
    imbalance = findTotalImbalance_InsertSort(group);
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "findTotalImbalance_InsertSort elapsed_seconds: " << elapsed_seconds.count() << std::endl;
    std::cout << "findTotalImbalance_InsertSort imbalance: " << imbalance << endl;

    start = std::chrono::system_clock::now();
    imbalance = findTotalImbalanceParallel(group, 4);
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << "findTotalImbalanceParallel elapsed_seconds: " << elapsed_seconds.count() << std::endl;
    std::cout << "findTotalImbalanceParallel imbalance: " << imbalance << endl;
    cin >> decade;
}
