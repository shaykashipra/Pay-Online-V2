# Pay-Online V2

## Overview
This is an educational C++ digital payment simulator, designed to demonstrate core object-oriented programming (OOP) principles, thread synchronization, exception handling, and standard library (STL) usage. It simulates a bKash-like digital wallet experience.

## Features
- **User Registration/Login:** Create accounts and login securely with PIN validation.
- **Wallet Management:** Deposit and check balances.
- **Money Transfer:** Transfer money to other registered users.
- **Mobile Recharge & Cash Out:** Process transactions to predefined phone numbers or agents.
- **Online Payment:** Pay merchants from your wallet.
- **Multithreading:** Demonstrate safe concurrent access to the wallet balance using `std::mutex`.
- **Exception Handling:** Robust error management using custom exceptions (e.g., `InsufficientBalanceException`).
- **File Persistence:** Data is stored in simple `.txt` files.
- **Design Pattern:** Uses the Strategy Pattern for executing different payment workflows.
- **Transaction ID Generation:** Uses `std::atomic<int>` to generate thread-safe, unique IDs.

## C++ Concepts Demonstrated
- **OOP:** Encapsulation (private members), Abstraction (hiding complexity), Polymorphism (virtual methods in strategies).
- **STL:** `std::unordered_map` for fast memory caching of users, `std::string`, `std::vector`, `std::shared_ptr`.
- **Exception Handling:** `try/catch` blocks for validation errors to avoid application crashes.
- **File Handling:** Utilizing `std::ifstream` and `std::ofstream` to read and write state.
- **Multithreading:** `std::thread`, `std::mutex`, `std::lock_guard`, and `std::atomic` to ensure thread-safe operations and prevent race conditions.
- **Design Pattern:** Strategy Pattern abstracts different transaction behaviors into dedicated classes.
- **RAII:** Using `std::lock_guard` for automatic mutex unlocking, and smart pointers for memory management.

## Architecture
- `User`: Represents an individual user with a name, phone number, and a `Wallet`.
- `Wallet`: Manages the user's balance and protects it from concurrent modification using a `std::mutex`.
- `TransactionManager`: A central facade that manages user lookup using `std::unordered_map`, writes data to the disk, and tracks transactions.
- `PaymentStrategy`: An abstract base class for processing payments (`TransferStrategy`, `CashOutStrategy`, etc.).

## Multithreading
- **Why threads are used:** To simulate a real-world scenario where multiple transactions (like a withdrawal and an online payment) might occur on the same account simultaneously.
- **Shared Data:** The `balance` inside the `Wallet` class.
- **Race Condition Prevention:** Without synchronization, two threads could read the balance simultaneously and overwrite each other's changes. We use a `std::mutex` inside `Wallet` and wrap critical sections with `std::lock_guard` to ensure only one thread can update the balance at a time.
- **Atomic Transaction IDs:** We use `std::atomic<int>` to generate transaction IDs without requiring a separate lock.

## Design Pattern
- **Strategy Pattern:** The `PaymentStrategy` interface defines a common `execute()` method. Different payment methods (e.g., `CashOutStrategy`, `OnlinePaymentStrategy`) implement this method. This adheres to the Open-Closed Principle, allowing new payment methods to be added without modifying the core transaction manager.

## Data Persistence
State is managed via text files in the `data/` directory:
- `users.txt`: Stores user credentials and balances.
- `transactions.txt`: Logs all successful transactions.
- `AGENT.txt` and `MERCHANT.txt`: Pre-defined lists of valid agents and merchants.

## Build Instructions
To compile the project using standard `g++`, open your terminal in the project root and run:
```bash
g++ -std=c++17 src/main.cpp src/User.cpp src/Wallet.cpp src/Transaction.cpp src/TransactionManager.cpp src/PaymentStrategy.cpp -Iinclude -o Pay-Online
```

## Example Workflow
1. Sign Up (Provide Name, Mobile, Balance, PIN)
2. Login
3. Run "Demo Multithreading" to see concurrent withdrawals working safely.
4. Try out Transfer, Deposit, or Mobile Recharge.
5. Exit (Data will be saved).

## Limitations
This is an educational simulation and is not intended for real financial transactions or production banking use. Passwords and PINs are not hashed, and data is stored in plain text.
