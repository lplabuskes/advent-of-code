#include <iostream>
#include <fstream>
#include <string>

int main()
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
    std::cout << sum << std::endl;
}