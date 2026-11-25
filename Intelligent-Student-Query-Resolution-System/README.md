# Intelligent Student Query Resolution System

A modular C++17 algorithmic platform developed for **Data Structures and Algorithms – II (BCSE0401)** Project-Based Learning (PBL) at **NIET Greater Noida**, aligned with **UN Sustainable Development Goal 4 (Quality Education)**.

The system automates ingestion, categorization, prioritization, escalation routing, peak-load triage, advising slot scheduling, campus file-clearance routing, and audit indexing of university student queries using all five syllabus modules of DSA-II.

---

## Project Metadata

| Field | Details |
| :--- | :--- |
| **Course** | Data Structures and Algorithm-II (`BCSE0401`) |
| **Student Name** | Trishir Agarwal |
| **Roll Number** | `2501331530280` |
| **Program** | B.Tech, Computer Science and Engineering (NIET Greater Noida) |
| **Faculty Mentor** | Mr. Adnan Naseem |
| **Assignment Type** | Individual Assignment |
| **SDG Alignment** | SDG 4 – Quality Education |

---

## Syllabus Module Mapping

| Unit / Module | Data Structures & Algorithms Implemented | System Role | Complexity |
| :--- | :--- | :--- | :--- |
| **Module 1: Trees** | Binary Max-Heap, Trie (Prefix Tree), AVL Tree, Hash Map | Urgency prioritization with FIFO timestamp tie-breaking, keyword auto-categorization, balanced ticket registry, and $O(1)$ FAQ lookup | $O(\log n)$ heap/AVL, $O(L)$ Trie, $O(1)$ FAQ |
| **Module 2: Graphs** | Weighted Directed Graph (Adjacency List), BFS, Dijkstra's Algorithm, Floyd-Warshall | Departmental escalation pathways, reachability verification, and minimum-turnaround routing | $O((V+E)\log V)$ Dijkstra, $O(V^3)$ Floyd-Warshall |
| **Module 3: Dynamic Programming** | 0/1 Knapsack DP (2D trace + $O(W)$ rolling array) | Peak-load query triage maximizing resolved urgency score within fixed staff time capacity $W$ | $O(nW)$ time, $O(W)$ space |
| **Module 4: Backtracking, Branch & Bound** | Backtracking Constraint Solver, Reduced-Cost-Matrix Branch & Bound TSP | Conflict-free faculty advising slot assignment & minimum-time multi-office file clearance tour | Pruned state-space search |
| **Module 5: Advanced Data Structures** | Red-Black Tree, Leaf-Linked B+ Tree Archive | Write-heavy in-memory audit log index (strict RB invariants) & sequential range compliance scans | $O(\log n)$ RB-Tree ($\le 2$ rotations/insert) |

---

## Repository Structure

```text
Intelligent-Student-Query-Resolution-System/
├── include/
│   ├── Ticket.h                   # Core ticket schema & FIFO priority comparator
│   ├── Module1_Trees.h            # Max-Heap, Trie, AVL Tree, FAQ Hash Cache
│   ├── Module2_Graphs.h           # Adjacency List, BFS, Dijkstra, Floyd-Warshall
│   ├── Module3_DP.h               # 0/1 Knapsack Peak-Load Triage DP
│   ├── Module4_Backtracking_BnB.h # Backtracking Scheduler & Branch-and-Bound TSP
│   └── Module5_AdvancedDS.h       # Red-Black Tree Audit Log & B+ Tree Archive
├── src/
│   └── main.cpp                   # End-to-end integrated console demonstration
├── tests/
│   └── test_suite.cpp             # Automated validation suite (TC-01 to TC-05)
├── Makefile                       # Build & test automation
├── .gitignore
└── README.md
```

---

## Build & Run Instructions

### Prerequisites
- Any C++17 compatible compiler (`g++` / `clang++` / MinGW-w64)

### Using `g++` Directly
```bash
# Compile and run the integrated application
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/main.cpp -o query_system
./query_system

# Compile and run the automated validation test suite (TC-01 to TC-05)
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude tests/test_suite.cpp -o test_suite
./test_suite
```

### Using `make` (Linux / WSL / macOS)
```bash
make run    # Builds and runs the main integrated demo
make test   # Builds and executes TC-01 through TC-05
make clean  # Removes compiled binaries
```

---

## Validation Test Cases (`tests/test_suite.cpp`)

- **TC-01 (Module 1):** Verifies Trie keyword categorization, Max-Heap FIFO tie-breaking for equal-priority tickets, and AVL height balance across 20 insertions.
- **TC-02 (Module 2):** Validates BFS reachability, Dijkstra's shortest escalation path (`Student Helpdesk -> Dean Academics`), and Floyd-Warshall all-pairs matrix agreement.
- **TC-03 (Module 3):** Confirms optimal 0/1 Knapsack subset selection and verifies agreement with the $O(W)$ space-optimized rolling array.
- **TC-04 (Module 4):** Tests conflict-free Backtracking slot assignment and exact Branch & Bound TSP tour optimality over a 5-office campus matrix.
- **TC-05 (Module 5):** Inserts 50 resolved audit records into the Red-Black Tree, verifies all RB-Tree color and black-height invariants, and validates B+ Tree leaf-linked range scans.
