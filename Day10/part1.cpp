#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>


// Directions 0 = right, 1 = up, 2 = left, 3 = down

int solution()
{
    // Using -1 to represent undefined
    int pipe_types[125][4];
    std::memset(pipe_types, -1, sizeof(pipe_types));
    {
    pipe_types['|'][1] = 1;
    pipe_types['|'][3] = 3;
    pipe_types['-'][0] = 0;
    pipe_types['-'][2] = 2;
    pipe_types['L'][3] = 0;
    pipe_types['L'][2] = 1;
    pipe_types['J'][0] = 1;
    pipe_types['J'][3] = 2;
    pipe_types['7'][1] = 2;
    pipe_types['7'][0] = 3;
    pipe_types['F'][1] = 0;
    pipe_types['F'][2] = 3;
    }

    std::ifstream file("input.txt");
    std::vector<std::string> pipes;

    int row = 0, start_row = 0, col = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        auto s_pos = line.find('S');
        if (s_pos != line.npos)
        {
            start_row = row;
            col = s_pos;
        }
        pipes.push_back(line);

        row++;
    }
    
    row = start_row;
    int pipe_length = 1;
    int direction = 0;
    if (pipe_types[pipes[row][col+1]][0] != -1)
    {
        direction = 0;
        col++;
    }
    else if (pipe_types[pipes[row-1][col]][1] != -1)
    {
        direction = 1;
        row--;
    }
    else if (pipe_types[pipes[row][col-1]][2] != -1)
    {
        direction = 2;
        col--;
    }
    else if (pipe_types[pipes[row+1][col]][3] != -1)
    {
        direction = 3;
        row++;
    }

    while(pipes[row][col] != 'S')
    {
        direction = pipe_types[pipes[row][col]][direction];
        pipe_length++;

        switch (direction)
        {
            case 0: col++; break;
            case 1: row--; break;
            case 2: col--; break;
            case 3: row++; break;
        }
    }

    return pipe_length/2;
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