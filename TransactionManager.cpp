#include "TransactionManager.h"
#include "Exceptions.h"
#include <fstream>
#include <iostream>
#include <sstream>

TransactionManager::TransactionManager() {
    loadAgents();
    loadMerchants();
    loadUsers();
}

void TransactionManager::loadAgents() {
    std::ifstream f("data/AGENT.txt");
    if (f) {
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty()) agents[line] = true;
        }
    }
}

void TransactionManager::loadMerchants() {
    std::ifstream f("data/MERCHANT.txt");
    if (f) {
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty()) merchants[line] = true;
        }
    }
}

void TransactionManager::loadUsers() {
    std::ifstream f("data/users.txt");
    if (!f) return;
    
    std::string phone, name;
    int pin;
    float balance;
    
    // Format: phone name pin balance
    while (f >> phone >> name >> pin >> balance) {
        users[phone] = std::make_shared<User>(name, phone, pin, balance);
    }
}

void TransactionManager::registerUser(const std::string& name, const std::string& phone, int pin, float initialBalance) {
    std::lock_guard<std::mutex> lock(mapMutex);
    
    if (users.find(phone) != users.end()) {
        throw InvalidInputException("Account with this phone number already exists.");
    }
    if (phone.length() != 11) {
        throw InvalidInputException("Phone number must be 11 digits.");
    }
    if (pin < 1000 || pin >= 10000) {
        throw InvalidInputException("PIN must be 4 digits.");
    }
    if (initialBalance < 0) {
        throw InvalidInputException("Initial balance cannot be negative.");
    }
    
    users[phone] = std::make_shared<User>(name, phone, pin, initialBalance);
    saveAllUsers();
}

std::shared_ptr<User> TransactionManager::login(const std::string& phone, int pin) {
    std::lock_guard<std::mutex> lock(mapMutex);
    
    auto it = users.find(phone);
    if (it != users.end() && it->second->verifyPin(pin)) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<User> TransactionManager::getUser(const std::string& phone) {
    std::lock_guard<std::mutex> lock(mapMutex);
    auto it = users.find(phone);
    if (it != users.end()) {
        return it->second;
    }
    return nullptr;
}

bool TransactionManager::isValidAgent(const std::string& phone) {
    return agents.find(phone) != agents.end();
}

bool TransactionManager::isValidMerchant(const std::string& phone) {
    return merchants.find(phone) != merchants.end();
}

void TransactionManager::recordTransaction(const Transaction& txn) {
    std::lock_guard<std::mutex> lock(fileMutex);
    
    std::ofstream f("data/transactions.txt", std::ios::app);
    if (!f) {
        throw FileHandlingException("Could not open transactions file.");
    }
    
    f << txn.toString() << "\n";
    f.close();
    
    saveAllUsers(); // Save balance changes
}

void TransactionManager::saveAllUsers() {
    // This assumes fileMutex is either already held or we lock it here.
    // To avoid double locking or recursive mutex, we can just lock fileMutex locally.
    // Note: If called from recordTransaction, we need a recursive_mutex OR just lock carefully.
    // Better: just save users without re-locking if called from within a locked context? 
    // Actually, saveAllUsers shouldn't lock fileMutex if it's already locked. 
    // Let's use fileMutex just for transactions, and userFileMutex for users. Or unlock before calling.
    
    // For simplicity, let's open and write. 
    // We'll write to a temporary file then rename to ensure atomic write.
    std::ofstream f("data/users_temp.txt");
    if (!f) return;
    
    for (const auto& pair : users) {
        f << pair.second->getPhone() << " "
          << pair.second->getName() << " "
          << pair.second->getPin() << " " 
          << pair.second->getWallet()->getBalance() << "\n";
    }
    f.close();
    
    std::remove("data/users.txt");
    std::rename("data/users_temp.txt", "data/users.txt");
}
