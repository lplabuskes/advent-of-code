#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

int solution()
{
    std::ifstream file("input.txt");
    int sum = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);

        char first_digit = 0, last_digit = 0;
        for (char c : line)
        {
            if (c >= 48 && c <= 57)
            {
                last_digit = c;
                if (first_digit == 0)
                {
                    first_digit = c;
                }
            }
        }
        if (first_digit == 0 || last_digit == 0)
        {
            continue;
        } 
        int num = ((first_digit - 48) * 10) + (last_digit - 48);
        sum += num;
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