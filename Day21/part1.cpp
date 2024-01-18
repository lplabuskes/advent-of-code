#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <string>
#include <chrono>


int solution()
{
    std::ifstream file("input.txt");
    uint16_t max_steps = 64;
    std::vector<std::vector<uint8_t>> is_obstacle;
    std::vector<std::vector<uint8_t>> explored;
    std::vector<std::vector<uint16_t>> min_distance;
    int start_row = 0, start_col = 0;

    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        min_distance.push_back(std::vector<uint16_t>(line.length(), -1));
        explored.push_back(std::vector<uint8_t>(line.length(), 0));

        std::vector<uint8_t> row_obstacles(line.length(), 0);
        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == '#')
            {
                row_obstacles[i] = 1;
            }
            else if (line[i] == 'S')
            {
                start_row = is_obstacle.size();
                start_col = i;
            }
        }
        is_obstacle.push_back(row_obstacles);
    }

    std::queue<std::tuple<int, int>> to_explore;
    min_distance[start_row][start_col] = 0;
    to_explore.push({start_row, start_col});
    explored[start_row][start_col] = 1;
    while(!to_explore.empty())
    {
        auto tmp = to_explore.front();
        int row = std::get<0>(tmp), col = std::get<1>(tmp);
        to_explore.pop();

        uint16_t node_dist = min_distance[row][col];
        if (node_dist >= max_steps)
        {
            break;
        }

        if (row-1 >= 0 && !explored[row-1][col] && !is_obstacle[row-1][col])
        {
            min_distance[row-1][col] = node_dist+1;
            to_explore.push({row-1, col});
            explored[row-1][col] = 1;
        }
        if (col-1 >= 0 && !explored[row][col-1] && !is_obstacle[row][col-1])
        {
            min_distance[row][col-1] = node_dist+1;
            to_explore.push({row, col-1});
            explored[row][col-1] = 1;
        }
        if (row+1 < is_obstacle.size() && !explored[row+1][col] && !is_obstacle[row+1][col])
        {
            min_distance[row+1][col] = node_dist+1;
            to_explore.push({row+1, col});
            explored[row+1][col] = 1;
        }
        if (col+1 < is_obstacle[0].size() && !explored[row][col+1] && !is_obstacle[row][col+1])
        {
            min_distance[row][col+1] = node_dist+1;
            to_explore.push({row, col+1});
            explored[row][col+1] = 1;
        }
    }

    int count = 0;
    for (const auto& row : min_distance)
    {
        for (const auto dist : row)
        {
            if (dist%2 == max_steps%2)
            {
                count++;
            }
        }
    }
    
    return count;
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