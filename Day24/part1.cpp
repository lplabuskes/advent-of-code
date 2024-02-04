#include <fstream>
#include <iostream>
#include <vector>
#include <string>
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

struct Hailstone
{
    Hailstone(const std::string& input)
    {
        auto tmp = split(input, " @ ");

        auto position = split(tmp[0], ", ");
        auto velocity = split(tmp[1], ", ");

        x0 = std::stod(position[0]); y0 = std::stod(position[1]); z0 = std::stod(position[2]);
        vx = std::stod(velocity[0]); vy = std::stod(velocity[1]); vz = std::stod(velocity[2]);
    }
    double x0, y0, z0;
    double vx, vy, vz;
};

bool intersect(const Hailstone& a, const Hailstone& b, 
                const long long xmin, const long long xmax,
                const long long ymin, const long long ymax)
{
    // Set of linear equations to solve for intersection
    // Check that time of intersection is positive for both
    // Validate solution happens within bounds
    double determinant = (a.vx * b.vy) - (a.vy * b.vx);
    double t1 = (1/determinant) * (b.vy * (b.x0 - a.x0) - b.vx * (b.y0 - a.y0));
    double t2 = (1/determinant) * (a.vy * (b.x0 - a.x0) - a.vx * (b.y0 - a.y0));

    if (t1 < 0 || t2 < 0)
    {
        return false;
    }

    double x_soln = a.x0 + t1 * a.vx, y_soln = a.y0 + t1*a.vy;
    return x_soln > xmin && x_soln < xmax && y_soln > ymin && y_soln < ymax;
}

int solution()
{
    std::ifstream file("input.txt");
    long long min_val = 200000000000000, max_val = 400000000000000;
    std::vector<Hailstone> stones;

    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }

        stones.emplace_back(line);
    }

    int n_intersect = 0;

    for (int i = 0; i < stones.size(); i++)
    {
        for (int j = i+1; j < stones.size(); j++)
        {
            if (intersect(stones[i], stones[j], min_val, max_val, min_val, max_val))
            {
                n_intersect++;
            }
        }
    }

    return n_intersect;
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