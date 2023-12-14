#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>


int solution()
{
    std::ifstream file("input.txt");
    int total = 0;

    std::vector<std::string> platform;
    std::vector<uint8_t> rolling;
    std::vector<int> new_position;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if(line.length() == 0)
        {
            continue;
        }

        rolling.assign(line.length(), 0);
        new_position.assign(line.length(), platform.size());

        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == 'O')
            {
                rolling[i] = 1;
            }
        }

        for (int i = platform.size()-1; i >= 0; i--)
        {
            std::string platform_row = platform[i];
            bool any_rolling = false;
            for (int j = 0; j < line.length(); j++)
            {
                if (rolling[j] == 0)
                {
                    continue;
                }
                any_rolling = true;
                if (platform_row[j] == '.')
                {
                    new_position[j] = i;
                }
                else
                {
                    rolling[j] = 0;
                }
            }
            if (!any_rolling)
            {
                break;
            }
        }

        for (int i = 0; i < line.length(); i++)
        {
            if (new_position[i] != platform.size())
            {
                line[i] = '.';
                platform[new_position[i]][i] = 'O';
            }
        }

        platform.push_back(line);
    }

    // Handled the rolling behavior while parsing input
    for (int i = 0; i < platform.size(); i++)
    {
        int n_stones = 0;
        std::string row = platform[i];
        for (char c : row)
        {
            if (c == 'O')
            {
                n_stones++;
            }
        }
        total += (platform.size() - i)*n_stones;
    }
    
    return total;
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