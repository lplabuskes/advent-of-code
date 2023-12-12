#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
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

bool valid_record(std::string& record, std::vector<int>& segments)
{
    int current_fail_count = 0, segment_idx = 0;
    for (char c : record)
    {
        if (c == '#')
        {
            current_fail_count++;
            if (current_fail_count > segments[segment_idx])
            {
                return false;
            }
        }
        else if (c == '?')
        {
            return true;
        }
        else if (current_fail_count == 0) // These following cases are all .'s
        {
            continue;
        }
        else if (segment_idx >= segments.size() || current_fail_count != segments[segment_idx])
        {
            return false;
        }
        else
        {
            current_fail_count = 0;
            segment_idx++;
        }
    }

    if (current_fail_count != 0)
    {
        return segment_idx == segments.size() - 1 && current_fail_count == segments[segment_idx];
    }
    else
    {
        return segment_idx == segments.size();
    }
}

int count_possibilities(std::string record, std::vector<int>& segments)
{
    if (!valid_record(record, segments))
    {
        return 0;
    }
    for (int i = 0; i < record.length(); i++)
    {
        if (record[i] == '?')
        {
            std::string s1 = record, s2 = record;
            s1[i] = '.';
            s2[i] = '#';
            return count_possibilities(s1, segments) + count_possibilities(s2, segments);
        }
    }
    // made it to this point only if it's valid and there are no question marks
    return 1;
}

int solution()
{
    std::ifstream file("input.txt");
    
    int sum = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }

        auto temp = split(line, " ");
        std::string record = temp[0];
        temp = split(temp[1], ",");
        std::vector<int> segments;
        for (std::string s : temp)
        {
            segments.push_back(std::stoi(s));
        }
        sum += count_possibilities(record, segments);
    }
    return sum;
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