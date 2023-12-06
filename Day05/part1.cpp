#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>


typedef unsigned long long ull;

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

struct AlmanacRange
{
    AlmanacRange(ull des, ull src, ull len): destination_start(des),
                                                     source_start(src),
                                                     length(len){};
    AlmanacRange(std::string s)
    {
        auto nums = split(s, " ");
        destination_start = std::stoull(nums[0]);
        source_start = std::stoull(nums[1]);
        length = std::stoull(nums[2]);
    }
    ull destination_start;
    ull source_start;
    ull length;
    
    bool contains(ull num)
    {
        return num >= source_start && num < (source_start + length);
    }

    ull process(ull num)
    {
        return (num - source_start) + destination_start;
    }
};

struct AlmanacMap
{
    // I know that sorting the ranges would make it faster to process a seed location
    // I'll make that optimization if it's actually necessary
    std::vector<AlmanacRange> ranges;

    ull process(ull num)
    {
        for (AlmanacRange range : ranges)
        {
            if (range.contains(num))
            {
                return range.process(num);
            }
        }
        // None of the ranges contain our input, so it is unmodified
        return num;
    }
};

struct AlmanacProcessor
{
    std::vector<AlmanacMap> maps;
    
    ull process(ull num)
    {
        for (AlmanacMap map : maps)
        {
            num = map.process(num);
        }
        return num;
    }
};

ull solution()
{
    std::ifstream file("input.txt");

    std::string line;
    std::getline(file, line);
    line = line.substr(7);

    std::vector<ull> seeds;
    for (std::string s : split(line, " "))
    {
        seeds.push_back(std::stoull(s));
    }

    AlmanacProcessor processor;
    AlmanacMap current_map;
    while (file)
    {
        std::getline(file, line);
        if (line.length() == 0)
        {
            processor.maps.push_back(current_map);
            current_map = AlmanacMap();
        }
        else if (line[0] >= '0' && line[1] <= '9')
        {
            current_map.ranges.push_back(AlmanacRange(line));
        }
    }
    processor.maps.push_back(current_map);

    ull min_position = 0-1;
    for (ull seed : seeds)
    {
        ull position = processor.process(seed);
        if (position < min_position)
        {
            min_position = position;
        }
    }
    return min_position;
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