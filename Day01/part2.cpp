#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <array>
#include <chrono>

int solution()
{
    std::string digits[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    std::ifstream file("input.txt");
    int sum = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);

        int digit_first_occurence[10];
        std::transform(std::begin(digits), std::end(digits), std::begin(digit_first_occurence), 
                        [line](std::string s){return line.find(s);});
        
        int digit_last_occurence[10];
        std::transform(std::begin(digits), std::end(digits), std::begin(digit_last_occurence), 
                        [line](std::string s){return line.rfind(s);});

        for (int i = 0; i < line.length(); i++)
        {
            char c = line[i];
            if (c >= 48 && c <= 57)
            {
                int digit = c - 48;
                if (digit_first_occurence[digit] == -1 || i < digit_first_occurence[digit])
                {
                    digit_first_occurence[digit] = i;
                }
                if (digit_last_occurence[digit] == -1 || i > digit_last_occurence[digit])
                {
                    digit_last_occurence[digit] = i;
                }
            }
        }

        int first_digit_index = 9999, first_digit = 0, last_digit_index = -1, last_digit = 0;
        for (int i = 0; i < 10; i++)
        {
            if (digit_first_occurence[i] != -1 && digit_first_occurence[i] < first_digit_index)
            {
                first_digit_index = digit_first_occurence[i];
                first_digit = i;
            }
            if (digit_last_occurence[i] != -1 && digit_last_occurence[i] > last_digit_index)
            {
                last_digit_index = digit_last_occurence[i];
                last_digit = i;
            }
        }
        sum += (10 * first_digit) + last_digit;
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