#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <array>
#include <string>
#include <memory>
#include <numeric>
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

inline int parse_key(const std::string& module_id)
{
    int key = 0;
    for (int i = 1; i < module_id.length(); i++)
    {
        key *= 26;
        key += module_id[i]-'a';
    }
    return key;
}

struct Module
{
    virtual int handle_signal(int source, int signal) = 0;
    virtual std::vector<int> get_destinations() = 0;
    virtual void register_input(int input){};
    virtual std::vector<int> get_inputs(){return std::vector<int>();};
};

struct FlipFlop: Module
{
    FlipFlop(const std::string& def_string)
    {
        auto destination_strs = split(def_string, ", ");
        for (const auto& des : destination_strs)
        {
            int key = 0;
            for (const auto c : des)
            {
                key *= 26;
                key += c-'a';
            }
            destinations.push_back(key);
        }
    }
    int handle_signal(int source, int signal)
    {
        if (signal == 1)
        {
            return -1;
        }
        state = !state;
        return state ? 1 : 0;
    }
    std::vector<int> get_destinations()
    {
        return destinations;
    }
    std::vector<int> destinations;
    bool state = false;
};

struct Conjunction: Module
{
    Conjunction(const std::string& def_string)
    {
        auto destination_strs = split(def_string, ", ");
        for (const auto& des : destination_strs)
        {
            int key = 0;
            for (const auto c : des)
            {
                key *= 26;
                key += c-'a';
            }
            destinations.push_back(key);
        }
    }
    int handle_signal(int source, int signal)
    {
        for (int i = 0; i < inputs.size(); i++)
        {
            if (inputs[i] == source)
            {
                memory[i] = signal;
                break;
            }
        }
        // if all high, send low --> if any low, send high
        for (const int i : memory)
        {
            if (i == 0)
            {
                return 1;
            }
        }
        return 0;
    }
    std::vector<int> get_destinations()
    {
        return destinations;
    }
    void register_input(int input)
    {
        inputs.push_back(input);
        memory.push_back(0);
    }
    std::vector<int> get_inputs()
    {
        return inputs;
    }
    std::vector<int> destinations;
    std::vector<int> inputs;
    std::vector<int> memory;
};

struct Broadcaster: Module
{
    Broadcaster(const std::string& def_string)
    {
        auto destination_strs = split(def_string, ", ");
        for (const auto& des : destination_strs)
        {
            int key = 0;
            for (const auto c : des)
            {
                key *= 26;
                key += c-'a';
            }
            destinations.push_back(key);
        }
    }
    int handle_signal(int source, int signal)
    {
        return signal;
    }
    std::vector<int> get_destinations()
    {
        return destinations;
    }
    std::vector<int> destinations;
    bool state = false;
};

void press_button(std::unordered_map<int, std::unique_ptr<Module>>& machine, int n, int monitored_destination, std::unordered_map<int, int>& cycle_counts)
{
    std::queue<std::array<int, 3>> signals({{-2, 0, -1}}); // a signal is given by source, content, destination
    while (!signals.empty())
    {
        auto current_signal = signals.front();
        signals.pop();

        if (current_signal[2] == monitored_destination && current_signal[1] == 1)  // a high signal sent to the "end signaller"
        {
            if (cycle_counts.find(current_signal[0]) == cycle_counts.end())
            {
                cycle_counts[current_signal[0]] = n;
            }
        }

        auto it = machine.find(current_signal[2]);
        if (it == machine.end())
        {
            continue;
        }

        int output = it->second->handle_signal(current_signal[0], current_signal[1]);
        if (output < 0)
        {
            continue;
        }
        for (int i : it->second->get_destinations())
        {
            signals.push({current_signal[2], output, i});
        }
    }
}

long solution()
{
    std::ifstream file("input.txt");
    std::unordered_map<int, std::unique_ptr<Module>> machine;
    std::unordered_set<int> conjuction_modules;

    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }

        auto tmp = split(line, " -> ");
        if (tmp[0][0] == '%')
        {
            machine[parse_key(tmp[0])] = std::make_unique<FlipFlop>(tmp[1]);
        }
        else if (tmp[0][0] == '&')
        {
            machine[parse_key(tmp[0])] = std::make_unique<Conjunction>(tmp[1]);
            conjuction_modules.insert(parse_key(tmp[0]));
        }
        else
        {
            machine[-1] = std::make_unique<Broadcaster>(tmp[1]);
        }
    }

    int end_node = parse_key(".rx");
    int end_signaller = -1;
    for (const auto& it : machine)
    {
        for (const int destination : it.second->get_destinations())
        {
            if (conjuction_modules.find(destination) != conjuction_modules.end())
            {
                machine[destination]->register_input(it.first);
            }
            if (destination == end_node)
            {
                end_signaller = it.first;
            }
        }
    }

    int num_end_signaller_inputs = (machine[end_signaller]->get_inputs()).size();
    std::unordered_map<int, int> cycle_counts;
    int i = 1;
    while (cycle_counts.size() < num_end_signaller_inputs)
    {
        press_button(machine, i, end_signaller, cycle_counts);
        i++;
    }

    long overall_cycle = 1;
    for (const auto& it : cycle_counts)
    {
        overall_cycle = std::lcm(overall_cycle, it.second);
    }
    
    return overall_cycle;
}

int main()
{
    using namespace std::chrono;
    long output;
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