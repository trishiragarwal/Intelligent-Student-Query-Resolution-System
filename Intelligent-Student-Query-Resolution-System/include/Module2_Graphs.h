#ifndef MODULE2_GRAPHS_H
#define MODULE2_GRAPHS_H

#include <vector>
#include <string>
#include <queue>
#include <limits>
#include <algorithm>
#include <iostream>

// ============================================================================
// Module 2: Weighted Directed Graph (Adjacency List) for Escalation Routing
// Algorithms: BFS, Dijkstra's Shortest Path, Floyd-Warshall All-Pairs
// ============================================================================
class EscalationGraph {
private:
    struct Edge {
        int to;
        int weight; // Average escalation turnaround time in minutes
    };

    int V;
    std::vector<std::string> officeNames;
    std::vector<std::vector<Edge>> adj;

public:
    explicit EscalationGraph(int numOffices)
        : V(numOffices), officeNames(numOffices), adj(numOffices) {}

    void setOfficeName(int id, const std::string& name) {
        if (id >= 0 && id < V) officeNames[id] = name;
    }

    std::string getOfficeName(int id) const {
        return (id >= 0 && id < V) ? officeNames[id] : "Unknown";
    }

    void addEscalationEdge(int from, int to, int turnaroundMins) {
        if (from >= 0 && from < V && to >= 0 && to < V) {
            adj[from].push_back({to, turnaroundMins});
        }
    }

    // BFS: Returns all reachable offices from a starting office
    std::vector<int> bfsReachable(int start) const {
        std::vector<bool> visited(V, false);
        std::vector<int> order;
        std::queue<int> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            order.push_back(u);
            for (const auto& e : adj[u]) {
                if (!visited[e.to]) {
                    visited[e.to] = true;
                    q.push(e.to);
                }
            }
        }
        return order;
    }

    struct ShortestPathResult {
        int totalMinutes;
        std::vector<int> path;
        bool reachable;
    };

    // Dijkstra's Algorithm: O((V + E) log V) using min-heap
    ShortestPathResult dijkstra(int src, int dest) const {
        const int INF = std::numeric_limits<int>::max() / 4;
        std::vector<int> dist(V, INF);
        std::vector<int> parent(V, -1);

        using P = std::pair<int, int>; // {dist, node}
        std::priority_queue<P, std::vector<P>, std::greater<P>> pq;

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d > dist[u]) continue;

            for (const auto& e : adj[u]) {
                if (dist[u] + e.weight < dist[e.to]) {
                    dist[e.to] = dist[u] + e.weight;
                    parent[e.to] = u;
                    pq.push({dist[e.to], e.to});
                }
            }
        }

        ShortestPathResult res;
        if (dist[dest] >= INF) {
            res.reachable = false;
            res.totalMinutes = -1;
            return res;
        }

        res.reachable = true;
        res.totalMinutes = dist[dest];
        for (int curr = dest; curr != -1; curr = parent[curr]) {
            res.path.push_back(curr);
        }
        std::reverse(res.path.begin(), res.path.end());
        return res;
    }

    // Floyd-Warshall Algorithm: O(V^3) All-Pairs Shortest Turnaround Matrix
    std::vector<std::vector<int>> floydWarshall() const {
        const int INF = std::numeric_limits<int>::max() / 4;
        std::vector<std::vector<int>> dist(V, std::vector<int>(V, INF));

        for (int i = 0; i < V; ++i) dist[i][i] = 0;
        for (int u = 0; u < V; ++u) {
            for (const auto& e : adj[u]) {
                dist[u][e.to] = std::min(dist[u][e.to], e.weight);
            }
        }

        for (int k = 0; k < V; ++k) {
            for (int i = 0; i < V; ++i) {
                for (int j = 0; j < V; ++j) {
                    if (dist[i][k] < INF && dist[k][j] < INF) {
                        dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
                    }
                }
            }
        }
        return dist;
    }

    // Helper: Build default 5-node NIET administrative escalation graph
    static EscalationGraph buildDefaultCampusGraph() {
        EscalationGraph g(5);
        g.setOfficeName(0, "Student Helpdesk");
        g.setOfficeName(1, "Faculty Advisor");
        g.setOfficeName(2, "Department HOD");
        g.setOfficeName(3, "Examination Cell");
        g.setOfficeName(4, "Dean Academics");

        g.addEscalationEdge(0, 1, 15);
        g.addEscalationEdge(0, 2, 35);
        g.addEscalationEdge(1, 2, 15);
        g.addEscalationEdge(1, 3, 25);
        g.addEscalationEdge(2, 3, 10);
        g.addEscalationEdge(2, 4, 20);
        g.addEscalationEdge(3, 4, 15);
        return g;
    }
};

#endif // MODULE2_GRAPHS_H
