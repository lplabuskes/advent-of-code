#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <string>
#include <chrono>


typedef unsigned long long ull;

std::vector<uint16_t> floodfill(const std::vector<std::vector<uint8_t>>& is_obstacle, int start_row, int start_col)
{
    std::vector<std::vector<uint16_t>> min_distance;
    for (int i = 0; i < is_obstacle.size(); i++)
    {
        min_distance.push_back(std::vector<uint16_t>(is_obstacle[0].size(), -1));
    }
    
    std::queue<std::tuple<int, int>> to_explore;
    min_distance[start_row][start_col] = 0;
    to_explore.push({start_row, start_col});

    while(!to_explore.empty())
    {
        auto tmp = to_explore.front();
        int row = std::get<0>(tmp), col = std::get<1>(tmp);
        to_explore.pop();

        uint16_t node_dist = min_distance[row][col];

        if (row-1 >= 0 && min_distance[row-1][col]==65535 && !is_obstacle[row-1][col])
        {
            min_distance[row-1][col] = node_dist+1;
            to_explore.push({row-1, col});
        }
        if (col-1 >= 0 && min_distance[row][col-1]==65535 && !is_obstacle[row][col-1])
        {
            min_distance[row][col-1] = node_dist+1;
            to_explore.push({row, col-1});
        }
        if (row+1 < is_obstacle.size() && min_distance[row+1][col]==65535 && !is_obstacle[row+1][col])
        {
            min_distance[row+1][col] = node_dist+1;
            to_explore.push({row+1, col});
        }
        if (col+1 < is_obstacle[0].size() && min_distance[row][col+1]==65535 && !is_obstacle[row][col+1])
        {
            min_distance[row][col+1] = node_dist+1;
            to_explore.push({row, col+1});
        }
    }

    std::vector<uint16_t> dist_count;
    for (const auto& row : min_distance)
    {
        for (const auto dist : row)
        {
            if (dist == 65535)
            {
                continue;
            }
            if (dist >= dist_count.size())
            {
                dist_count.resize(dist+1, 0);
            }
            dist_count[dist]++;
        }
    }

    for (int i = 2; i < dist_count.size(); i++)
    {
        dist_count[i] += dist_count[i-2];
    }
    return dist_count;
}

ull solution()
{
    std::ifstream file("input.txt");
    std::vector<std::vector<uint8_t>> is_obstacle;
    int max_steps = 26501365;
    int start_row = 0, start_col = 0;

    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }

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

    int row_max = is_obstacle.size()-1, col_max = is_obstacle[0].size()-1;

    auto center_counts = floodfill(is_obstacle, start_row, start_col);

    auto left_counts = floodfill(is_obstacle, start_row, 0);
    auto right_counts = floodfill(is_obstacle, start_row, col_max);
    auto top_counts = floodfill(is_obstacle, 0, start_col);
    auto bottom_counts = floodfill(is_obstacle, row_max, start_col);

    auto bl_counts = floodfill(is_obstacle, row_max, 0);
    auto br_counts = floodfill(is_obstacle, row_max, col_max);
    auto tl_counts = floodfill(is_obstacle, 0, 0);
    auto tr_counts = floodfill(is_obstacle, 0, col_max);

    // S is in the center of the map, there are 65 spaces to the edge in each direction all free
    // The border of the map is also entirely clear
    // This means you can reach anywhere on the edge of a "cell" with the theoretical minimum number of steps
    // Let the original map be cell <0,0> centered at point (0,0)
    // Cell <i,j> is centered at point (131i, 131j) spans (131i +/- 65, 131j +/- 65)
    // The minimum distance in x to cell <i,?> is 0 if i=0 else 131|i|-65. Similar for for y
    // Fully reachable cells have a "parity", either count spaces reachable in 129 or 130 depending on parity
    ull odd_cells = 0, even_cells = 0;
    ull edge_cell_spaces = 0;

    long cell_distance;
    // left half plane
    for (long long i = (-(max_steps+65)/131); i < 0; i++)
    {
        cell_distance = -131*i - 65;
        ull max_y = (max_steps-cell_distance+65)/131;
        if (max_y == 0)
        {
            edge_cell_spaces += right_counts[max_steps-cell_distance];
        }
        else if (max_y == 1)
        {
            edge_cell_spaces += right_counts[max_steps-cell_distance];
            cell_distance += 66;
            edge_cell_spaces += br_counts[max_steps-cell_distance];
            edge_cell_spaces += tr_counts[max_steps-cell_distance];
        }
        else
        {
            cell_distance += 131*max_y - 65;
            edge_cell_spaces += br_counts[max_steps-cell_distance];
            edge_cell_spaces += tr_counts[max_steps-cell_distance];
            cell_distance -= 131;
            edge_cell_spaces += br_counts[max_steps-cell_distance];
            edge_cell_spaces += tr_counts[max_steps-cell_distance];

            if ((-i)%2 == 0)
            {
                even_cells += max_y - 1 - max_y%2;
                odd_cells += max_y - 2 + max_y%2;
            }
            else
            {
                even_cells += max_y - 2 + max_y%2;
                odd_cells += max_y - 1 - max_y%2;
            }
        }
    }

    // i = 0
    cell_distance = 131*((max_steps+65)/131)-65; // using integer division here
    edge_cell_spaces += bottom_counts[max_steps-cell_distance];
    even_cells += ((max_steps+65)/131) - 1 + (((max_steps+65)/131)%2);
    odd_cells += ((max_steps+65)/131) - (((max_steps+65)/131)%2);
    edge_cell_spaces += top_counts[max_steps-cell_distance];

    // right half plane
    for (long long i = 1; i <= (max_steps+65)/131; i++)
    {
        cell_distance = 131*i - 65;
        ull max_y = (max_steps-cell_distance+65)/131;
        if (max_y == 0)
        {
            edge_cell_spaces += left_counts[max_steps-cell_distance];
        }
        else if (max_y == 1)
        {
            edge_cell_spaces += left_counts[max_steps-cell_distance];
            cell_distance += 66;
            edge_cell_spaces += bl_counts[max_steps-cell_distance];
            edge_cell_spaces += tl_counts[max_steps-cell_distance];
        }
        else
        {
            cell_distance += 131*max_y - 65;
            edge_cell_spaces += bl_counts[max_steps-cell_distance];
            edge_cell_spaces += tl_counts[max_steps-cell_distance];
            cell_distance -= 131;
            edge_cell_spaces += bl_counts[max_steps-cell_distance];
            edge_cell_spaces += tl_counts[max_steps-cell_distance];

            if (i%2 == 0)
            {
                even_cells += max_y - 1 - max_y%2;
                odd_cells += max_y - 2 + max_y%2;
            }
            else
            {
                even_cells += max_y - 2 + max_y%2;
                odd_cells += max_y - 1 - max_y%2;
            }
        }
    }

    ull total_spaces = edge_cell_spaces;
    if (center_counts.size()%2 == 0)
    {
        total_spaces += odd_cells * center_counts[center_counts.size()-2];
        total_spaces += even_cells * center_counts[center_counts.size()-1];
    }
    else
    {
        total_spaces += even_cells * center_counts[center_counts.size()-2];
        total_spaces += odd_cells * center_counts[center_counts.size()-1];
    }

    return total_spaces;
}

int main()
{
    using namespace std::chrono;
    ull output;
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