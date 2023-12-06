#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
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

void add_symbol(int row, int col, std::unordered_set<Point>& adjacent_spaces)
{
    // Don't really care about bounds because we're just producing a set to check against
    adjacent_spaces.insert(Point(row - 1, col - 1));
    adjacent_spaces.insert(Point(row - 1, col));
    adjacent_spaces.insert(Point(row - 1, col + 1));
    adjacent_spaces.insert(Point(row, col - 1));
    adjacent_spaces.insert(Point(row, col + 1));
    adjacent_spaces.insert(Point(row + 1, col - 1));
    adjacent_spaces.insert(Point(row + 1, col));
    adjacent_spaces.insert(Point(row + 1, col + 1));
}

int solution()
{
    std::ifstream file("input.txt");
    std::vector<Number> numbers;
    std::unordered_set<Point> adjacent_spaces;

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
            else  // i.e. this is a symbol
            {
                if (reading_number)
                {
                    numbers.push_back(current_number);
                    current_number = Number();
                    reading_number = false;
                }
                add_symbol(row, col, adjacent_spaces);
            }
        }
        // Gotten to the end of the row. If we were reading a number, we need to add that
        if (reading_number)
        {
            numbers.push_back(current_number);
        }
        row++;
    }

    int sum = 0;
    for (Number n : numbers)
    {
        for (Point p : n.coordinates)
        {
            if (adjacent_spaces.find(p) != adjacent_spaces.end())
            {
                sum += n.val;
                break;
            }
        }
    }
    return sum;
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