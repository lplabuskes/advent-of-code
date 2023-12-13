#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>


int string_diff(const std::string& s1, const std::string& s2)
{
    int diffs = 0;
    for (int i = 0; i < s1.length(); i++)
    {
        if (s1[i] != s2[i])
        {
            diffs++;
        }
    }
    return diffs;
}

int determine_mirror(const std::vector<std::string>& terrain, const std::vector<int>& horizontal_candidates, const std::vector<uint8_t>& vertical_candidates)
{
    for (int i : horizontal_candidates)
    {
        int offset = 0, total_diff = 0;
        while ((i - offset - 1) >= 0 && (i + offset) < terrain.size())
        {
            total_diff += string_diff(terrain[i - offset - 1], terrain[i + offset]);
            if (total_diff >= 2)
            {
                break;
            }
            offset++;
        }
        if (total_diff == 1)
        {
            return 100 * i;
        }
    }
    
    for (int i = 0; i < vertical_candidates.size(); i++)
    {
        int total_diff = vertical_candidates[i];
        for (std::string s : terrain)
        {
            int offset = 1;
            while ((i - offset) >= 0 && (i + offset + 1) < s.length())
            {
                if (s[i - offset] != s[i + offset + 1])
                {
                    total_diff++;
                }
                if (total_diff >= 2)
                {
                    break;
                }
                offset++;
            }
            if (total_diff >= 2)
            {
                break;
            }
        }
        if (total_diff == 1)
        {
            return i + 1;
        }
    }
    return 0;
}

int solution()
{
    std::ifstream file("input.txt");
    int total = 0;

    std::vector<std::string> current_terrain;
    std::vector<int> possible_horizontal_mirrors;
    std::vector<uint8_t> possible_veritcal_mirrors;
    int row = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            total += determine_mirror(current_terrain, possible_horizontal_mirrors, possible_veritcal_mirrors);
            current_terrain.clear();
            possible_horizontal_mirrors.clear();
            possible_veritcal_mirrors.clear();
            row = 0;
            continue;
        }
        
        if (current_terrain.size() == 0)
        {
            possible_veritcal_mirrors.resize(line.length()-1, 0);
            current_terrain.push_back(line);
        }
        else
        {
            if (string_diff(line, current_terrain.back()) <= 1)
            {
                possible_horizontal_mirrors.push_back(row);
            }
            current_terrain.push_back(line);
        }
        for (int i = 0; i < line.length() - 1; i++)
        {
            if (line[i] != line[i+1])
            {
                possible_veritcal_mirrors[i]++;
            }
        }

        row++;
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