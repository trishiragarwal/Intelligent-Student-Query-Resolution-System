#ifndef MODULE4_BACKTRACKING_BNB_H
#define MODULE4_BACKTRACKING_BNB_H

#include <vector>
#include <string>
#include <queue>
#include <limits>
#include <iostream>

// ============================================================================
// 4A. Backtracking Constraint Solver: Conflict-Free Advising Slot Scheduling
// Assigns each student grievance to a compatible advisor and free time slot
// ============================================================================
class AdvisingSlotScheduler {
public:
    struct QueryRequest {
        int ticketId;
        std::string requiredDept;
    };

    struct Advisor {
        std::string name;
        std::string dept;
        std::vector<bool> slotAvailable; // e.g., 3 slots: 10AM, 11AM, 12PM
    };

    struct Assignment {
        int ticketId;
        std::string advisorName;
        int slotIndex;
    };

private:
    static bool backtrack(int idx,
                          const std::vector<QueryRequest>& requests,
                          std::vector<Advisor>& advisors,
                          std::vector<Assignment>& current) {
        if (idx == static_cast<int>(requests.size())) {
            return true;
        }

        const auto& req = requests[idx];
        for (auto& adv : advisors) {
            if (adv.dept != req.requiredDept) continue;
            for (int s = 0; s < static_cast<int>(adv.slotAvailable.size()); ++s) {
                if (adv.slotAvailable[s]) {
                    // Choose
                    adv.slotAvailable[s] = false;
                    current.push_back({req.ticketId, adv.name, s});

                    // Explore
                    if (backtrack(idx + 1, requests, advisors, current)) {
                        return true;
                    }

                    // Undo (Backtrack)
                    current.pop_back();
                    adv.slotAvailable[s] = true;
                }
            }
        }
        return false;
    }

public:
    static bool schedule(const std::vector<QueryRequest>& requests,
                         std::vector<Advisor> advisors,
                         std::vector<Assignment>& outAssignments) {
        outAssignments.clear();
        return backtrack(0, requests, advisors, outAssignments);
    }
};

// ============================================================================
// 4B. Branch and Bound (Reduced Cost Matrix TSP)
// Computes optimal minimum-time campus office file-clearance tour
// ============================================================================
class CampusClearanceTSP {
public:
    static constexpr int INF = 1000000000;

    struct TSPResult {
        int minCost;
        std::vector<int> optimalTour;
        int nodesEvaluated;
        int nodesPruned;
    };

private:
    struct Node {
        std::vector<std::vector<int>> reducedMatrix;
        std::vector<int> path;
        int cost;
        int vertex;
        int level;

        bool operator>(const Node& other) const {
            return cost > other.cost;
        }
    };

    static int reduceMatrix(std::vector<std::vector<int>>& mat) {
        int n = static_cast<int>(mat.size());
        int reductionCost = 0;

        // Row reduction
        for (int i = 0; i < n; ++i) {
            int rowMin = INF;
            for (int j = 0; j < n; ++j) {
                if (mat[i][j] < rowMin) rowMin = mat[i][j];
            }
            if (rowMin != INF && rowMin > 0) {
                reductionCost += rowMin;
                for (int j = 0; j < n; ++j) {
                    if (mat[i][j] != INF) mat[i][j] -= rowMin;
                }
            }
        }

        // Column reduction
        for (int j = 0; j < n; ++j) {
            int colMin = INF;
            for (int i = 0; i < n; ++i) {
                if (mat[i][j] < colMin) colMin = mat[i][j];
            }
            if (colMin != INF && colMin > 0) {
                reductionCost += colMin;
                for (int i = 0; i < n; ++i) {
                    if (mat[i][j] != INF) mat[i][j] -= colMin;
                }
            }
        }
        return reductionCost;
    }

public:
    static TSPResult solveTSP(const std::vector<std::vector<int>>& costMatrix) {
        int n = static_cast<int>(costMatrix.size());
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

        Node root;
        root.reducedMatrix = costMatrix;
        root.path = {0};
        root.vertex = 0;
        root.level = 0;
        root.cost = reduceMatrix(root.reducedMatrix);

        pq.push(root);

        TSPResult best;
        best.minCost = INF;
        best.nodesEvaluated = 0;
        best.nodesPruned = 0;

        while (!pq.empty()) {
            Node curr = pq.top();
            pq.pop();

            if (curr.cost >= best.minCost) {
                best.nodesPruned++;
                continue;
            }

            best.nodesEvaluated++;

            if (curr.level == n - 1) {
                int returnCost = costMatrix[curr.vertex][0];
                if (returnCost != INF) {
                    int actualCost = 0;
                    for (size_t i = 0; i + 1 < curr.path.size(); ++i) {
                        actualCost += costMatrix[curr.path[i]][curr.path[i + 1]];
                    }
                    actualCost += returnCost;
                    if (actualCost < best.minCost) {
                        best.minCost = actualCost;
                        best.optimalTour = curr.path;
                        best.optimalTour.push_back(0);
                    }
                }
                continue;
            }

            for (int j = 0; j < n; ++j) {
                if (curr.reducedMatrix[curr.vertex][j] != INF) {
                    Node child;
                    child.reducedMatrix = curr.reducedMatrix;
                    for (int k = 0; k < n; ++k) {
                        child.reducedMatrix[curr.vertex][k] = INF;
                        child.reducedMatrix[k][j] = INF;
                    }
                    // Block premature return to start node 0 until the last stop
                    if (curr.level + 1 < n - 1) {
                        child.reducedMatrix[j][0] = INF;
                    }

                    child.cost = curr.cost + curr.reducedMatrix[curr.vertex][j]
                                 + reduceMatrix(child.reducedMatrix);
                    child.level = curr.level + 1;
                    child.vertex = j;
                    child.path = curr.path;
                    child.path.push_back(j);

                    if (child.cost < best.minCost) {
                        pq.push(child);
                    } else {
                        best.nodesPruned++;
                    }
                }
            }
        }
        return best;
    }
};

#endif // MODULE4_BACKTRACKING_BNB_H
