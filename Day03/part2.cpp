#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>


struct Point
{
    Point(int x, int y): x(x), y(y){};
    bool operator==(const Point& p) const{return p.x==x && p.y==y;};
    int x;
    int y;
};

template<>
struct std::hash<Point>
{
    std::size_t operator()(const Point& p) const
    {
        return std::hash<int>()(p.x) ^ std::hash<int>()(p.y);
    }
};

struct Number
{
    int val = 0;
    std::vector<Point> coordinates;
};

struct Gear
{
    Gear(int x, int y): x(x), y(y){};
    int x;
    int y;
    std::vector<int> neighbors;
};

void add_symbol(std::shared_ptr<Gear> gear, std::unordered_map<Point, std::vector<std::shared_ptr<Gear>>>& adjacent_spaces)
{
    int row = gear->x, col = gear->y;
    Point points[] = {Point(row - 1, col - 1), Point(row - 1, col), Point(row - 1, col + 1),
                      Point(row, col - 1), Point(row, col + 1),
                      Point(row + 1, col - 1), Point(row + 1, col), Point(row + 1, col + 1)};
    for (Point p : points)
    {
        adjacent_spaces.insert({p, std::vector<std::shared_ptr<Gear>>()});
        adjacent_spaces[p].push_back(gear);
    }
}

int solution()
{
    std::ifstream file("input.txt");
    std::vector<Number> numbers;
    std::vector<std::shared_ptr<Gear>> gears;
    std::unordered_map<Point, std::vector<std::shared_ptr<Gear>>> adjacent_spaces;

    int row = 0;
    while(file)
    {
        std::string line;
        std::getline(file, line);

        bool reading_number = false;
        Number current_number;
        for (int col = 0; col < line.length(); col++)
        {
            if (line[col] == '.')
            {
                if (reading_number)
                {
                    numbers.push_back(current_number);
                    current_number = Number();
                    reading_number = false;
                }
                else
                {
                    continue;
                }
            }
            else if (line[col] >= '0' && line[col] <= '9')
            {
                reading_number = true;
                current_number.val *= 10;
                current_number.val += line[col] - '0';
                current_number.coordinates.push_back(Point(row, col));
            }
            else if (line[col] == '*')
            {
                if (reading_number)
                {
                    numbers.push_back(current_number);
                    current_number = Number();
                    reading_number = false;
                }
                auto g = std::make_shared<Gear>(row, col);
                gears.push_back(g);
                add_symbol(g, adjacent_spaces);
            }
        }
        // Gotten to the end of the row. If we were reading a number, we need to add that
        if (reading_number)
        {
            numbers.push_back(current_number);
        }
        row++;
    }

    for (Number n : numbers)
    {
        for (Point p : n.coordinates)
        {
            auto it = adjacent_spaces.find(p);
            if (it != adjacent_spaces.end())
            {
                for (auto g : it->second)
                {
                    g->neighbors.push_back(n.val);
                }
                break;
            }
        }
    }

    int total_ratio = 0;
    for (auto g : gears)
    {
        if (g->neighbors.size() != 2)
        {
            continue;
        }
        int ratio = g->neighbors[0] * g->neighbors[1];
        total_ratio += ratio;
    }

    return total_ratio;
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