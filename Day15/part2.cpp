#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <chrono>


std::vector<std::string> split(const std::string& text, const std::string& delimeter)
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

inline int hash(const std::string& s)
{
    int out = 0;
    for (char c : s)
    {
        if (c == '-' || c == '=')
        {
            return out;
        }
        out += c;
        out *= 17;
        out %= 256;
    }
    return out;
}

inline int parse_label(const std::string& s)
{
    int label = 0;
    for (char c : s)
    {
        if (c == '-' || c == '=')
        {
            return label;
        }
        label *= 26;
        label += (c - 'a');
    }
    return label;
}

void handle_minus(std::vector<std::pair<int, int>>& box, int label)
{
    for (int i = 0; i < box.size(); i++)
    {
        if (box[i].first == label)
        {
            box.erase(box.begin() + i);
            return;
        }
    }
}

void handle_equal(std::vector<std::pair<int, int>>& box, std::pair<int, int> step)
{
    for (int i = 0; i < box.size(); i++)
    {
        if (box[i].first == step.first)
        {
            box[i].second = step.second;
            return;
        }
    }
    box.push_back(step);
}

unsigned long solution()
{
    std::ifstream file("input.txt");
    std::string sequence;
    std::getline(file, sequence);
    auto steps = split(sequence, ",");
    
    std::vector<std::vector<std::pair<int, int>>> boxes(256);
    for (const auto& s : steps)
    {
        auto& box = boxes[hash(s)];
        if (s.back() == '-')
        {
            handle_minus(box, parse_label(s));
        }
        else
        {
            handle_equal(box, std::pair<int, int>(parse_label(s), s.back()-'0'));
        }
    }

    unsigned long total_power = 0;

    for (int i = 0; i < boxes.size(); i++)
    {
        const auto& box = boxes[i];
        for (int j = 0; j < box.size(); j++)
        {
            total_power += (i+1)*(j+1)*box[j].second;
        }
    }

    return total_power;
}

int main()
{
    using namespace std::chrono;
    unsigned long output;
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