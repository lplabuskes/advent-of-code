#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>


struct Beam
{
    Beam(int row, int col, uint8_t direction): row(row), col(col), direction(direction){};
    int row;
    int col;
    uint8_t direction; // 0, 1, 2, 3 for right, up, left, down
};

void process_beam(Beam& beam, const std::vector<std::string>& contraption, std::vector<std::vector<uint8_t>>& energized, std::vector<Beam>& beam_stack)
{
    while ((energized[beam.row][beam.col] & (1<<beam.direction)) == 0)
    {
        energized[beam.row][beam.col] |= (1<<beam.direction);
        char c = contraption[beam.row][beam.col];

        // Handle any branching and direction changes
        if (c == '.')
        {
            // Do nothing, just to get the early short circuit
        }
        else if (c == '\\')
        {
            beam.direction ^= 0b11;
        }
        else if (c == '/')
        {
            beam.direction ^= 0b01;
        }
        else if (c == '|')
        {
            if (beam.direction % 2 == 0)
            {
                beam.direction = 1;
                beam_stack.push_back(Beam(beam.row, beam.col, 3));
            }
        }
        else if (c == '-')
        {
            if (beam.direction % 2 == 1)
            {
                beam.direction = 0;
                beam_stack.push_back(Beam(beam.row, beam.col, 2));
            }
        }

        // Update position
        switch (beam.direction)
        {
            case 0: beam.col++; break;
            case 1: beam.row--; break;
            case 2: beam.col--; break;
            case 3: beam.row++; break;
        }

        if (beam.row < 0 || beam.col < 0 || beam.row >= contraption.size() || beam.col >= contraption[0].size())
        {
            return;
        }
    }
}

int process_starting_beam(Beam beam, const std::vector<std::string>& contraption)
{
    std::vector<std::vector<uint8_t>> energized;
    int ncol = contraption[0].length();
    for (int i = 0; i < contraption.size(); i++)
    {
        energized.push_back(std::vector<uint8_t>(ncol, 0));
    }

    std::vector<Beam> beams = {beam};
    while (beams.size() > 0)
    {
        Beam b = beams.back();
        beams.pop_back();
        process_beam(b, contraption, energized, beams);
    }

    int num_energized = 0;
    for (const auto& row : energized)
    {
        for (auto i : row)
        {
            if(i != 0)
            {
                num_energized++;
            }
        }
    }
    return num_energized;
}

int solution()
{
    std::ifstream file("input.txt");
    std::vector<std::string> contraption;
    
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        contraption.push_back(line);
    }
    
    int max_energized = 0;
    for (int i = 0; i < contraption[0].length(); i++)
    {
        int e1 = process_starting_beam(Beam(i, 0, 0), contraption),
            e2 = process_starting_beam(Beam(0, i, 3), contraption),
            e3 = process_starting_beam(Beam(i, contraption[0].length()-1, 2), contraption),
            e4 = process_starting_beam(Beam(contraption.size()-1, i, 1), contraption);
        max_energized = std::max({max_energized, e1, e2, e3, e4});
    }
    return max_energized;
}

int main()
{
    using namespace std::chrono;
    int output;
    int n_iter = 1;
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