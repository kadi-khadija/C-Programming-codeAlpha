//CodeAlpha
//C++ Programming
//Hachemaoui Khadija

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define FILENAME "users.txt"

void registerUser();
void loginUser();
bool usernameExists(string username);
string encode(string text);
string decode(string text);

int main() {
    int choice;

    do {
        cout << "\n LOGIN & REGISTRATION SYSTEM :" << endl;
        cout << "1. Register" << endl;
        cout << "2. Login" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: registerUser(); break;
            case 2: loginUser(); break;
            case 0: cout << "Goodbye!" << endl; break;
            default: cout << "Invalid choice." << endl;
        }
    } while (choice != 0);

    return 0;
}

string encode(string text) {
    string result = "";
    int key = 3;

    for (int i = 0; i < text.length(); i++) {
        result += (char)(text[i] + key);
    }

    return result;
}

string decode(string text) {
    string result = "";
    int key = 3;

    for (int i = 0; i < text.length(); i++) {
        result += (char)(text[i] - key);
    }

    return result;
}

bool usernameExists(string username) {
    ifstream file(FILENAME);
    string storedUser, storedPass;

    while (file >> storedUser >> storedPass) {
        if (storedUser == username) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

void registerUser() {
    string username, password, confirmPassword;

    cout << " REGISTER " << endl;

    cout << "Enter username: ";
    cin >> username;

    if (usernameExists(username)) {
        cout << "Error! Username '" << username << "' is already taken." << endl;
        return;
    }

    cout << "Enter password: ";
    cin >> password;

    cout << "Confirm password: ";
    cin >> confirmPassword;

    if (password != confirmPassword) {
        cout << "Error! Passwords do not match." << endl;
        return;
    }

    if (password.length() < 4) {
        cout << "Error! Password must be at least 4 characters." << endl;
        return;
    }

    ofstream file(FILENAME, ios::app);
    file << username << " " << encode(password) << endl;
    file.close();

    cout << "Registration successful! Welcome, " << username << "!" << endl;
}

void loginUser() {
    string username, password;
    string storedUser, storedPass;

    cout << " LOGIN " << endl;

    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    cin >> password;

    ifstream file(FILENAME);
    bool found = false;

    while (file >> storedUser >> storedPass) {
        if (storedUser == username && storedPass == encode(password)) {
            found = true;
            break;
        }
    }
    file.close();

    if (found) {
        cout << "Login successful! Welcome back, " << username << "!" << endl;
    } else {
        cout << "Login failed! Invalid username or password." << endl;
    }
}