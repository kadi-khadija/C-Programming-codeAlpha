//CodeAlpha
//C++ Programming
//Hachemaoui Khadija

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

#define CUSTOMER_FILE "bank_customers.dat"
#define ACCOUNT_FILE "bank_accounts.dat"

//  Transaction class
class Transaction {
public:
    string type;
    float amount;
    string date;
    string details;

    Transaction() : amount(0.0f) {}

    Transaction(string t, float a, string d, string det = "")
        : type(t), amount(a), date(d), details(det) {}
};

// Customer class
class Customer {
public:
    int id;
    char name[50];
    char password[50];

    Customer() : id(0) {
        name[0] = '\0';
        password[0] = '\0';
    }

    string getName() { return string(name); }
    string getPassword() { return string(password); }

    void setName(string n) {
        int len = n.length();
        if (len >= 50) len = 49;
        for (int i = 0; i < len; i++) name[i] = n[i];
        name[len] = '\0';
    }

    void setPassword(string p) {
        int len = p.length();
        if (len >= 50) len = 49;
        for (int i = 0; i < len; i++) password[i] = p[i];
        password[len] = '\0';
    }
};

//  Account class
class Account {
public:
    int accountId;
    int customerId;
    float balance;

    Account() : accountId(0), customerId(0), balance(0.0f) {}
};

//  TransactionRecord (for file storage)
class TransactionRecord {
public:
    int accountId;
    string type;
    float amount;
    string date;
    string details;

    TransactionRecord(int aid, string t, float a, string d, string det = "")
        : accountId(aid), type(t), amount(a), date(d), details(det) {}
};

//  Bank class
class Bank {
private:
    vector<Customer> customers;
    vector<Account> accounts;
    vector<TransactionRecord> transactions;
    int nextCustomerId;
    int nextAccountId;
    int currentCustomerId;  // the logged-in user (0 = if no user is logged in)

    int findCustomerIndex(int id);
    int findAccountByCustomer(int customerId);

    void saveData();
    void loadData();

public:
    Bank();

    void startScreen();
    void registerCustomer();
    int login();
    void bankingMenu();
};

//  Constructor: load data from files
Bank::Bank() {
    currentCustomerId = 0;
    loadData();
}

//  Save all data to files
void Bank::saveData() {
    // Save customers
    ofstream custFile(CUSTOMER_FILE, ios::binary);
    for (int i = 0; i < customers.size(); i++) {
        custFile.write((char*)&customers[i], sizeof(Customer));
    }
    custFile.close();

    // Save accounts
    ofstream accFile(ACCOUNT_FILE, ios::binary);
    for (int i = 0; i < accounts.size(); i++) {
        accFile.write((char*)&accounts[i], sizeof(Account));
    }
    accFile.close();

    // Save transactions
    ofstream transFile("bank_transactions.txt", ios::app);
    for (int i = 0; i < transactions.size(); i++) {
        transFile << transactions[i].accountId << " "
                  << transactions[i].type << " "
                  << transactions[i].amount << " "
                  << transactions[i].date << " "
                  << transactions[i].details << "\n";
    }
    transFile.close();
}

//  Load all data from files
void Bank::loadData() {
    // Load customers
    ifstream custFile(CUSTOMER_FILE, ios::binary);
    if (custFile.is_open()) {
        Customer c;
        while (custFile.read((char*)&c, sizeof(Customer))) {
            customers.push_back(c);
        }
        custFile.close();
    }

    // Load accounts
    ifstream accFile(ACCOUNT_FILE, ios::binary);
    if (accFile.is_open()) {
        Account a;
        while (accFile.read((char*)&a, sizeof(Account))) {
            accounts.push_back(a);
        }
        accFile.close();
    }

    // Load transactions
    ifstream transFile("bank_transactions.txt");
    if (transFile.is_open()) {
        int aid;
        string type, date, details;
        float amount;
        while (transFile >> aid >> type >> amount >> date) {
            getline(transFile, details);  // read the rest as details
            transactions.push_back(TransactionRecord(aid, type, amount, date, details));
        }
        transFile.close();
    }

    // Calculate next IDs
    nextCustomerId = 1;
    for (int i = 0; i < customers.size(); i++) {
        if (customers[i].id >= nextCustomerId)
            nextCustomerId = customers[i].id + 1;
    }

    nextAccountId = 1001;
    for (int i = 0; i < accounts.size(); i++) {
        if (accounts[i].accountId >= nextAccountId)
            nextAccountId = accounts[i].accountId + 1;
    }
}

// Find helpers
int Bank::findCustomerIndex(int id) {
    for (int i = 0; i < customers.size(); i++) {
        if (customers[i].id == id)
            return i;
    }
    return -1;
}

int Bank::findAccountByCustomer(int customerId) {
    for (int i = 0; i < accounts.size(); i++) {
        if (accounts[i].customerId == customerId)
            return i;
    }
    return -1;
}

