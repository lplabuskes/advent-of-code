#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <chrono>


struct Node
{
    Node(int row, int col, bool is_vertical): row(row), col(col), is_vertical(is_vertical){};
    inline bool operator==(const Node& n) const
    {
        return (row == n.row) && (col == n.col) && (is_vertical == n.is_vertical);
    }
    struct HashFunction
    {
        std::size_t operator()(const Node& n) const
        {
            return (std::hash<int>()(n.row)) ^ (std::hash<int>()(n.col)<<2) ^ (std::hash<bool>()(n.is_vertical)>>1);
        }
    };
    bool operator<(const Node& n) const {return heuristic < n.heuristic;};
    bool operator>(const Node& n) const {return heuristic > n.heuristic;};
    int row;
    int col;
    bool is_vertical;
    mutable uint16_t distance = 0;
    mutable uint16_t heuristic = 0;
};

void insert_node(Node& node, std::priority_queue<Node, std::vector<Node>, std::greater<Node>>& unexplored, std::unordered_set<Node, Node::HashFunction> explored)
{
    auto ex_it = explored.find(node);
    if (ex_it != explored.end())
    {
        return;
    }

    unexplored.push(node);
}

int min_cost(int start_row, int start_col, int goal_row, int goal_col, const std::vector<std::vector<uint8_t>>& weights)
{
    int max_row = weights.size() - 1, max_col = weights[0].size() - 1;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> unexplored;

    Node n1 = Node(start_row, start_col, true), n2 = Node(start_row, start_col, false);

    unexplored.push(n1);
    unexplored.push(n2);

    std::unordered_set<Node, Node::HashFunction> explored;

    while (unexplored.size() > 0)
    {
        Node current = unexplored.top();
        unexplored.pop();

        if (explored.find(current) != explored.end()) // essentially, a higher weight version of a node we already explored
        {
            continue;
        }

        explored.insert(current);

        if (current.row == goal_row && current.col == goal_col)
        {
            return current.distance;
        }

        if (current.is_vertical)
        {
            int cost_right = 0, cost_left = 0;
            for (int i = 1; i <= 3; i++)
            {
                if (current.col + i <= max_col)
                {
                    cost_right += weights[current.row][current.col+i];
                    Node next(current.row, current.col+i, false);
                    next.distance = current.distance + cost_right;
                    next.heuristic = next.distance + (goal_row - next.row) + (goal_col - next.col);
                    insert_node(next, unexplored, explored);
                }
                if (current.col - i >= 0)
                {
                    cost_left += weights[current.row][current.col-i];
                    Node next(current.row, current.col-i, false);
                    next.distance = current.distance + cost_left;
                    next.heuristic = next.distance + (goal_row - next.row) + (goal_col - next.col);
                    insert_node(next, unexplored, explored);
                }
            }
        }
        else
        {
            int cost_up = 0, cost_down = 0;
            for (int i = 1; i <= 3; i++)
            {
                if (current.row + i <= max_row)
                {
                    cost_down += weights[current.row+i][current.col];
                    Node next(current.row+i, current.col, true);
                    next.distance = current.distance + cost_down;
                    next.heuristic = next.distance + (goal_row - next.row) + (goal_col - next.col);
                    insert_node(next, unexplored, explored);
                }
                if (current.row - i >= 0)
                {
                    cost_up += weights[current.row-i][current.col];
                    Node next(current.row-i, current.col, true);
                    next.distance = current.distance + cost_up;
                    next.heuristic = next.distance + (goal_row - next.row) + (goal_col - next.col);
                    insert_node(next, unexplored, explored);
                }
            }
        }
    }
    return INT32_MAX;
}

int solution()
{
    std::ifstream file("input.txt");
    std::vector<std::vector<uint8_t>> weights;

    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }

        std::vector<uint8_t> row_weights(line.length());
        for (int i = 0; i < line.length(); i++)
        {
            row_weights[i] = line[i] - '0';
        }
        weights.push_back(row_weights);
    }

    return min_cost(0, 0, weights.size()-1, weights[0].size()-1, weights);
}

int main()
{
    using namespace std::chrono;
    int output;
    int n_iter = 1;
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