#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <chrono>


std::vector<std::string> split(const std::string& text, const std::string& delimeter)
{
    std::vector<std::string> output;
    int index = 0;
    while (index < text.length())
    {
        int position = text.find(delimeter, index);
        if (position == -1)
        {
            break;
        }
        output.push_back(text.substr(index, position - index));
        index = position + delimeter.length();
    }
    output.push_back(text.substr(index));
    return output;
}

struct Block
{
    Block(const std::string& input, int id): id(id)
    {
        auto tmp = split(input, "~");
        auto lhs = split(tmp[0], ",");
        auto rhs = split(tmp[1], ",");
        // For the problem input lhs <= rhs, so no need for comparison
        xmin = std::stoi(lhs[0]);
        ymin = std::stoi(lhs[1]);
        zmin = std::stoi(lhs[2]);
        xmax = std::stoi(rhs[0]);
        ymax = std::stoi(rhs[1]);
        zmax = std::stoi(rhs[2]);
    }
    bool operator<(const Block& b) const{return zmin < b.zmin;};
    static bool id_compare(const Block& a, const Block& b) {return a.id < b.id;};

    int xmin = 0, ymin = 0, zmin = 0;
    int xmax = 0, ymax = 0, zmax = 0;
    int id;
    std::vector<int> supporters;
    std::vector<int> supporting;
};

int solution()
{
    std::ifstream file("input.txt");
    std::vector<Block> blocks;
    std::map<int, std::vector<Block>> blocks_by_zmax;

    int row = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        blocks.push_back(Block(line, row));
        row++;
    }

    std::sort(blocks.begin(), blocks.end());

    for (auto b_it = blocks.begin(); b_it != blocks.end(); b_it++)
    {
        int candidate_z_support = 0;
        for (auto z_rit = blocks_by_zmax.rbegin(); z_rit != blocks_by_zmax.rend(); z_rit++)
        {
            if (z_rit->first < candidate_z_support)
            {
                break;
            }
            for (const auto& fallen_block : z_rit->second)
            {
                if (fallen_block.xmax < b_it->xmin || fallen_block.xmin > b_it->xmax ||
                    fallen_block.ymax < b_it->ymin || fallen_block.ymin > b_it->ymax)
                {
                    continue;
                }

                if (candidate_z_support == z_rit->first)
                {
                    b_it->supporters.push_back(fallen_block.id);
                }
                else
                {
                    candidate_z_support = z_rit->first;
                    b_it->supporters.clear();
                    b_it->supporters.push_back(fallen_block.id);
                }
            }
        }

        int z_shift = (b_it->zmin - candidate_z_support) - 1;
        b_it->zmin -= z_shift;
        b_it->zmax -= z_shift;

        if (blocks_by_zmax.find(b_it->zmax) == blocks_by_zmax.end())
        {
            blocks_by_zmax[b_it->zmax] = std::vector<Block>({*b_it});
        }
        else
        {
            blocks_by_zmax[b_it->zmax].push_back(*b_it);
        }
    }

    std::sort(blocks.begin(), blocks.end(), Block::id_compare);
    
    for (const auto& block : blocks)
    {
        for (const auto id : block.supporters)
        {
            blocks[id].supporting.push_back(block.id);
        }
    }

    int blocks_fall = 0;
    std::queue<int> to_explore;
    std::unordered_set<int> falling;
    for (const auto & block : blocks)
    {
        falling.insert(block.id);
        for (const auto i : block.supporting)
        {
            to_explore.push(i);
        }

        while (!to_explore.empty())
        {
            int id = to_explore.front();
            to_explore.pop();

            bool has_support = false;
            for (const auto supporter : blocks[id].supporters)
            {
                if (falling.find(supporter) == falling.end()) // aka supporter is not falling
                {
                    has_support = true;
                    break;
                }
            }
            if (!has_support)
            {
                falling.insert(id);
                for (const auto supported : blocks[id].supporting)
                {
                    to_explore.push(supported);
                }
            }
        }
        blocks_fall += falling.size() - 1;
        falling.clear();
    }

    return blocks_fall;
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