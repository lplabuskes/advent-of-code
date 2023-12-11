#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>
#include <chrono>


int solution()
{
    const int grid_rows = 140, grid_cols = 140;

    std::ifstream file("input.txt");
    std::vector<std::pair<int, int>> galaxies;
    int cum_empty_rows[grid_rows] = {0}, cum_empty_cols[grid_cols] = {0};
    bool occupied_cols[grid_cols] = {false};

    int row = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        int galaxy_position = line.find('#');
        if (galaxy_position == line.npos)
        {
            for (int r = row; r < grid_rows; r++)
            {
                cum_empty_rows[r]++;
            }
        }
        else
        {
            while(galaxy_position < line.npos)
            {
                galaxies.push_back(std::pair<int, int>(row, galaxy_position));
                occupied_cols[galaxy_position] = true;
                galaxy_position = line.find('#', galaxy_position + 1);
            }
        }
        row++;
    }

    for (int i = 0; i < grid_cols; i++)
    {
        if (occupied_cols[i])
        {
            continue;
        }
        for (int j = i; j < grid_cols; j++)
        {
            cum_empty_cols[j]++;
        }
    }

    int total_distance = 0;
    for (int i = 0; i < galaxies.size(); i++)
    {
        int i_row = galaxies[i].first, i_col = galaxies[i].second;
        for (int j = i+1; j < galaxies.size(); j++)
        {
            int j_row = galaxies[j].first, j_col = galaxies[j].second;
            int normal_dist = std::abs(j_row-i_row) + std::abs(j_col - i_col);
            int expansion = std::abs(cum_empty_rows[j_row]-cum_empty_rows[i_row]) + std::abs(cum_empty_cols[j_col] - cum_empty_cols[i_col]);
            total_distance += normal_dist + expansion;
        }
    }

    return total_distance;
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