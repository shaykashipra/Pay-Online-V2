#include "Wallet.h"
#include "Exceptions.h"

Wallet::Wallet(float initialBalance) : balance(initialBalance) {}

float Wallet::getBalance() const {
    std::lock_guard<std::mutex> lock(walletMutex);
    return balance;
}

void Wallet::deposit(float amount) {
    if (amount <= 0) {
        throw InvalidInputException("Deposit amount must be positive.");
    }
    std::lock_guard<std::mutex> lock(walletMutex);
    balance += amount;
}

void Wallet::withdraw(float amount) {
    if (amount <= 0) {
        throw InvalidInputException("Withdrawal amount must be positive.");
    }
    std::lock_guard<std::mutex> lock(walletMutex);
    if (balance < amount) {
        throw InsufficientBalanceException();
    }
    balance -= amount;
}
