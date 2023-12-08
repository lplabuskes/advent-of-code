#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <chrono>


inline int16_t node_to_num(char a, char b, char c)
{
    return (a-'A')*(26*26) + (b-'A')*(26) + (c-'A')*(1);
}

int solution()
{
    int16_t nodes[2][17575];
    std::memset(nodes, -1, sizeof(nodes));
    std::ifstream file("input.txt");

    std::string directions;
    std::getline(file, directions);
    file.ignore(1000, '\n');

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
        nodes[0][key] = left;
        nodes[1][key] = right;
    }

    int16_t node = 0;
    int steps = 0;
    while (node != 17575)
    {
        int direction = directions[steps%directions.length()]=='L' ? 0 : 1;
        steps++;
        node = nodes[direction][node];
    }

    return steps;
}

int main()
{
    using namespace std::chrono;
    int output;
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