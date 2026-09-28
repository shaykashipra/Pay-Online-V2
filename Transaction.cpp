#include "Transaction.h"
#include <sstream>
#include <iomanip>
#include <atomic>

// Atomic counter for transaction IDs (Thread-safe)
static std::atomic<int> transactionCounter{1000};

std::string Transaction::generateTransactionId() {
    int id = transactionCounter.fetch_add(1, std::memory_order_relaxed);
    return "TXN" + std::to_string(id);
}

std::string Transaction::toString() const {
    std::string typeStr;
    switch(type) {
        case TransactionType::TRANSFER: typeStr = "TRANSFER"; break;
        case TransactionType::CASH_IN: typeStr = "CASH_IN"; break;
        case TransactionType::CASH_OUT: typeStr = "CASH_OUT"; break;
        case TransactionType::MOBILE_RECHARGE: typeStr = "MOBILE_RECHARGE"; break;
        case TransactionType::ONLINE_PAYMENT: typeStr = "ONLINE_PAYMENT"; break;
        case TransactionType::SALAMI: typeStr = "SALAMI"; break;
    }

    std::stringstream ss;
    ss << "ID: " << transactionId << " | " << timestamp << " | " 
       << typeStr << " | " << senderPhone << " -> " << receiverPhone 
       << " | RS. " << std::fixed << std::setprecision(2) << amount 
       << " | Status: " << status;
    return ss.str();
}