//  Start screen: Register or Login
void Bank::startScreen() {
    int choice;

    do {
        cout << "\n===== BANKING SYSTEM =====\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: registerCustomer(); break;
            case 2:
                if (login()) {
                    bankingMenu();  // logged in, go to banking
                }
                break;
            case 0: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

//  Register
void Bank::registerCustomer() {
    Customer c;
    c.id = nextCustomerId++;

    string name, password, confirmPass;

    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Create a password: ";
    cin >> password;

    cout << "Confirm password: ";
    cin >> confirmPass;

    if (password != confirmPass) {
        cout << "Error! Passwords do not match.\n";
        nextCustomerId--;
        return;
    }

    if (password.length() < 4) {
        cout << "Error! Password must be at least 4 characters.\n";
        nextCustomerId--;
        return;
    }

    c.setName(name);
    c.setPassword(password);
    customers.push_back(c);

    // Create account
    Account a;
    a.accountId = nextAccountId++;
    a.customerId = c.id;
    a.balance = 0.0f;
    accounts.push_back(a);

    saveData();

    cout << "\nRegistration successful!\n";
    cout << "Your Customer ID: " << c.id << "\n";
    cout << "Your Account ID:  " << a.accountId << "\n";
}

// Login
int Bank::login() {
    int id;
    string password;

    cout << "Enter Customer ID: ";
    cin >> id;

    cout << "Enter Password: ";
    cin >> password;

    int index = findCustomerIndex(id);
    if (index == -1) {
        cout << "Customer not found.\n";
        return 0;
    }

    if (customers[index].getPassword() != password) {
        cout << "Wrong password.\n";
        return 0;
    }

    currentCustomerId = id;
    cout << "Welcome back, " << customers[index].getName() << "!\n";
    return 1;
}

// Banking menu (only shown after login)
void Bank::bankingMenu() {
    int choice;

    do {
        cout << "\n MY ACCOUNT :\n";
        cout << "1. Deposit\n";
        cout << "2. Withdraw\n";
        cout << "3. Transfer\n";
        cout << "4. View Balance\n";
        cout << "5. Transaction History\n";
        cout << "0. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        int accIndex = findAccountByCustomer(currentCustomerId);
        if (accIndex == -1) {
            cout << "No account found.\n";
            break;
        }

        switch (choice) {
            case 1: {
                float amount;
                cout << "Enter amount to deposit: ";
                cin >> amount;
                if (amount <= 0) {
                    cout << "Error! Amount must be positive.\n";
                    break;
                }
                accounts[accIndex].balance += amount;
                transactions.push_back(TransactionRecord(
                    accounts[accIndex].accountId, "Deposit", amount, "2025-06-10", ""));
                saveData();
                cout << "Deposited: " << amount << " | New balance: "
                     << accounts[accIndex].balance << "\n";
                break;
            }
            case 2: {
                float amount;
                cout << "Enter amount to withdraw: ";
                cin >> amount;
                if (amount <= 0) {
                    cout << "Error! Amount must be positive.\n";
                    break;
                }
                if (amount > accounts[accIndex].balance) {
                    cout << "Error! Insufficient balance. Current: "
                         << accounts[accIndex].balance << "\n";
                    break;
                }
                accounts[accIndex].balance -= amount;
                transactions.push_back(TransactionRecord(
                    accounts[accIndex].accountId, "Withdrawal", amount, "2025-06-10", ""));
                saveData();
                cout << "Withdrew: " << amount << " | Remaining: "
                     << accounts[accIndex].balance << "\n";
                break;
            }
            case 3: {
                int receiverId;
                cout << "Enter receiver's Customer ID: ";
                cin >> receiverId;

                int receiverIndex = findCustomerIndex(receiverId);
                if (receiverIndex == -1) {
                    cout << "Receiver not found.\n";
                    break;
                }

                float amount;
                cout << "Enter amount to transfer: ";
                cin >> amount;

                int receiverAcc = findAccountByCustomer(receiverId);

                if (accounts[accIndex].balance < amount) {
                    cout << "Error! Insufficient balance.\n";
                    break;
                }

                accounts[accIndex].balance -= amount;
                accounts[receiverAcc].balance += amount;

                transactions.push_back(TransactionRecord(
                    accounts[accIndex].accountId, "Transfer", amount, "2025-06-10",
                    "To " + customers[receiverIndex].getName()));

                transactions.push_back(TransactionRecord(
                    accounts[receiverAcc].accountId, "Transfer", amount, "2025-06-10",
                    "From " + customers[findCustomerIndex(currentCustomerId)].getName()));

                saveData();
                cout << "Transferred " << amount << " to "
                     << customers[receiverIndex].getName() << "\n";
                cout << "Your new balance: " << accounts[accIndex].balance << "\n";
                break;
            }
            case 4: {
                cout << "\n--- Account Info ---\n";
                cout << "Account ID: " << accounts[accIndex].accountId << "\n";
                cout << "Customer:   "
                     << customers[findCustomerIndex(currentCustomerId)].getName() << "\n";
                cout << "Balance:    " << fixed << setprecision(2)
                     << accounts[accIndex].balance << "\n";
                break;
            }
            case 5: {
                cout << "\n--- Transaction History ---\n";
                cout << left << setw(12) << "Type"
                     << setw(10) << "Amount"
                     << setw(12) << "Date"
                     << "Details" << "\n";
                cout << "------------------------------------------------\n";

                bool found = false;
                for (int i = 0; i < transactions.size(); i++) {
                    if (transactions[i].accountId == accounts[accIndex].accountId) {
                        cout << left << setw(12) << transactions[i].type
                             << setw(10) << fixed << setprecision(2)
                             << transactions[i].amount
                             << setw(12) << transactions[i].date
                             << transactions[i].details << "\n";
                        found = true;
                    }
                }
                if (!found) cout << "No transactions yet.\n";
                break;
            }
            case 0:
                cout << "Logged out.\n";
                currentCustomerId = 0;
                break;
            default:
                cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

int main() {
    Bank bank;
    bank.startScreen();
    return 0;
}