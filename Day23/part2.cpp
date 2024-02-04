#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <tuple>
#include <set>
#include <iterator>
#include <bitset>
#include <chrono>


struct Point
{
    Point(): row(0), col(0){};
    Point(int row, int col): row(row), col(col){};
    inline Point operator+(const Point& p) const {return Point(row+p.row, col+p.col);};
    inline bool operator<(const Point& p) const {return row<p.row || col<p.col;};  // convenience operator for checking out of bounds
    inline bool operator>=(const Point& p) const {return row>=p.row || col>=p.col;};  // convenience operator for checking out of bounds

    template <typename T>
    inline T get(const std::vector<std::vector<T>>& arr) const{return arr[row][col];};
    inline int id(int stride) const {return row*stride + col;};

    int row;
    int col;
};

struct Segment
{
    Segment(){length = 0;}; // length 0 will indicate an invalid segment
    Segment(Point start, Point end, int length): start(start), end(end), length(length){};
    Point start;
    Point end;
    int length;
};

std::vector<Point> offsets = {Point(0, 1), Point(-1, 0), Point(0, -1), Point(1, 0)};

bool valid_point(const Point& p, const std::vector<std::vector<uint8_t>>& explored)
{
    if (p < Point(0, 0) || p >= Point(explored.size(), explored[0].size()))
    {
        return false;  // invalid due to OOB
    }
    if (p.get<uint8_t>(explored) > 0)
    {
        return false;  // invalid due to segment already having been explored
    }
    // Don't need to check if direction would lead into wall bc walls initialize as explored
    return true;
}

Segment generate_segment(const Point& start, uint8_t direction, const std::vector<std::vector<uint8_t>>& map, std::vector<std::vector<uint8_t>>& explored, std::vector<std::vector<uint8_t>>& is_intersection, std::queue<std::tuple<Point, uint8_t>>& to_explore)
{
    bool reversed = false;
    Point current = start + offsets[direction];
    
    if (!valid_point(current, explored))
    {
        return Segment();
    }
    
    int length = 1;
    while (true)  // exit with returns
    {
        reversed |= ((current.get<uint8_t>(map) & (1<<direction)) == 0);

        std::vector<uint8_t> valid_direction;
        for (int i = 0; i <= 3; i++)
        {
            if (i == (direction+2)%4)
            {
                continue;
            }
            if (valid_point(current+offsets[i], explored))
            {
                valid_direction.push_back(i);
            }
        }

        if (valid_direction.size() >= 2 || is_intersection[current.row][current.col] > 0) // we have reached a branch, add options to queue and end current segment
        {
            is_intersection[current.row][current.col] = 1;
            for (const auto dir : valid_direction)
            {
                to_explore.push({current, dir});
            }
            return !reversed ? Segment(start, current, length) : Segment(current, start, length);
        }
        explored[current.row][current.col] = 1; // mark explored after branch so many segments can enter same intersection
        if (valid_direction.size() == 0) // dead end, probably only happens on exiting map
        {
            return !reversed ? Segment(start, current, length) : Segment(current, start, length);
        }
        if (valid_direction.size() == 1) // only one possible way to go, continue in that direction
        {
            direction = valid_direction[0];
            length++;
            current = current + offsets[direction];
        }
    }
}

int max_distance(const std::vector<std::vector<int>>& costs)
{
    int max_dist = 0;
    std::vector<std::bitset<100>> explored_stack;  // big enough for given input
    std::vector<int> node_stack = {0};
    std::vector<int> distance_stack = {0};

    std::bitset<100> init;
    init[0] = 1;
    explored_stack.push_back(init);

    while (!node_stack.empty())
    {
        int current_node = node_stack.back();
        int current_distance = distance_stack.back();
        auto explored = explored_stack.back();
        node_stack.pop_back();
        distance_stack.pop_back();
        explored_stack.pop_back();

        if (current_node == costs.size()-1)
        {
            if (current_distance > max_dist)
            {
                max_dist = current_distance;
            }
            continue;
        }

        const std::vector<int>& edges = costs[current_node];
        for (int i = 0; i < edges.size(); i++)
        {
            if (edges[i] == 0)
            {
                continue;
            }
            if (explored[i])
            {
                continue;
            }
            node_stack.push_back(i);
            distance_stack.push_back(current_distance + edges[i]);
            auto new_explored = explored;
            new_explored[i] = 1;
            explored_stack.push_back(new_explored);
        }
    }
    
    return max_dist;
}

int solution()
{
    std::ifstream file("input.txt");
    std::vector<std::vector<uint8_t>> map;
    std::vector<std::vector<uint8_t>> explored;
    std::vector<std::vector<uint8_t>> intersection;

    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            continue;
        }
        
        std::vector<uint8_t> map_row(line.length());
        std::vector<uint8_t> exp_row(line.length());
        intersection.push_back(std::vector<uint8_t>(line.length(), 0));
        for (int i = 0; i < line.length(); i++)
        {
            switch (line[i])
            {
                case '#': map_row[i] = 0; exp_row[i] = 1; break;
                case '>': map_row[i] = 1; exp_row[i] = 0; break;
                case '^': map_row[i] = 2; exp_row[i] = 0; break;
                case '<': map_row[i] = 4; exp_row[i] = 0; break;
                case 'v': map_row[i] = 8; exp_row[i] = 0; break;
                case '.': map_row[i] = 15; exp_row[i] = 0; break;
            }
        }
        map.push_back(map_row);
        explored.push_back(exp_row);
    }
    
    auto start_row = map[0];
    int start_col = 0;
    for (int i = 0; i < start_row.size(); i++)
    {
        if (start_row[i] == 15)
        {
            start_col = i;
            break;
        }
    }

    int stride = map[0].size();
    std::vector<Segment> segments;
    std::set<int> unique_points;
    std::queue<std::tuple<Point, uint8_t>> to_explore({std::tuple<Point, uint8_t>(Point(0, start_col), 3)});

    while (!to_explore.empty())
    {
        auto current = to_explore.front();
        to_explore.pop();

        Segment s = generate_segment(std::get<0>(current), std::get<1>(current), map, explored, intersection, to_explore);
        if (s.length > 0)
        {
            unique_points.insert(s.start.id(stride));
            unique_points.insert(s.end.id(stride));
            segments.push_back(s);
        }
    }
    
    std::vector<std::vector<int>> cost_matrix;
    
    for (int i = 0; i < unique_points.size(); i++)
    {
        cost_matrix.push_back(std::vector<int>(unique_points.size(), 0));
    }
    for (const auto& segment : segments)
    {
        int start_idx = std::distance(unique_points.begin(), unique_points.find(segment.start.id(stride)));
        int end_idx = std::distance(unique_points.begin(), unique_points.find(segment.end.id(stride)));
        cost_matrix[start_idx][end_idx] = segment.length;
        cost_matrix[end_idx][start_idx] = segment.length;
    }

    return max_distance(cost_matrix);
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