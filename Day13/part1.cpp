#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>


int determine_mirror(const std::vector<std::string>& terrain, const std::vector<int>& horizontal_candidates, const std::vector<uint8_t>& vertical_candidates)
{
    for (int i : horizontal_candidates)
    {
        bool valid = true;
        int offset = 1;
        while ((i - offset - 1) >= 0 && (i + offset) < terrain.size())
        {
            if (terrain[i - offset - 1] != terrain[i + offset])
            {
                valid = false;
                break;
            }
            offset++;
        }
        if (valid)
        {
            return 100 * i;
        }
    }
    
    for (int i = 0; i < vertical_candidates.size(); i++)
    {
        if (vertical_candidates[i] == 0)
        {
            continue;
        }
        bool valid = true;
        for (std::string s : terrain)
        {
            int offset = 1;
            while ((i - offset) >= 0 && (i + offset + 1) < s.length())
            {
                if (s[i - offset] != s[i + offset + 1])
                {
                    valid = false;
                    break;
                }
                offset++;
            }
            if (!valid)
            {
                break;
            }
        }
        if (valid)
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
            possible_veritcal_mirrors.resize(line.length()-1, 1);
            current_terrain.push_back(line);
        }
        else
        {
            if (line == current_terrain.back())
            {
                possible_horizontal_mirrors.push_back(row);
            }
            current_terrain.push_back(line);
        }
        for (int i = 0; i < line.length() - 1; i++)
        {
            if (line[i] != line[i+1])
            {
                possible_veritcal_mirrors[i] = 0;
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