#ifndef MODULE3_DP_H
#define MODULE3_DP_H

#include "Ticket.h"
#include <vector>
#include <algorithm>
#include <iostream>

// ============================================================================
// Module 3: Dynamic Programming (0/1 Knapsack for Peak-Load Query Triage)
// Maximizes total urgency score resolved within a fixed staff time capacity W
// ============================================================================
class PeakLoadTriageDP {
public:
    struct TriageResult {
        int maxUrgencyScore;
        int totalMinutesUsed;
        std::vector<Ticket> selectedTickets;
        std::vector<Ticket> deferredTickets;
    };

    // Space-optimized O(W) score computation + full selection trace
    static TriageResult solveKnapsackTriage(const std::vector<Ticket>& tickets, int capacityMins) {
        int n = static_cast<int>(tickets.size());
        std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacityMins + 1, 0));

        for (int i = 1; i <= n; ++i) {
            int w = tickets[i - 1].estimatedMins;
            int v = tickets[i - 1].priorityWeight;
            for (int c = 0; c <= capacityMins; ++c) {
                dp[i][c] = dp[i - 1][c];
                if (w <= c) {
                    dp[i][c] = std::max(dp[i][c], dp[i - 1][c - w] + v);
                }
            }
        }

        // Backtrack to reconstruct selected vs. deferred tickets
        TriageResult result;
        result.maxUrgencyScore = dp[n][capacityMins];
        result.totalMinutesUsed = 0;

        int c = capacityMins;
        std::vector<bool> chosen(n, false);
        for (int i = n; i >= 1; --i) {
            if (dp[i][c] != dp[i - 1][c]) {
                chosen[i - 1] = true;
                c -= tickets[i - 1].estimatedMins;
                result.totalMinutesUsed += tickets[i - 1].estimatedMins;
            }
        }

        for (int i = 0; i < n; ++i) {
            if (chosen[i]) result.selectedTickets.push_back(tickets[i]);
            else result.deferredTickets.push_back(tickets[i]);
        }

        return result;
    }

    // Pure O(W) rolling-array variant when only optimal score is needed
    static int maxUrgencyScoreSpaceOptimized(const std::vector<Ticket>& tickets, int capacityMins) {
        std::vector<int> dp(capacityMins + 1, 0);
        for (const auto& t : tickets) {
            int w = t.estimatedMins;
            int v = t.priorityWeight;
            for (int c = capacityMins; c >= w; --c) {
                dp[c] = std::max(dp[c], dp[c - w] + v);
            }
        }
        return dp[capacityMins];
    }
};

#endif // MODULE3_DP_H
