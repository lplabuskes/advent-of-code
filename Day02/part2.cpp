#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

struct CubeCount
{
    int red = 0;
    int green = 0;
    int blue = 0;

    int power(){return red * green * blue;};
};

std::vector<std::string> split(std::string text, std::string delimeter)
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

CubeCount count_draw(std::string text)
{
    CubeCount counts;
    auto entries = split(text, ", ");
    for (std::string entry : entries)
    {
        auto temp = split(entry, " ");
        int num = std::stoi(temp[0]);
        if (temp[1] == "red")
        {
            counts.red += num;
        }
        else if (temp[1] == "green")
        {
            counts.green += num;
        }
        else if (temp[1] == "blue"){
            counts.blue += num;
        }
    }
    return counts;
}

int process_game(std::string line)
{
    auto temp = split(line, ": ");
    auto draws = split(temp[1], "; ");
    CubeCount min_required;
    for (std::string draw : draws)
    {
        CubeCount draw_count = count_draw(draw);
        if (draw_count.red > min_required.red)
        {
            min_required.red = draw_count.red;
        }
        if (draw_count.green > min_required.green)
        {
            min_required.green = draw_count.green;
        }
        if (draw_count.blue > min_required.blue)
        {
            min_required.blue = draw_count.blue;
        }
    }
    return min_required.power();
}

int solution()
{
    std::ifstream file("input.txt");
    int sum = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line=="")
        {
            continue;
        }
        int res = process_game(line);
        sum += process_game(line);
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