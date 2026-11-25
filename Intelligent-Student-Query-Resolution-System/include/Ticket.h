#ifndef TICKET_H
#define TICKET_H

#include <string>
#include <iostream>
#include <iomanip>

struct Ticket {
    int ticketId;
    std::string studentId;
    std::string queryText;
    std::string category;
    int priorityWeight;   // Higher value = more urgent (1-100)
    long long timestamp;  // Lower value = submitted earlier (for FIFO tie-break)
    int estimatedMins;    // Estimated resolution time in minutes
    std::string status;   // "Open", "In-Progress", "Escalated", "Resolved"

    Ticket()
        : ticketId(0), studentId(""), queryText(""), category("General"),
          priorityWeight(0), timestamp(0), estimatedMins(10), status("Open") {}

    Ticket(int id, const std::string& sid, const std::string& text,
           const std::string& cat, int prio, long long ts, int mins = 10,
           const std::string& st = "Open")
        : ticketId(id), studentId(sid), queryText(text), category(cat),
          priorityWeight(prio), timestamp(ts), estimatedMins(mins), status(st) {}

    // Higher priorityWeight comes first; on tie, earlier timestamp (smaller) comes first (FIFO)
    bool hasHigherPriorityThan(const Ticket& other) const {
        if (priorityWeight != other.priorityWeight) {
            return priorityWeight > other.priorityWeight;
        }
        return timestamp < other.timestamp;
    }

    void print() const {
        std::cout << "[ID: " << ticketId
                  << " | Student: " << studentId
                  << " | Cat: " << category
                  << " | Prio: " << priorityWeight
                  << " | TS: " << timestamp
                  << " | Est: " << estimatedMins << "m"
                  << " | Status: " << status << "] "
                  << queryText << "\n";
    }
};

#endif // TICKET_H
