#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
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

int recursive_predictor(const std::vector<int>& input)
{
    if (std::adjacent_find(input.begin(), input.end(), std::not_equal_to<>()) == input.end())  // aka all the same
    {
        return input[0];
    }
    std::vector<int> diffs(input.size() - 1);
    for (int i = 0; i < diffs.size(); i++)
    {
        diffs[i] = input[i+1] - input[i];
    }
    return input[0] - recursive_predictor(diffs);
}

int solution()
{
    std::ifstream file("input.txt");
    int sum = 0;
    while(file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        auto number_strs = split(line, " ");
        std::vector<int> numbers(number_strs.size());
        std::transform(number_strs.begin(), number_strs.end(), numbers.begin(), [](std::string s){return std::stoi(s);});
        sum += recursive_predictor(numbers);
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