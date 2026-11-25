#include "../include/Ticket.h"
#include "../include/Module1_Trees.h"
#include "../include/Module2_Graphs.h"
#include "../include/Module3_DP.h"
#include "../include/Module4_Backtracking_BnB.h"
#include "../include/Module5_AdvancedDS.h"

#include <iostream>
#include <vector>
#include <string>

void runIntegratedDemo() {
    std::cout << "====================================================================\n";
    std::cout << "  INTELLIGENT STUDENT QUERY RESOLUTION SYSTEM (DSA-II PBL - BCSE0401)\n";
    std::cout << "  Developer: Trishir Agarwal | Roll No: 2501331530280 | NIET Greater Noida\n";
    std::cout << "====================================================================\n\n";

    CategoryTrie trie;
    FAQHashCache faqCache;
    QueryMaxHeap priorityQueue;
    TicketAVLTree registryAVL;
    AuditRedBlackTree auditRB;
    BPlusArchive bplusArchive;
    EscalationGraph campusGraph = EscalationGraph::buildDefaultCampusGraph();

    // ------------------------------------------------------------------
    // MODULE 1: Trees (Trie + FAQ Hash Cache + Max-Heap + AVL Registry)
    // ------------------------------------------------------------------
    std::cout << "[Module 1] Ingesting & Auto-Categorizing Student Queries...\n";

    std::string faqAns;
    if (faqCache.lookup("download admit card", faqAns)) {
        std::cout << "  -> Instant FAQ Cache Hit (O(1)): " << faqAns << "\n";
    }

    std::vector<std::tuple<int, std::string, std::string, long long, int>> rawQueries = {
        {101, "2501331530280", "Urgent exam hallticket missing tomorrow", 1001, 15},
        {102, "2501331530112", "Hostel wifi not working in room", 1002, 20},
        {103, "2501331530145", "Fee receipt and scholarship discrepancy", 1003, 10},
        {104, "2501331530199", "Urgent exam admit card error tomorrow", 1004, 25},
        {105, "2501331530210", "Attendance shortage inquiry for lab", 1005, 10}
    };

    std::vector<Ticket> activeTickets;
    for (const auto& [id, sid, text, ts, mins] : rawQueries) {
        auto [cat, score] = trie.categorizeQuery(text);
        Ticket t(id, sid, text, cat, score, ts, mins, "Open");
        priorityQueue.insert(t);
        registryAVL.insert(t);
        activeTickets.push_back(t);
    }

    std::cout << "  -> AVL Registry Balanced: " << (registryAVL.isBalanced() ? "YES" : "NO")
              << " (Height: " << registryAVL.getTreeHeight() << ")\n";
    std::cout << "  -> Top Priority Query Extracted (Max-Heap + FIFO Tie-Break):\n     ";
    Ticket topTicket = priorityQueue.extractMax();
    topTicket.print();

    // ------------------------------------------------------------------
    // MODULE 2: Graphs (BFS, Dijkstra, Floyd-Warshall)
    // ------------------------------------------------------------------
    std::cout << "\n[Module 2] Graph Escalation Routing (Student Helpdesk -> Dean Academics)...\n";
    auto route = campusGraph.dijkstra(0, 4);
    if (route.reachable) {
        std::cout << "  -> Dijkstra Optimal Escalation Path (" << route.totalMinutes << " mins): ";
        for (size_t i = 0; i < route.path.size(); ++i) {
            std::cout << campusGraph.getOfficeName(route.path[i]);
            if (i + 1 < route.path.size()) std::cout << " -> ";
        }
        std::cout << "\n";
    }
    auto fw = campusGraph.floydWarshall();
    std::cout << "  -> Floyd-Warshall Verification (Helpdesk -> Exam Cell): " << fw[0][3] << " mins\n";

    // ------------------------------------------------------------------
    // MODULE 3: Dynamic Programming (0/1 Knapsack Peak-Load Triage)
    // ------------------------------------------------------------------
    std::cout << "\n[Module 3] Dynamic Programming 0/1 Knapsack Triage (Staff Shift Capacity = 40 mins)...\n";
    auto triage = PeakLoadTriageDP::solveKnapsackTriage(activeTickets, 40);
    std::cout << "  -> Max Urgency Score Achieved: " << triage.maxUrgencyScore
              << " (Time Used: " << triage.totalMinutesUsed << "/40 mins)\n";
    std::cout << "  -> Selected Tickets: ";
    for (const auto& t : triage.selectedTickets) std::cout << "#" << t.ticketId << " ";
    std::cout << "\n";

    // ------------------------------------------------------------------
    // MODULE 4: Backtracking & Branch and Bound
    // ------------------------------------------------------------------
    std::cout << "\n[Module 4A] Backtracking Conflict-Free Advising Slot Scheduler...\n";
    std::vector<AdvisingSlotScheduler::QueryRequest> reqs = {
        {101, "Examination Cell"},
        {103, "Accounts & Finance"},
        {105, "Academic Department"}
    };
    std::vector<AdvisingSlotScheduler::Advisor> advisors = {
        {"Dr. Sharma", "Examination Cell", {true, true}},
        {"Mr. Verma", "Accounts & Finance", {false, true}},
        {"Prof. Gupta", "Academic Department", {true, false}}
    };
    std::vector<AdvisingSlotScheduler::Assignment> scheduleOut;
    if (AdvisingSlotScheduler::schedule(reqs, advisors, scheduleOut)) {
        for (const auto& a : scheduleOut) {
            std::cout << "  -> Ticket #" << a.ticketId << " assigned to " << a.advisorName
                      << " (Slot " << (a.slotIndex + 1) << ")\n";
        }
    }

    std::cout << "\n[Module 4B] Branch & Bound TSP Campus File-Clearance Route (5 Offices)...\n";
    const int INF = CampusClearanceTSP::INF;
    std::vector<std::vector<int>> campusMat = {
        {INF, 10, 15, 20, 12},
        {10, INF, 14, 18, 16},
        {15, 14, INF, 11, 13},
        {20, 18, 11, INF,  9},
        {12, 16, 13,  9, INF}
    };
    std::vector<std::string> stops = {"Admin Block", "Exam Cell", "Accounts", "Library", "Hostel Office"};
    auto tsp = CampusClearanceTSP::solveTSP(campusMat);
    std::cout << "  -> Optimal Clearance Tour (" << tsp.minCost << " mins | Pruned: "
              << tsp.nodesPruned << " nodes): ";
    for (size_t i = 0; i < tsp.optimalTour.size(); ++i) {
        std::cout << stops[tsp.optimalTour[i]];
        if (i + 1 < tsp.optimalTour.size()) std::cout << " -> ";
    }
    std::cout << "\n";

    // ------------------------------------------------------------------
    // MODULE 5: Advanced Data Structures (Red-Black Tree + B+ Archive)
    // ------------------------------------------------------------------
    std::cout << "\n[Module 5] Red-Black Tree Audit Log & B+ Tree Range Archive...\n";
    for (auto t : activeTickets) {
        t.status = "Resolved";
        auditRB.insert(t);
        bplusArchive.archiveTicket(t);
    }
    std::cout << "  -> Red-Black Tree Invariants Valid: "
              << (auditRB.verifyInvariants() ? "PASSED" : "FAILED") << "\n";
    auto rangeRes = bplusArchive.rangeScan(102, 104);
    std::cout << "  -> B+ Tree Leaf-Linked Range Scan [102..104] returned "
              << rangeRes.size() << " archived records.\n";
}

int main() {
    runIntegratedDemo();
    return 0;
}
