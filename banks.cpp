#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <ctime>
#include <iomanip> // For fixed decimal in output
#include <stdexcept>
#include <conio.h>  // For getch() to hide passwords in Windows
//-------------------------------------------

using namespace std;

class User {
private:
    string accountNumber;
    string email;
    string name;
    string phoneNumber;
    string dob;           // Store as string in "dd/mm/yyyy" format
    int age;
    string fatherName;
    string fatherPhoneNumber; // Only for student accounts
    string address;
    string pinCode;
    double balance;
    string accountType;    // "Student" or "Regular"
    string password;

public:
    User() : balance(0.0), age(0){}

    string getEmail() const { return email; }
    string getPhoneNumber() const { return phoneNumber; }
    string getAccountNumber() const { return accountNumber; }
    double getBalance() const { return balance; }
    void setBalance(double newBalance) { balance = newBalance; }

    bool isValidEmail(const string &email) {
        regex emailPattern(R"((\w+)(\.{0,1})(\w*)@(\w+)\.(\w+))");
        return regex_match(email, emailPattern);
    }

    bool isValidPinCode(const string &pin) {
        return regex_match(pin, regex(R"(\d{6})"));
    }

    int calculateAge(const string &dob) {
        int day, month, year;
        char delim;
        stringstream ss(dob);
        ss >> day >> delim >> month >> delim >> year;

        time_t t = time(0);
        tm *now = localtime(&t);
        int currentYear = now->tm_year + 1900;
        int currentMonth = now->tm_mon + 1;
        int currentDay = now->tm_mday;

        int age = currentYear - year;
        if (currentMonth < month || (currentMonth == month && currentDay < day)) {
            age--;
        }
        return age;
    }


    void createUser() {
        do {
            cout << "Enter Email ID: ";
            cin >> email;
            if (!isValidEmail(email)) {
                cout << "Invalid email format. Please try again.\n";
            }
        } while (!isValidEmail(email));

        cin.ignore();
        cout << "Enter Name: ";
        getline(cin, name);

        cout << "Enter Phone Number: ";
        cin >> phoneNumber;

        do {
            cout << "Enter Date of Birth (dd/mm/yyyy): ";
            cin >> dob;
            age = calculateAge(dob);
            if (age <= 0) {
                cout << "Invalid date. Please try again.\n";
            }
        } while (age <= 0);

        if (age < 18) {
            accountType = "Student";
            cout << "Enter Father's Name: ";
            cin.ignore();
            getline(cin, fatherName);

            cout << "Enter Father's Phone Number: ";
            cin >> fatherPhoneNumber;
        } else {
            accountType = "Regular";
            cin.ignore();
            cout << "Enter Father's Name: ";
            getline(cin, fatherName);
        }

        cout << "Enter Address: ";
        getline(cin, address);

        do {
            cout << "Enter Pin Code: ";
            cin >> pinCode;
            if (!isValidPinCode(pinCode)) {
                cout << "Invalid pin code. Please enter a 6-digit number.\n";
            }
        } while (!isValidPinCode(pinCode));

        cout << "Create Password: ";
        char ch;
        while((ch = _getch()) != '\r') {
            password.push_back(ch);
            cout << '*';
        }
        cout << endl;

        cout << "Account Type: " << accountType << " (Age: " << age << ")\n";
    }

    void setAccountNumber(const string &accNum) {
        accountNumber = accNum;
    }

    bool authenticate(const string &inputEmail, const string &inputAccount, const string &inputPassword) {
        return (email == inputEmail && accountNumber == inputAccount && password == inputPassword);
    }

    void displayUser() const {
        cout << "Account Number: " << accountNumber << "\nName: " << name << "\nBalance: " << balance << "\n";
        cout << "Account Type: " << accountType << " (Age: " << age << ")\n";
    }
    bool loadUser(ifstream &file) {
        string line;
         if (getline(file, line)) {
            stringstream ss(line);
            string balanceStr;
            getline(ss, accountNumber, ',');
            getline(ss, email, ',');
            getline(ss, name, ',');
            getline(ss, phoneNumber, ',');
            getline(ss, dob, ',');
            getline(ss, fatherName, ',');
            getline(ss, fatherPhoneNumber, ',');
            getline(ss, address, ',');
            getline(ss, pinCode, ',');
            getline(ss, balanceStr, ',');
            getline(ss, password, ',');

            // Convert balance to double
             balance = stod(balanceStr);
            age = calculateAge(dob);
            accountType = (age < 18) ? "Student" : "Regular";
            return true;
         }
        return false;
    }

