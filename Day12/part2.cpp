#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <chrono>


struct Record
{
    Record(std::string conditions, std::vector<int> groups): conditions(conditions), groups(groups){};
    bool valid()
    {
        int min_required = std::accumulate(groups.begin(), groups.end(), 0) + groups.size() - 1;
        if (min_required > 0 && conditions.length() < min_required)
        {
            return false;
        }
        int current_fail_count = 0, group_idx = 0;
        for (char c : conditions)
        {
            if (c == '#')
            {
                current_fail_count++;
                if (group_idx >= groups.size() || current_fail_count > groups[group_idx])
                {
                    return false;
                }
            }
            else if (c == '?')
            {
                return true;
            }
            else if (current_fail_count == 0) // These following cases are all .'s
            {
                continue;
            }
            else if (group_idx >= groups.size() || current_fail_count != groups[group_idx])
            {
                return false;
            }
            else
            {
                current_fail_count = 0;
                group_idx++;
            }
        }

        if (current_fail_count != 0)
        {
            return group_idx == groups.size() - 1 && current_fail_count == groups[group_idx];
        }
        else
        {
            return group_idx == groups.size();
        }
    }
    bool operator==(const Record& r) const
    {
        if (groups.size() != r.groups.size())
        {
            return false;
        }
        if (conditions != r.conditions)
        {
            return false;
        }
        for (int i = 0; i < groups.size(); i++)
        {
            if (groups[i] != r.groups[i])
            {
                return false;
            }
        }
        return true;
    }
    struct HashFunction
    {
        std::size_t operator()(const Record& r) const
        {
            std::size_t h1 = std::hash<std::string>{}(r.conditions);
            std::size_t h2 = 0;
            for (int i : r.groups)
            {
                h2 ^= (std::hash<int>{}(i) + h2<<7 + h2>>2);
            }
            return h1 ^ (h2 << 1);
        }
    };

    unsigned long long count_possibilities(std::unordered_map<Record, unsigned long long, Record::HashFunction>& cache)
    {        
        if (!valid())
        {
            return 0;
        }

        auto cache_loc = cache.find(*this);
        if (cache_loc != cache.end())
        {
            return cache_loc->second;
        }

        int fail_count = 0, start_from = 0, groups_skip = 0;
        for (int i = 0; i < conditions.length(); i++)
        {
            if (conditions[i] == '#')
            {
                fail_count++;
            }
            else if (conditions[i] == '.')
            {
                if (fail_count > 0)
                {
                    fail_count = 0;
                    groups_skip++;
                }
                start_from = i;
            }
            else if (conditions[i] == '?')
            {
                std::string s1 = conditions.substr(start_from), s2 = conditions.substr(start_from);
                s1[i-start_from] = '.';
                s2[i-start_from] = '#';
                std::vector<int> reduced_groups(groups.size()-groups_skip);
                std::copy(groups.begin()+groups_skip, groups.end(), reduced_groups.begin());
                unsigned long long result = Record(s1, reduced_groups).count_possibilities(cache) + Record(s2, reduced_groups).count_possibilities(cache);
                cache.insert({*this, result});
                return result;
            }
        }
        // made it to this point only if it's valid and there are no question marks
        cache.insert({*this, 1});
        return 1;
    }
    std::string conditions;
    std::vector<int> groups;
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


unsigned long long solution()
{
    std::ifstream file("input.txt");
    std::unordered_map<Record, unsigned long long, Record::HashFunction> cache;
    unsigned long long sum = 0;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }

        auto temp = split(line, " ");
        std::string record = temp[0];
        temp = split(temp[1], ",");
        std::vector<int> segments;
        for (std::string s : temp)
        {
            segments.push_back(std::stoi(s));
        }
        std::string unfolded_record = record;
        std::vector<int> unfolded_segments = segments;
        for (int i = 0; i < 4; i++)
        {
            unfolded_record += "?"+record;
            for (int seg : segments)
            {
                unfolded_segments.push_back(seg);
            }
        }
        unsigned long long result = Record(unfolded_record, unfolded_segments).count_possibilities(cache);
        sum += result;
    }
    return sum;
}

int main()
{
    using namespace std::chrono;
    unsigned long long output;
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