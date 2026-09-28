#include <iostream>
#include <thread>
#include <vector>
#include <limits>
#include <cstdlib>
#include "TransactionManager.h"
#include "PaymentStrategy.h"
#include "Exceptions.h"

using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void demoMultithreading(TransactionManager& tm) {
    clearScreen();
    cout << "--- MULTITHREADING DEMO ---\n\n";
    
    try {
        tm.registerUser("DemoUser", "01999999999", 1234, 5000.0);
    } catch (...) {} // Ignore if exists
    
    auto user = tm.getUser("01999999999");
    if (!user) {
        cout << "DemoUser setup failed.\n";
        pauseScreen();
        return;
    }

    cout << "Initial Balance: RS. " << user->getWallet()->getBalance() << "\n\n";
    cout << "Thread 1 is cashing out RS. 500.0\n";
    cout << "Thread 2 is cashing out RS. 700.0\n\n";

    // Create a dummy valid agent for the demo (we skip validation in demo or add one)
    // To bypass validation cleanly, we can use TransferStrategy instead to another dummy user.
    try {
        tm.registerUser("DummyAgent", "01700000000", 1111, 0.0);
    } catch (...) {}

    auto withdrawTask = [&](float amount) {
        try {
            TransferStrategy strategy;
            strategy.execute(user.get(), "01700000000", amount, &tm); 
            cout << "[Thread] Successfully transferred RS. " << amount << "\n";
        } catch (const exception& e) {
            cout << "[Thread] Failed to transfer RS. " << amount << " - " << e.what() << "\n";
        }
    };

    thread t1(withdrawTask, 500.0);
    thread t2(withdrawTask, 700.0);

    t1.join();
    t2.join();

    cout << "\nFinal Balance: RS. " << user->getWallet()->getBalance() << " (Expected: 3800)\n";
    cout << "Both operations executed concurrently safely because of std::mutex.\n";
    pauseScreen();
}

class Menu {
private:
    TransactionManager tm;
    
    void displayUserMenu(shared_ptr<User> user) {
        while (true) {
            clearScreen();
            cout << "WELCOME " << user->getName() << "\n";
            cout << "AVAILABLE BALANCE: RS. " << user->getWallet()->getBalance() << "\n\n";
            cout << "1. DEPOSIT\n";
            cout << "2. TRANSFER\n";
            cout << "3. MOBILE RECHARGE\n";
            cout << "4. CASH OUT\n";
            cout << "5. ONLINE PAYMENT\n";
            cout << "6. LOGOUT\n\n";
            cout << "ENTER YOUR CHOICE: ";
            
            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            
            try {
                if (choice == 1) {
                    float amt;
                    cout << "ENTER AMOUNT TO DEPOSIT: RS. ";
                    cin >> amt;
                    user->getWallet()->deposit(amt);
                    
                    Transaction txn;
                    txn.transactionId = Transaction::generateTransactionId();
                    txn.senderPhone = "DEPOSIT";
                    txn.receiverPhone = user->getPhone();
                    txn.amount = amt;
                    txn.type = TransactionType::CASH_IN;
                    txn.timestamp = "N/A"; // Simplified for now
                    txn.status = "SUCCESS";
                    tm.recordTransaction(txn);
                    cout << "DEPOSIT SUCCESSFUL!\n";
                } 
                else if (choice == 2) {
                    string phone;
                    float amt;
                    cout << "ENTER ACCOUNT NUMBER TO TRANSFER: ";
                    cin >> phone;
                    cout << "ENTER AMOUNT TO TRANSFER: RS. ";
                    cin >> amt;
                    
                    TransferStrategy strategy;
                    strategy.execute(user.get(), phone, amt, &tm);
                    cout << "TRANSFER SUCCESSFUL!\n";
                }
                else if (choice == 3) {
                    string phone;
                    float amt;
                    cout << "ENTER MOBILE NUMBER: ";
                    cin >> phone;
                    cout << "ENTER AMOUNT TO RECHARGE: RS. ";
                    cin >> amt;
                    
                    MobileRechargeStrategy strategy;
                    strategy.execute(user.get(), phone, amt, &tm);
                    cout << "RECHARGE SUCCESSFUL!\n";
                }
                else if (choice == 4) {
                    string phone;
                    float amt;
                    cout << "ENTER AGENT NUMBER: ";
                    cin >> phone;
                    cout << "ENTER AMOUNT TO CASHOUT: RS. ";
                    cin >> amt;
                    
                    CashOutStrategy strategy;
                    strategy.execute(user.get(), phone, amt, &tm);
                    cout << "CASHOUT SUCCESSFUL!\n";
                }
                else if (choice == 5) {
                    string phone;
                    float amt;
                    cout << "ENTER MERCHANT NUMBER: ";
                    cin >> phone;
                    cout << "ENTER AMOUNT TO PAY: RS. ";
                    cin >> amt;
                    
                    OnlinePaymentStrategy strategy;
                    strategy.execute(user.get(), phone, amt, &tm);
                    cout << "PAYMENT SUCCESSFUL!\n";
                }
                else if (choice == 6) {
                    break;
                }
            } catch (const exception& e) {
                cout << "\nERROR: " << e.what() << "\n";
            }
            pauseScreen();
        }
    }

public:
    void run() {
        while (true) {
            clearScreen();
            cout << "=== PAY-ONLINE V2 ===\n\n";
            cout << "1. SIGN UP\n";
            cout << "2. LOGIN\n";
            cout << "3. DEMO MULTITHREADING\n";
            cout << "4. EXIT\n\n";
            cout << "ENTER YOUR CHOICE: ";
            
            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            
            if (choice == 1) {
                string name, phone;
                int pin;
                float balance;
                cout << "ENTER YOUR NAME: ";
                cin.ignore();
                getline(cin, name);
                cout << "ENTER YOUR MOBILE NUMBER (11 digits): ";
                cin >> phone;
                cout << "ENTER INITIAL BALANCE: RS. ";
                cin >> balance;
                cout << "ENTER A 4 DIGIT PIN: ";
                cin >> pin;
                
                try {
                    tm.registerUser(name, phone, pin, balance);
                    cout << "ACCOUNT CREATED SUCCESSFULLY!\n";
                } catch (const exception& e) {
                    cout << "\nERROR: " << e.what() << "\n";
                }
                pauseScreen();
            } 
            else if (choice == 2) {
                string phone;
                int pin;
                cout << "ENTER YOUR ACCOUNT NUMBER: ";
                cin >> phone;
                cout << "ENTER YOUR PIN: ";
                cin >> pin;
                
                auto user = tm.login(phone, pin);
                if (user) {
                    displayUserMenu(user);
                } else {
                    cout << "\nACCOUNT NOT FOUND OR INVALID PIN!\n";
                    pauseScreen();
                }
            }
            else if (choice == 3) {
                demoMultithreading(tm);
            }
            else if (choice == 4) {
                break;
            }
        }
    }
};

int main() {
    Menu app;
    app.run();
    return 0;
}
