#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
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

int hash(const std::string& s)
{
    int out = 0;
    for (char c : s)
    {
        out += c;
        out *= 17;
        out %= 256;
    }
    return out;
}

unsigned long solution()
{
    std::ifstream file("input.txt");
    std::string sequence;
    std::getline(file, sequence);
    auto steps = split(sequence, ",");
    unsigned long out = 0;
    for (const auto& s : steps)
    {
        out += hash(s);
    }
    return out;
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