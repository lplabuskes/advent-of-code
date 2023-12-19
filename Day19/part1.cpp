#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <functional>
#include <chrono>


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

// need some special values to define accept, reject
// accept = -1, reject = -2
int name_to_id(const std::string& name)
{
    if (name[0]=='A')
    {
        return -1;
    }
    else if (name[0]=='R')
    {
        return -2;
    }
    
    int id = 0;
    for (char c : name)
    {
        id *= 26;
        id += c-'a';
    }
    return id;
}

// n/a will be another special value, -3
struct Rule
{
    Rule(const std::string& definition)
    {
        // a rule with conditions is at least 5 characters, and no rule name is more than 3
        if (definition.length() <= 3)
        {
            index = -1;
            output = name_to_id(definition);
        }
        else
        {
            auto tmp = split(definition, ":");
            output = name_to_id(tmp[1]);

            switch (tmp[0][0])
            {
                case 'x': index = 0; break;
                case 'm': index = 1; break;
                case 'a': index = 2; break;
                case 's': index = 3; break;
            }

            switch (tmp[0][1])
            {
                case '<': comparator = std::less<int>(); break;
                case '>': comparator = std::greater<int>(); break;
            }

            threshold = std::stoi(tmp[0].substr(2));
        }
    }
    int operator()(const std::array<int, 4>& part)
    {
        if (index > 3)
        {
            return output;
        }
        else if (comparator(part[index], threshold))
        {
            return output;
        }
        else
        {
            return -3; // for n/a
        }
    }
    uint8_t index;  // x=0, m=1, a=2, s=3
    std::function<bool(int,int)> comparator;
    int threshold;
    int output;
};

struct Workflow
{
    Workflow(){};
    Workflow(const std::string& definition)
    {
        auto subdefs = split(definition, ",");
        for (const auto& def : subdefs)
        {
            rules.push_back(Rule(def));
        }
    }
    int operator()(const std::array<int, 4>& part)
    {
        for (Rule rule : rules)
        {
            int result = rule(part);
            if (result != -3)
            {
                return result;
            }
        }
        return -3;  // should never get here, but just in case
    }
    std::vector<Rule> rules;
};

int solution()
{
    std::ifstream file("input.txt");
    std::unordered_map<int, Workflow> system;

    const int start = name_to_id("in");
    int total_rating = 0;
    bool reading_workflows = true;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            reading_workflows = false;
            continue;
        }

        if (reading_workflows)
        {
            int rules_start = line.find("{");
            int id = name_to_id(line.substr(0, rules_start));
            Workflow workflow(line.substr(rules_start+1, line.length() - rules_start - 2));
            system.insert({id, workflow});
        }
        else
        {
            auto tmp = split(line.substr(1, line.length()-2), ",");
            std::array<int, 4> part;
            for (int i = 0; i < 4; i++)
            {
                part[i] = std::stoi(tmp[i].substr(2));
            }

            int rule_id = system[start](part);
            while (rule_id >= 0)
            {
                rule_id = system[rule_id](part);
            }
            if (rule_id == -1)
            {
                total_rating += part[0] + part[1] + part[2] + part[3];
            }
        }
    }
    
    return total_rating;
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