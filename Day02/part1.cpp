#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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

bool valid_draw(std::string text)
{
    int counts[] = {0, 0, 0};
    auto entries = split(text, ", ");
    for (std::string entry : entries)
    {
        auto temp = split(entry, " ");
        int num = std::stoi(temp[0]);
        if (temp[1][0] == 'r')
        {
            counts[0] += num;
        }
        else if (temp[1][0] == 'g')
        {
            counts[1] += num;
        }
        else{
            counts[2] += num;
        }
    }
    return (counts[0] <= 12 && counts[1] <= 13 && counts[2] <= 14);
}

int process_game(std::string line)
{
    auto temp = split(line, ": ");
    int id = std::stoi(temp[0].substr(5));
    auto draws = split(temp[1], "; ");
    for (std::string draw : draws)
    {
        if (!valid_draw(draw))
        {
            return 0;
        }
    }
    return id;
}

int solution()
{
    std::ifstream file("input.txt");
    int sum = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line=="")
        {
            continue;
        }
        int res = process_game(line);
        sum += process_game(line);
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