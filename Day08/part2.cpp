#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>

typedef unsigned long long ull;

inline int16_t node_to_num(char a, char b, char c)
{
    return (a-'A')*(26*26) + (b-'A')*(26) + (c-'A')*(1);
}

ull solution()
{
    int16_t navigation[2][17575];
    std::memset(navigation, -1, sizeof(navigation));
    std::ifstream file("input.txt");

    std::string directions_str;
    std::getline(file, directions_str);
    std::vector<uint8_t> directions(directions_str.length());
    std::transform(directions_str.begin(), directions_str.end(), directions.begin(), [](char c){return c=='L' ? 0 : 1;});
    file.ignore(1000, '\n');

    std::vector<int16_t> start_nodes;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        int16_t key = node_to_num(line[0], line[1], line[2]);
        int16_t left = node_to_num(line[7], line[8], line[9]);
        int16_t right = node_to_num(line[12], line[13], line[14]);
        navigation[0][key] = left;
        navigation[1][key] = right;
        if (key%26 == 0)  // i.e. ends in 'A'
        {
            start_nodes.push_back(key);
        }
    }

    // There is nothing in the problem statement that guarantees this
    // But each starting point has a constant cycle length that is a multiple of the number of directions
    // This greatly simplifies the computation

    std::vector<ull> cycle_lengths;
    for (int16_t start : start_nodes)
    {
        int steps = 0;
        int16_t node = start;
        while (node%26!=25)
        {
            node = navigation[directions[steps%directions.size()]][node];
            steps++;
        }
        cycle_lengths.push_back(steps/directions.size());
    }

    ull steps = 1;
    for (ull i : cycle_lengths)
    {
        steps = std::lcm(steps, i);
    }

    return steps*directions.size();
}

int main()
{
    using namespace std::chrono;
    ull output;
    int n_iter = 10;
    unsigned long total_us = 0;
    for (int i = 0; i < n_iter; i++)
    {
        auto start = high_resolution_clock::now();
        output = solution();
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        total_us += duration.count();
    }
    std::cout << output << std::endl;
    std::cout << total_us/n_iter << " us" << std::endl;
}