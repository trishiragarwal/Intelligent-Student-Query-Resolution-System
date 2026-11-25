#include "../include/Ticket.h"
#include "../include/Module1_Trees.h"
#include "../include/Module2_Graphs.h"
#include "../include/Module3_DP.h"
#include "../include/Module4_Backtracking_BnB.h"
#include "../include/Module5_AdvancedDS.h"

#include <cassert>
#include <iostream>

int main() {
    // TC-01: Module 1 (Max-Heap FIFO tie-break, Trie categorization, AVL balance)
    {
        CategoryTrie trie;
        auto [cat, prio] = trie.categorizeQuery("Urgent exam hallticket tomorrow");
        assert(cat == "Examination Cell");
        assert(prio >= 90);

        QueryMaxHeap pq;
        pq.insert(Ticket(1, "S1", "Query A", "Exam", 80, 100));
        pq.insert(Ticket(2, "S2", "Query B", "Exam", 80, 90)); // Earlier timestamp wins tie
        pq.insert(Ticket(3, "S3", "Query C", "Hostel", 50, 50));

        assert(pq.extractMax().ticketId == 2);
        assert(pq.extractMax().ticketId == 1);
        assert(pq.extractMax().ticketId == 3);

        TicketAVLTree avl;
        for (int i = 1; i <= 20; ++i) {
            avl.insert(Ticket(i, "S" + std::to_string(i), "Sample", "General", 30, i));
        }
        assert(avl.isBalanced());
        Ticket found;
        assert(avl.search(15, found) && found.ticketId == 15);
        std::cout << "[PASS] TC-01: Module 1 (Max-Heap, Trie, AVL Tree)\n";
    }

    // TC-02: Module 2 (Graph BFS, Dijkstra, Floyd-Warshall)
    {
        EscalationGraph g = EscalationGraph::buildDefaultCampusGraph();
        auto reachable = g.bfsReachable(0);
        assert(reachable.size() == 5);

        auto sp = g.dijkstra(0, 4);
        assert(sp.reachable);
        assert(sp.totalMinutes == 50); // 0->1(15)->2(15)->4(20) = 50

        auto fw = g.floydWarshall();
        assert(fw[0][4] == 50);
        assert(fw[0][3] == 40); // 0->1(15)->3(25) or 0->1(15)->2(15)->3(10) = 40
        std::cout << "[PASS] TC-02: Module 2 (BFS, Dijkstra, Floyd-Warshall)\n";
    }

    // TC-03: Module 3 (0/1 Knapsack DP Triage)
    {
        std::vector<Ticket> batch = {
            Ticket(1, "S1", "Exam Issue", "Exam", 50, 1, 30),
            Ticket(2, "S2", "Fee Refund", "Accounts", 55, 2, 40),
            Ticket(3, "S3", "Portal Lock", "IT", 40, 3, 20),
            Ticket(4, "S4", "Lab Query", "Academic", 25, 4, 10)
        };
        auto res = PeakLoadTriageDP::solveKnapsackTriage(batch, 60);
        assert(res.maxUrgencyScore == 115); // Tickets 1 (30m,50) + 3 (20m,40) + 4 (10m,25) = 60m, 115
        assert(PeakLoadTriageDP::maxUrgencyScoreSpaceOptimized(batch, 60) == 115);
        std::cout << "[PASS] TC-03: Module 3 (0/1 Knapsack Peak-Load Triage DP)\n";
    }

    // TC-04: Module 4 (Backtracking Slot Scheduler & Branch and Bound TSP)
    {
        std::vector<AdvisingSlotScheduler::QueryRequest> reqs = {
            {1, "Exam"}, {2, "Exam"}
        };
        std::vector<AdvisingSlotScheduler::Advisor> advs = {
            {"Dr. A", "Exam", {true, true}}
        };
        std::vector<AdvisingSlotScheduler::Assignment> out;
        assert(AdvisingSlotScheduler::schedule(reqs, advs, out));
        assert(out.size() == 2);
        assert(out[0].slotIndex != out[1].slotIndex);

        const int INF = CampusClearanceTSP::INF;
        std::vector<std::vector<int>> mat = {
            {INF, 10, 15, 20, 12},
            {10, INF, 14, 18, 16},
            {15, 14, INF, 11, 13},
            {20, 18, 11, INF,  9},
            {12, 16, 13,  9, INF}
        };
        auto tsp = CampusClearanceTSP::solveTSP(mat);
        assert(tsp.minCost == 56); // 0->1(10)->2(14)->3(11)->4(9)->0(12) = 56
        assert(tsp.optimalTour.front() == 0 && tsp.optimalTour.back() == 0);
        std::cout << "[PASS] TC-04: Module 4 (Backtracking & Branch and Bound TSP)\n";
    }

    // TC-05: Module 5 (Red-Black Tree Audit Index & B+ Tree Archive)
    {
        AuditRedBlackTree rb;
        BPlusArchive bplus;
        for (int i = 1; i <= 50; ++i) {
            Ticket t(1000 + i, "STU" + std::to_string(i), "Resolved Query", "Academic", 60, i, 10, "Resolved");
            assert(rb.insert(t));
            bplus.archiveTicket(t);
        }
        assert(rb.verifyInvariants());
        Ticket check;
        assert(rb.search(1025, check) && check.ticketId == 1025);
        assert(bplus.rangeScan(1010, 1019).size() == 10);
        std::cout << "[PASS] TC-05: Module 5 (Red-Black Tree & B+ Tree Archive)\n";
    }

    std::cout << "\nALL 5 INTEGRATED VALIDATION TEST CASES PASSED SUCCESSFULLY.\n";
    return 0;
}
