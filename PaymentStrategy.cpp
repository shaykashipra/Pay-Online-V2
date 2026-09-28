#include "PaymentStrategy.h"
#include "TransactionManager.h"
#include "Exceptions.h"
#include <chrono>

static std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::string timeStr = std::ctime(&now_time);
    timeStr.pop_back(); // Remove trailing newline
    return timeStr;
}

void TransferStrategy::execute(User* sender, const std::string& receiverIdentifier, float amount, TransactionManager* tm) {
    auto receiver = tm->getUser(receiverIdentifier);
    if (!receiver) {
        throw UserNotFoundException();
    }
    
    if (sender->getPhone() == receiverIdentifier) {
        throw InvalidInputException("Cannot transfer to yourself.");
    }

    // Attempt withdrawal first
    sender->getWallet()->withdraw(amount);
    
    // Attempt deposit
    receiver->getWallet()->deposit(amount);
    
    Transaction txn;
    txn.transactionId = Transaction::generateTransactionId();
    txn.senderPhone = sender->getPhone();
    txn.receiverPhone = receiverIdentifier;
    txn.amount = amount;
    txn.type = TransactionType::TRANSFER;
    txn.timestamp = getCurrentTime();
    txn.status = "SUCCESS";
    
    tm->recordTransaction(txn);
}

void CashOutStrategy::execute(User* sender, const std::string& agentNumber, float amount, TransactionManager* tm) {
    if (!tm->isValidAgent(agentNumber)) {
        throw InvalidInputException("Invalid Agent Number.");
    }
    
    sender->getWallet()->withdraw(amount);
    
    Transaction txn;
    txn.transactionId = Transaction::generateTransactionId();
    txn.senderPhone = sender->getPhone();
    txn.receiverPhone = agentNumber;
    txn.amount = amount;
    txn.type = TransactionType::CASH_OUT;
    txn.timestamp = getCurrentTime();
    txn.status = "SUCCESS";
    
    tm->recordTransaction(txn);
}

void MobileRechargeStrategy::execute(User* sender, const std::string& phoneNumber, float amount, TransactionManager* tm) {
    if (phoneNumber.length() != 11) {
        throw InvalidInputException("Invalid Phone Number length.");
    }
    
    sender->getWallet()->withdraw(amount);
    
    Transaction txn;
    txn.transactionId = Transaction::generateTransactionId();
    txn.senderPhone = sender->getPhone();
    txn.receiverPhone = phoneNumber;
    txn.amount = amount;
    txn.type = TransactionType::MOBILE_RECHARGE;
    txn.timestamp = getCurrentTime();
    txn.status = "SUCCESS";
    
    tm->recordTransaction(txn);
}

void OnlinePaymentStrategy::execute(User* sender, const std::string& merchantNumber, float amount, TransactionManager* tm) {
    if (!tm->isValidMerchant(merchantNumber)) {
        throw InvalidInputException("Invalid Merchant Number.");
    }
    
    sender->getWallet()->withdraw(amount);
    
    Transaction txn;
    txn.transactionId = Transaction::generateTransactionId();
    txn.senderPhone = sender->getPhone();
    txn.receiverPhone = merchantNumber;
    txn.amount = amount;
    txn.type = TransactionType::ONLINE_PAYMENT;
    txn.timestamp = getCurrentTime();
    txn.status = "SUCCESS";
    
    tm->recordTransaction(txn);
}
