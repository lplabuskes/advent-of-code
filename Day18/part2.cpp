#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>


std::vector<std::string> split(std::string text, std::string delimeter)
{
    std::vector<std::string> output;
    int index = 0;
    while (index < text.length())
    {
        int position = text.find(delimeter, index);
        if (position == -1)
        {
            break;
        }
        output.push_back(text.substr(index, position - index));
        index = position + delimeter.length();
    }
    output.push_back(text.substr(index));
    return output;
}

long solution()
{
    std::ifstream file("input.txt");

    long area = 0, turns = 0, straights = 0;
    long x = 0, y = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        std::string step = split(line, " ")[2];
        long step_distance = std::stoi(step.substr(2, 5), nullptr, 16);
        straights += step_distance - 1;
        turns++;
        
        if (step[7] == '0')
        {
            area -= y*step_distance;
            x += step_distance;
        }
        else if (step[7] == '3')
        {
            area += x*step_distance;
            y += step_distance;
        }
        else if (step[7] == '2')
        {
            area += y*step_distance;
            x -= step_distance;
        }
        else
        {
            area -= x*step_distance;
            y -= step_distance;
        }
    }
    
    area = (area > 0) ? area/2 : -area/2;
    area += straights/2;
    area += turns/2 + 1;
    return area;
}

int main()
{
    using namespace std::chrono;
    long output;
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