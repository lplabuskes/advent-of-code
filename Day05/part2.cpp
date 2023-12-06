#include <fstream>
#include <iostream>
#include <vector>
#include <deque>
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

struct ResultRange
{
    ResultRange(ull start, ull end): start(start), end(end){};
    ull start;
    ull end;
};

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
    
    bool overlaps(ResultRange input)
    {
        return source_start <= input.end && (source_start + length) >= input.start;
    }
};

struct AlmanacMap
{
    std::vector<AlmanacRange> ranges;

    std::deque<ResultRange> process(std::deque<ResultRange> unprocessed)
    {
        std::deque<ResultRange> processed;
        while (unprocessed.size() > 0)
        {
            ResultRange current_range = unprocessed[0];
            unprocessed.pop_front();

            bool overlapped = false;
            for (AlmanacRange rule : ranges)
            {
                if (!rule.overlaps(current_range))
                {
                    continue;
                }
                if (current_range.start < rule.source_start)
                {
                    unprocessed.push_back(ResultRange(current_range.start, rule.source_start-1));
                    current_range.start = rule.source_start;
                }
                ull rule_end = rule.source_start + rule.length;
                if (current_range.end > rule_end)
                {
                    unprocessed.push_back(ResultRange(rule_end + 1, current_range.end));
                    current_range.end = rule_end;
                }
                processed.push_back(ResultRange((current_range.start + rule.destination_start) - rule.source_start, (current_range.end + rule.destination_start) - rule.source_start));
                overlapped = true;
                break;
            }
            // handle unmapped ranges
            if (!overlapped)
            {
                processed.push_back(current_range);
            }
        }
        return processed;
    }
};

struct AlmanacProcessor
{
    std::vector<AlmanacMap> maps;
    
    std::deque<ResultRange> process(std::deque<ResultRange> input)
    {
        for (AlmanacMap map : maps)
        {
            input = map.process(input);
        }
        return input;
    }
};

ull solution()
{
    std::ifstream file("input.txt");

    std::string line;
    std::getline(file, line);
    line = line.substr(7);

    std::deque<ResultRange> seed_ranges;
    auto tmp = split(line, " ");
    for (int i = 0; i < tmp.size(); i+=2)
    {
        ull start = std::stoull(tmp[i]);
        ull length = std::stoull(tmp[i+1]);
        seed_ranges.push_back(ResultRange(start, start+length));
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
    std::deque<ResultRange> position_ranges = processor.process(seed_ranges);
    for (ResultRange range : position_ranges)
    {
        if (range.start < min_position)
        {
            min_position = range.start;
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