    void saveUser(ofstream &file) const {
        file << accountNumber << "," << email << "," << name << "," << phoneNumber << ","
             << dob << "," << fatherName << "," << fatherPhoneNumber << "," << address << ","
             << pinCode << "," << balance << "," << password << endl;
    }
};

class Bank {
private:
    vector<User> users;

public:
    Bank() {
        loadUsersFromFile();
    }

    string generateAccountNumber() {
        return "AC" + to_string(1000 + users.size());
    }

    bool isExistingUser(const string &email, const string &phone) {
        for (const auto &user : users) {
            if (user.getEmail() == email || user.getPhoneNumber() == phone)
                return true;
        }
        return false;
    }

    void addUser() {
        User newUser;
        newUser.createUser();

        if (isExistingUser(newUser.getEmail(), newUser.getPhoneNumber())) {
            cout << "User already exists. Please login.\n";
        }
        else {
            newUser.setAccountNumber(generateAccountNumber());
            users.push_back(newUser);
            cout << "Account created successfully. Account Number: " << newUser.getAccountNumber() << endl;
        }
    }

    void loginUser() {
        string email, accountNumber, password;
        cout << "Enter Email ID: "; cin >> email;
        cout << "Enter Account Number: "; cin >> accountNumber;
        cout << "Enter Password: ";
        char ch;
        while((ch = _getch()) != '\r') {
            password.push_back(ch);
            cout << '*';
        }
        cout << endl;

        for (auto &user : users) {
            if (user.authenticate(email, accountNumber, password)) {
                cout << "Login successful!\n";
                user.displayUser();
                return;
            }
        }
        cout << "Invalid credentials.\n";
    }

    void deposit() {
        string accountNumber;
        double amount;
        cout << "Enter Account Number: "; cin >> accountNumber;
        cout << "Enter Amount to Deposit: "; cin >> amount;

        for (auto &user : users) {
            if (user.getAccountNumber() == accountNumber) {
                user.setBalance(user.getBalance() + amount);
                cout << "Deposit successful. New Balance: " << user.getBalance() << endl;
                return;
            }
        }
        cout << "Account not found.\n";
    }

    void loadUsersFromFile() {
        ifstream file("users.csv");
        if (!file.is_open()) {
            cerr << "Error: Unable to open users.csv file for reading.\n";
            return;
        }

        User user;
        while (user.loadUser(file)) {
            users.push_back(user);
        }
        file.close();
    }

    void saveUsersToFile() {
        ofstream file("users.csv");
        if (!file.is_open()) {
            cerr << "Error: Unable to open users.csv file for writing.\n";
            return;
        }

        for (const auto &user : users) {
            user.saveUser(file);
        }
        file.close();
    }
};

class ATM {
public:
    void atmService() {
        cout << "ATM Service Selected. (Under development)\n";
    }
};

class Menu {
private:
    Bank bank;
    ATM atm;

public:
    void showMenu() {
        int choice;
        while (true) {
            cout << "\n--- Main Menu ---\n";
            cout << "1. Banking Service\n";
            cout << "2. ATM Service\n";
            cout << "3. Exit\n";
            cout << "Select an option: ";
            cin >> choice;

            switch (choice) {
                case 1: bankingServiceMenu(); break;
                case 2: atm.atmService(); break;
                case 3:
                    bank.saveUsersToFile();
                    cout << "Exiting program. Goodbye!\n";
                    return;
                default:
                    cout << "Invalid option. Please try again.\n";
            }
        }
    }

    void bankingServiceMenu() {
        int choice;
        while (true) {
            cout << "\n--- Banking Service ---\n";
            cout << "1. New User\n";
            cout << "2. Already User\n";
            cout << "3. Deposit\n";
            cout << "4. Go Back\n";
            cout << "Select an option: ";
            cin >> choice;

            switch (choice) {
                case 1: bank.addUser(); break;
                case 2: bank.loginUser(); break;
                case 3: bank.deposit(); break;
                case 4: return;
                default: cout << "Invalid option. Please try again.\n";
            }
        }
    }
};
int main() {
    try {
        Menu menu;
        menu.showMenu();
    } catch (const bad_alloc &e) {
        cerr << "Memory allocation failed: " << e.what() << endl;
    }
    return 0;
}
