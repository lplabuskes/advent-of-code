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

    int straight = 0, left = 0, right = 0;
    int quad_area = 0;
    while(pipes[row][col] != 'S')
    {
        // This is a Green's Theorem approach
        switch (direction)
        {
            case 0: quad_area-=row; break;
            case 1: quad_area-=col; break;
            case 2: quad_area+=row; break;
            case 3: quad_area+=col; break;
        }

        int new_direction = pipe_types[pipes[row][col]][direction];
        if (new_direction == direction)
        {
            straight++;
        }
        else if(new_direction == (direction+1)%4)
        {
            left++;
        }
        else
        {
            right++;
        }
        direction = new_direction;

        switch (direction)
        {
            case 0: quad_area-=row; col++; break;
            case 1: quad_area-=col; row--; break;
            case 2: quad_area+=row; col--; break;
            case 3: quad_area+=col; row++; break;
        }    
    }

    // Handle the area contribution from the start square
    switch (direction)
    {
        case 0: quad_area-=row; break;
        case 1: quad_area-=col; break;
        case 2: quad_area+=row; break;
        case 3: quad_area+=col; break;
    }
    
    // Know that either exactly 4 more left than right or 4 more right than left, use to figure out start square direction
    if (right == left + 3)
    {
        right++;
        direction = (direction+3)%4;
    }
    else if (left == right + 3)
    {
        left++;
        direction = (direction+1)%4;
    }
    else
    {
        straight++;
    }

    switch (direction)
    {
        case 0: quad_area-=row; break;
        case 1: quad_area-=col; break;
        case 2: quad_area+=row; break;
        case 3: quad_area+=col; break;
    }

    // Now to handle the error from the regions occupied by pipe
    // If it's negative, we went counter clockwise and need subtract more for right turns
    if (quad_area < 0)
    {
        quad_area *= -1;
        quad_area -= 2*right;
    }
    else
    {
        quad_area -= 2*left;
    }
    quad_area -= 2*straight + left + right;

    return quad_area/4;
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