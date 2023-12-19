#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
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

struct Range
{
    Range(std::array<int, 4> min, std::array<int, 4> max, int destination): min(min), max(max), destination(destination){};
    long n_units() const
    {
        long output = 1;
        for (int i = 0; i < 4; i++)
        {
            output *= (max[i]-min[i]+1);
        }
        return output;
    }
    std::array<int, 4> min;
    std::array<int, 4> max;
    int destination;
};

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
                case '<': use_less = true; break;
                case '>': use_less = false; break;
            }

            threshold = std::stoi(tmp[0].substr(2));
        }
    }
    std::vector<Range> operator()(const Range& input_range)
    {
        std::vector<Range> retval;
        if (index > 3)
        {
            retval.push_back(Range(input_range.min, input_range.max, output));
        }
        else if ((use_less && input_range.max[index] < threshold) || (!use_less && input_range.min[index] > threshold))
        {
            retval.push_back(Range(input_range.min, input_range.max, output));
        }
        else if ((use_less && input_range.min[index] >= threshold) || (!use_less && input_range.max[index] <= threshold))
        {
            retval.push_back(Range(input_range.min, input_range.max, -3));
        }
        else
        {
            Range r1(input_range.min, input_range.max, output), r2(input_range.min, input_range.max, -3);
            if (use_less)
            {
                r1.max[index] = threshold - 1;
                r2.min[index] = threshold;
            }
            else
            {
                r1.min[index] = threshold + 1;
                r2.max[index] = threshold;
            }
            retval.push_back(r1);
            retval.push_back(r2);
        }
        return retval;
    }
    uint8_t index;  // x=0, m=1, a=2, s=3
    bool use_less;
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
    std::vector<Range> operator()(const Range& input_range)
    {
        std::vector<Range> retval;
        Range working_range = input_range;
        for (Rule rule : rules)
        {
            std::vector<Range> result = rule(working_range);
            for (Range& r : result)
            {
                if (r.destination != -3)
                {
                    retval.push_back(r);
                }
                else
                {
                    working_range = r;
                }
            }
        }
        return retval;
    }
    std::vector<Rule> rules;
};

unsigned long solution()
{
    std::ifstream file("input.txt");
    std::unordered_map<int, Workflow> system;

    const int start = name_to_id("in");
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
    }
    
    Range full(std::array<int, 4>({1, 1, 1, 1}), std::array<int, 4>({4000, 4000, 4000, 4000}), -3);
    std::vector<Range> ranges = system[start](full);
    unsigned long total_pass = 0;

    while (ranges.size() > 0)
    {
        Range current_range = ranges.back();
        ranges.pop_back();
        std::vector<Range> new_ranges = system[current_range.destination](current_range);
        for (const Range& r : new_ranges)
        {
            if (r.destination == -1)
            {
                total_pass += r.n_units();
            }
            else if (r.destination >= 0)
            {
                ranges.push_back(r);
            }
        }
    }
    
    return total_pass;
}

int main()
{
    using namespace std::chrono;
    unsigned long output;
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