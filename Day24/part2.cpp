#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <chrono>


std::vector<std::string> split(const std::string& text, const std::string& delimeter)
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

struct Hailstone
{
    Hailstone(const std::string& input)
    {
        auto tmp = split(input, " @ ");
        auto position = split(tmp[0], ", ");
        auto velocity = split(tmp[1], ", ");

        x0 = std::stoll(position[0]); y0 = std::stoll(position[1]); z0 = std::stoll(position[2]);
        vx = std::stoi(velocity[0]); vy = std::stoi(velocity[1]); vz = std::stoi(velocity[2]);
    }
    long long x0, y0, z0;
    int vx, vy, vz;
};

std::unordered_map<long long, int> prime_factorize(long long n)
{
    std::unordered_map<long long, int> factors;

    while (n % 2 == 0)
    {
        if (factors.size() == 0)
        {
            factors[2] = 0;
        }
        factors[2]++;
        n /= 2;
    }

    long long i = 3;
    while (i < std::sqrt(n))
    {
        while (n % i == 0)
        {
            if (factors.find(i) == factors.end())
            {
                factors[i] = 0;
            }
            factors[i]++;
            n /= i;
        }
        i+=2;
    }
    if (n > 1)
    {
        factors[n] = 1;
    }
    
    return factors;
}

struct VelocityInfo
{
    VelocityInfo(int base_v, const std::vector<long long>& offsets): base_v(base_v)
    {
        std::vector<long long> diffs;
        for (int i = 0; i < offsets.size(); i++)
        {
            for (int j = i+1; j < offsets.size(); j++)
            {
                long long diff = offsets[j]-offsets[i];
                diffs.push_back(std::abs(diff));
            }
        }
        
        prime_factors = prime_factorize(diffs[0]);
        for (int i = 1; i < diffs.size(); i++)
        {
            auto new_factors = prime_factorize(diffs[i]);
            std::unordered_map<long long, int> merged_factors;
            for (const auto& f : prime_factors)
            {
                auto it = new_factors.find(f.first);
                if (it == new_factors.end())
                {
                    continue;
                }
                merged_factors[f.first] = std::min(f.second, it->second);
            }
            prime_factors = merged_factors;
        }
        
        n_velocities = 2;
        for (const auto& f : prime_factors)
        {
            n_velocities *= (f.second+1);
        }
    }
    bool operator<(const VelocityInfo& v) const {return n_velocities < v.n_velocities;};
    std::vector<long long> all_velocities() const
    {
        std::vector<long long> velocities = {1};
        for (const auto& f : prime_factors)
        {
            int j_max = velocities.size();
            for (int i = 0; i < f.second; i++)
            {
                for (int j = 0; j < j_max; j++)
                {
                    velocities.push_back(velocities[j]);
                    velocities[j] *= f.first;
                }
            } 
        }
        int max_i = velocities.size();
        for (int i = 0; i < max_i; i++)
        {
            velocities.push_back(base_v - velocities[i]);
            velocities[i] += base_v;
        }
        std::sort(velocities.begin(), velocities.end());
        return velocities;
    }

    std::unordered_map<long long, int> prime_factors;
    int base_v;
    int n_velocities;
};

long long find_common_velocity(std::unordered_map<int, std::vector<long long>> velocity_pairs)
{
    std::vector<VelocityInfo> infos;
    for (const auto& v : velocity_pairs)
    {
        if (v.second.size() == 1)
        {
            continue;
        }
        infos.emplace_back(v.first, v.second);
    }

    std::sort(infos.begin(), infos.end());
    std::vector<long long> possible_velocities = infos[0].all_velocities();
    for (int i = 1; i < infos.size(); i++)
    {
        if (possible_velocities.size() == 1)
        {
            break;
        }
        auto tmp = infos[i].all_velocities();
        std::vector<long long> merged(std::min(possible_velocities.size(), tmp.size()));
        auto it = std::set_intersection(possible_velocities.begin(), possible_velocities.end(), tmp.begin(), tmp.end(), merged.begin());
        merged.resize(it - merged.begin());
        possible_velocities = merged;
    }
    return possible_velocities[0];
}

long long solution()
{
    std::ifstream file("input.txt");
    std::vector<Hailstone> stones;
    std::unordered_map<int, std::vector<long long>> vx_pairs;
    std::unordered_map<int, std::vector<long long>> vy_pairs;
    std::unordered_map<int, std::vector<long long>> vz_pairs;

    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }

        stones.emplace_back(line);
    }

    for (const auto& s : stones)
    {
        if (vx_pairs.find(s.vx) == vx_pairs.end())
        {
            vx_pairs[s.vx] = std::vector<long long>({s.x0});
        }
        else
        {
            vx_pairs[s.vx].push_back(s.x0);
        }
        
        if (vy_pairs.find(s.vy) == vy_pairs.end())
        {
            vy_pairs[s.vy] = std::vector<long long>({s.y0});
        }
        else
        {
            vy_pairs[s.vy].push_back(s.y0);
        }

        if (vz_pairs.find(s.vz) == vz_pairs.end())
        {
            vz_pairs[s.vz] = std::vector<long long>({s.z0});
        }
        else
        {
            vz_pairs[s.vz].push_back(s.z0);
        }
    }

    long long vx = find_common_velocity(vx_pairs);
    long long vy = find_common_velocity(vy_pairs);
    long long vz = find_common_velocity(vz_pairs);

    long long x0 = vx_pairs[vx][0];
    long long y0 = vy_pairs[vy][0];
    // for my input, no hailstones have a vz that matches the rock's vz.
    // know vx and x0 -> solve t -> calculate z0
    long long t = (stones[0].x0 - x0) / (vx - stones[0].vx);
    long long z0 = t*(stones[0].vz - vz) + stones[0].z0;

    return x0 + y0 + z0;
}

int main()
{
    using namespace std::chrono;
    long long output;
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