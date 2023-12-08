#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <chrono>

typedef std::string String;
typedef std::pair<String, String> StringPair;

int solution()
{
    std::ifstream file("input.txt");

    String directions;
    std::getline(file, directions);
    file.ignore(1000, '\n');

    std::unordered_map<String, StringPair> nodes;
    while (file)
    {
        String line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        String key = line.substr(0, 3);
        String left = line.substr(7, 3);
        String right = line.substr(12, 3);
        nodes.insert({key, std::pair<String, String>(left, right)});
    }

    String node = "AAA";
    int steps = 0;
    while (node != "ZZZ")
    {
        bool go_left = directions[steps%directions.length()]=='L';
        steps++;
        auto options = nodes.at(node);
        node = go_left ? options.first : options.second;
    }

    return steps;
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