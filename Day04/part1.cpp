#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
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

int score_game(std::string game)
{
    std::string numbers = split(game, ": ")[1];
    std::vector<std::string> tmp = split(numbers, " | ");
    std::unordered_set<int> winning_numbers;

    for (auto s : split(tmp[0], " "))
    {
        if (s == "")
        {
            continue;
        }
        winning_numbers.insert(std::stoi(s));
    }

    int score = 0;
    for (auto s : split(tmp[1], " "))
    {
        if (s == "")
        {
            continue;
        }
        if (winning_numbers.find(std::stoi(s)) == winning_numbers.end())
        {
            continue;
        }
        else if (score == 0)
        {
            score += 1;
        }
        else
        {
            score *= 2;
        }
    }

    return score;
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
        int res = score_game(line);
        sum += res;
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