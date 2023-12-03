#include <iostream>
#include <fstream>
#include <string>
#include <vector>


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

int main()
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
    std::cout << sum << std::endl;
}