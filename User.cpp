#include "User.h"

User::User(const std::string& name, const std::string& phone, int pin, float initialBalance)
    : name(name), phone(phone), pin(pin), wallet(std::make_unique<Wallet>(initialBalance)) {}

std::string User::getName() const {
    return name;
}

std::string User::getPhone() const {
    return phone;
}

int User::getPin() const {
    return pin;
}

bool User::verifyPin(int inputPin) const {
    return pin == inputPin;
}

Wallet* User::getWallet() const {
    return wallet.get();
}
