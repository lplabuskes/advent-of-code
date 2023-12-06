#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
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

int solution()
{
    std::ifstream file("input.txt");
    std::string time_input, distance_input;
    std::getline(file, time_input);
    std::getline(file, distance_input);

    std::string time_str = "";
    std::string distance_str = "";

    for (auto s : split(time_input, " "))
    {
        if (s.length() == 0 || s[0] < '0' || s[0] > '9')
        {
            continue;
        }
        time_str += s;
    }
    for (auto s : split(distance_input, " "))
    {
        if (s.length() == 0 || s[0] < '0' || s[0] > '9')
        {
            continue;
        }
        distance_str += s;
    }
    // Squaring things so just be careful on variable size
    long long time = std::stol(time_str);
    long long distance = std::stol(distance_str);

    double spread = std::sqrt(time*time - 4*distance)/2.0;
    double center = time/2.0;
    double lb = center - spread, ub = center + spread;

    return ((int) (ub-1e-5)) - ((int) (lb+1)) + 1; // The 1e-5 is a hacky way to round down and take the next lowest integer if exactly equal
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