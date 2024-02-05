#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iterator>
#include <utility>
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

inline uint16_t id(const std::string& s)
{
    return (s[0]-'a') + 26*(s[1]-'a') + 26*26*(s[2]-'a'); 
}

// copied from wikipedia because understanding this mincut stuff is making my head hurt
std::pair<int, std::vector<int>> global_min_cut(std::vector<std::vector<int>> mat) {
    std::pair<int, std::vector<int>> best = {INT32_MAX, {}};
    int n = mat.size();
    std::vector<std::vector<int>> co(n);

    for (int i = 0; i < n; i++)
        co[i] = {i};

    for (int ph = 1; ph < n; ph++) {
        std::vector<int> w = mat[0];
        size_t s = 0, t = 0;
        for (int it = 0; it < n - ph; it++) { // O(V^2) -> O(E log V) with prio. queue
            w[t] = INT32_MIN;
            s = t, t = std::max_element(w.begin(), w.end()) - w.begin();
            for (int i = 0; i < n; i++) w[i] += mat[t][i];
        }
        best = min(best, {w[t] - mat[t][t], co[t]});
        co[s].insert(co[s].end(), co[t].begin(), co[t].end());
        for (int i = 0; i < n; i++) mat[s][i] += mat[t][i];
        for (int i = 0; i < n; i++) mat[i][s] = mat[s][i];
        mat[0][t] = INT32_MIN;

        if (best.first == 3)  // short cut since we know mincut is 3
        {
            return best;
        }
    }

    return best;
}

int solution()
{
    std::ifstream file("input.txt");
    std::unordered_map<uint16_t, std::unordered_set<uint16_t>> graph;
    std::set<uint16_t> unique_ids;

    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        auto tmp = split(line, ": ");
        uint16_t lhs = id(tmp[0]);
        if (graph.find(lhs) == graph.end())
        {
            graph[lhs] = std::unordered_set<uint16_t>();
        }
        unique_ids.insert(lhs);

        auto rhs = split(tmp[1], " ");
        for (const auto& s : rhs)
        {
            uint16_t rid = id(s);
            unique_ids.insert(rid);
            graph[lhs].insert(rid);
            if (graph.find(rid) == graph.end())
            {
                graph[rid] = std::unordered_set<uint16_t>();
            }
            graph[rid].insert(lhs);
        }
    }
    
    std::vector<std::vector<int>> cost_matrix(unique_ids.size());
    for (const auto& node : graph)
    {
        std::vector<int> weights(unique_ids.size());
        for (const auto neighbor : node.second)
        {
            weights[std::distance(unique_ids.begin(), unique_ids.find(neighbor))] = 1;
        }
        cost_matrix[std::distance(unique_ids.begin(), unique_ids.find(node.first))] = weights;
    }

    auto mincut = global_min_cut(cost_matrix);

    return mincut.second.size()*(cost_matrix.size()-mincut.second.size());
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