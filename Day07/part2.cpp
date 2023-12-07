#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>


// Simple remap of hand_type, joker_count to new hand_type
std::vector<std::vector<int>> joker_retype = {{0, 1, 3, 5, 6, 6}, {1, 3, 5, 6}, {2, 4}, {3, 5, 6}, {4}, {5, 6}, {6}};

int card_to_score(char c)
{
    switch (c)
    {
        case 'A':
            return 12;
        case 'K':
            return 11;
        case 'Q':
            return 10;
        case 'J':
            return 0;
        case 'T':
            return 9;
        default:
            return (c - '0') - 1;
    }
}

struct Hand
{
    Hand(std::string s)
    {
        for (int i = 0; i < 5; i++)
        {
            hand_in_order.push_back(card_to_score(s[i]));
        }
        bid = std::stoi(s.substr(6));

        int rank_counts[13] = {0};
        for (int r : hand_in_order)
        {
            rank_counts[r]++;
        }

        for (int i = 1; i < 13; i++)
        {
            int count = rank_counts[i];
            if (count == 5)
            {
                hand_type = 6;
            }
            else if (count == 4)
            {
                hand_type = 5;
            }
            else if (count == 3)
            {
                hand_type += 3;
            }
            else if (count == 2)
            {
                hand_type++;
            }
        }
        hand_type = joker_retype[hand_type][rank_counts[0]];
    }
    bool operator<(const Hand& h) const
    {
        if (hand_type != h.hand_type)
        {
            return hand_type < h.hand_type;
        }
        for (int i = 0; i < hand_in_order.size(); i++)
        {
            if (hand_in_order[i] != h.hand_in_order[i])
            {
                return hand_in_order[i] < h.hand_in_order[i];
            }
        }
        // hands are completely equal, return whatever
        return true;
    }

    std::vector<int> hand_in_order;
    int hand_type = 0;
    int bid = 0;
};

int solution()
{
    std::ifstream file("input.txt");
    std::vector<Hand> hands;
    while (file)
    {
        std::string line;
        std::getline(file, line);
        if (line.length() == 0)
        {
            break;
        }
        hands.push_back(Hand(line));
    }
    std::sort(hands.begin(), hands.end());

    int total_winnings = 0;
    for (int i = 0; i < hands.size(); i++)
    {
        total_winnings += (i + 1) * hands[i].bid;
    }
    return total_winnings;
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