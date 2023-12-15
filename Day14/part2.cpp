#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>


void roll(std::vector<int>& round_stones, const std::vector<int>& square_stones)
{
    int square_idx = 0;
    for (int round_idx = 0; round_idx < round_stones.size(); round_idx++)
    {
        int original_pos = round_stones[round_idx];
        bool hit_square = false;
        while (square_idx < square_stones.size() && square_stones[square_idx] < original_pos)
        {
            hit_square = true;
            round_stones[round_idx] = square_stones[square_idx] + 1;
            square_idx++;
        }
        if (hit_square)
        {
            continue;
        }
        if (round_idx == 0)
        {
            round_stones[round_idx] = 0;
        }
        else
        {
            round_stones[round_idx] = round_stones[round_idx - 1] + 1;
        }
    }
}

void roll(std::vector<std::vector<int>>& round_stones, const std::vector<std::vector<int>>& square_stones)
{
    for (int i = 0; i < round_stones.size(); i++)
    {
        roll(round_stones[i], square_stones[i]);
    }
}

struct Platform
{
    Platform(std::vector<std::vector<int>> rolling, std::vector<std::vector<int>> square_north, int n_rows): rolling(rolling), square_north(square_north), n_rows(n_rows)
    {
        n_cols = rolling.size();

        square_west.resize(n_rows, std::vector<int>());
        square_south.resize(n_cols, std::vector<int>());
        square_east.resize(n_rows, std::vector<int>());

        for (int i = 0; i < n_cols; i++)
        {
            const auto& col = square_north[i];
            for (int j = 0; j < col.size(); j++)
            {
                square_west[col[j]].push_back(i);
                square_south[i].push_back(n_cols - 1 - col[col.size() - 1 - j]);
            }
        }
        for (int i = 0; i < n_rows; i++)
        {
            const auto& row = square_west[i];
            for (int j = row.size() - 1; j >= 0; j--)
            {
                square_east[i].push_back(n_rows - 1 - row[j]);
            }
        }
    }
    int n_rows;
    int n_cols;
    std::vector<std::vector<int>> rolling;
    std::vector<std::vector<int>> square_north;
    std::vector<std::vector<int>> square_west;
    std::vector<std::vector<int>> square_south;
    std::vector<std::vector<int>> square_east;

    bool operator==(const Platform& p) const
    {
        if (rolling.size() != p.rolling.size())
        {
            return false;
        }
        for (int i = 0; i < rolling.size(); i++)
        {
            const auto& v1 = rolling[i], v2 = p.rolling[i];
            if (v1.size() != v2.size())
            {
                return false;
            }
            for (int j = 0; j < v1.size(); j++)
            {
                if (v1[j] != v2[j])
                {
                    return false;
                }
            }
        }
        return true;
    }
    struct HashFunction
    {
        std::size_t operator()(const Platform& p) const
        {
            std::size_t h = 0;
            for (const auto& v : p.rolling)
            {
                std::size_t subhash = 0;
                for (int i : v)
                {
                    subhash ^= (std::hash<int>{}(i) + subhash<<6 + subhash>>2);
                }
                h ^= subhash + h<<5 + h>>3;
            }
            return h;
        }
    };
    void cycle()
    {
        roll(rolling, square_north);

        std::vector<std::vector<int>> horizontal_roller;
        horizontal_roller.resize(n_rows, std::vector<int>());
        for (int i = 0; i < n_cols; i++)
        {
            const auto& col = rolling[i];
            for (auto j : col)
            {
                horizontal_roller[j].push_back(i);
            }
        }
        roll(horizontal_roller, square_west);

        for (auto& v : rolling)
        {
            v.clear();
        }
        for (int i = n_rows - 1; i >= 0; i--)
        {
            const auto& row = horizontal_roller[i];
            for (auto j : row)
            {
                rolling[j].push_back(n_rows - i - 1);
            }
        }
        roll(rolling, square_south);

        for (auto& v : horizontal_roller)
        {
            v.clear();
        }
        for (int i = n_cols - 1; i >= 0; i--)
        {
            const auto& col = rolling[i];
            for (auto j : col)
            {
                horizontal_roller[n_rows - j - 1].push_back(n_cols - i - 1);
            }
        }
        roll(horizontal_roller, square_east);

        for (auto& v : rolling)
        {
            v.clear();
        }
        for (int i = 0; i < n_rows; i++)
        {
            const auto& row = horizontal_roller[i];
            for (auto j : row)
            {
                rolling[n_cols - j - 1].push_back(i);
            }
        }
    }
    int weight()
    {
        int total_weight = 0;
        for (const auto& col : rolling)
        {
            for (auto pos : col)
            {
                total_weight += (n_rows - pos);
            }
        }
        return total_weight;
    }
};

int weight_cycle_many(Platform& p, int n_iter)
{
    std::unordered_map<Platform, int, Platform::HashFunction> previous_seen;
    std::unordered_map<int, int> previous_weights;

    int iterations = 0;
    auto it = previous_seen.find(p);
    while (it == previous_seen.end() && iterations < n_iter)
    {
        previous_seen.insert({p, iterations});
        previous_weights.insert({iterations, p.weight()});
        p.cycle();
        iterations++;
        it = previous_seen.find(p);
    }

    int cycle_length = iterations - it->second;
    int end_position = (n_iter - it->second) % cycle_length;
    return previous_weights.at(end_position + it->second);
}

int solution()
{
    std::ifstream file("input.txt");

    std::vector<std::vector<int>> round_stones;
    std::vector<std::vector<int>> square_stones;

    int row_idx = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if(line.length() == 0)
        {
            continue;
        }

        if (row_idx == 0)
        {
            round_stones.resize(line.length(), std::vector<int>());
            square_stones.resize(line.length(), std::vector<int>());
        }

        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == 'O')
            {
                round_stones[i].push_back(row_idx);
            }
            else if (line[i] == '#')
            {
                square_stones[i].push_back(row_idx);
            }
        }
        row_idx++;
    }

    Platform p(round_stones, square_stones, row_idx);

    return weight_cycle_many(p, 1'000'000'000);
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