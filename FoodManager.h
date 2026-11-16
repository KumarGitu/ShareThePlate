
#ifndef FOODMANAGER_H
#define FOODMANAGER_H

#include "Models.h"

#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <ctime>
#include <cctype>

using namespace std;

class FoodManager {

private:

    vector<User> users;
    vector<FoodItem> foods;

    stack<int> undoStack;
    queue<pair<int, string>> claimQueue;
    list<string> donationHistory;

    unordered_map<int, int> foodIndex;

    int nextUserId = 1;
    int nextFoodId = 1;

    // Binary Search Tree for food ID searching
    struct TreeNode {
        int id;
        TreeNode* left;
        TreeNode* right;

        TreeNode(int value) {
            id = value;
            left = nullptr;
            right = nullptr;
        }
    };

    TreeNode* root = nullptr;

    TreeNode* insertNode(TreeNode* node, int id) {
        if (node == nullptr)
            return new TreeNode(id);

        if (id < node->id)
            node->left = insertNode(node->left, id);
        else if (id > node->id)
            node->right = insertNode(node->right, id);

        return node;
    }

    bool searchNode(TreeNode* node, int id) {
        if (node == nullptr)
            return false;

        if (node->id == id)
            return true;

        if (id < node->id)
            return searchNode(node->left, id);

        return searchNode(node->right, id);
    }

    void deleteTree(TreeNode* node) {
        if (node == nullptr)
            return;

        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

    void rebuildIndex() {
        foodIndex.clear();
        deleteTree(root);
        root = nullptr;

        for (int i = 0; i < static_cast<int>(foods.size()); i++) {
            foodIndex[foods[i].id] = i;
            root = insertNode(root, foods[i].id);
        }
    }

    // Saves user information
    void saveUsers() {
        ofstream file("users.txt");

        for (const auto& user : users) {
            file << user.id << "|"
                 << user.name << "|"
                 << user.phone << "|"
                 << user.password << "|"
                 << user.role << "|"
                 << user.verified << "\n";
        }
    }

    // Saves food records
    void saveFoods() {
        ofstream file("food_records.txt");

        for (const auto& food : foods) {
            file << food.id << "|"
                 << food.donorId << "|"
                 << food.foodName << "|"
                 << food.quantity << "|"
                 << food.location << "|"
                 << food.expiry << "|"
                 << food.price << "|"
                 << food.status << "|"
                 << food.claimedBy << "\n";
        }
    }

    // Saves donation history
    void saveHistory() {
        ofstream file("donation_history.txt");

        for (const auto& entry : donationHistory)
            file << entry << "\n";
    }

    // Checks expiry using YYYY-MM-DD
    bool isExpired(const string& date) {
        tm expiryDate = {};

        istringstream input(date);
        input >> get_time(&expiryDate, "%Y-%m-%d");

        if (input.fail())
            return true;

        expiryDate.tm_hour = 23;
        expiryDate.tm_min = 59;
        expiryDate.tm_sec = 59;

        time_t expiryTime = mktime(&expiryDate);
        time_t currentTime = time(nullptr);

        return difftime(expiryTime, currentTime) < 0;
    }

    void updateExpiredFoods() {
        bool changed = false;

        for (auto& food : foods) {
            if (food.status == "Available" &&
                isExpired(food.expiry)) {

                food.status = "Expired";

                donationHistory.push_back(
                    "Food ID " + to_string(food.id) + " expired"
                );

                changed = true;
            }
        }

        if (changed) {
            saveFoods();
            saveHistory();
        }
    }

    void loadData() {

        // Load users
        ifstream userFile("users.txt");
        string line;

        while (getline(userFile, line)) {
            stringstream ss(line);
            string field;
            User user;

            try {
                getline(ss, field, '|');
                user.id = stoi(field);

                getline(ss, user.name, '|');
                getline(ss, user.phone, '|');
                getline(ss, user.password, '|');
                getline(ss, user.role, '|');

                getline(ss, field, '|');
                user.verified = (field == "1");

                users.push_back(user);

                nextUserId = max(nextUserId, user.id + 1);
            }
            catch (...) {
                continue;
            }
        }

        // Load food records
        ifstream foodFile("food_records.txt");

        while (getline(foodFile, line)) {
            stringstream ss(line);
            string field;
            FoodItem food;

            try {
                getline(ss, field, '|');
                food.id = stoi(field);

                getline(ss, field, '|');
                food.donorId = stoi(field);

                getline(ss, food.foodName, '|');

                getline(ss, field, '|');
                food.quantity = stoi(field);

                getline(ss, food.location, '|');
                getline(ss, food.expiry, '|');

                getline(ss, field, '|');
                food.price = stod(field);

                getline(ss, food.status, '|');
                getline(ss, food.claimedBy);

                foods.push_back(food);

                nextFoodId = max(nextFoodId, food.id + 1);
            }
            catch (...) {
                continue;
            }
        }

        // Load donation history
        ifstream historyFile("donation_history.txt");

        while (getline(historyFile, line))
            donationHistory.push_back(line);

        rebuildIndex();
        updateExpiredFoods();
    }

    int getUserIndex(int id) {
        for (int i = 0; i < static_cast<int>(users.size()); i++) {
            if (users[i].id == id)
                return i;
        }

        return -1;
    }

    bool validDate(const string& date) {
        if (date.size() != 10 || date[4] != '-' || date[7] != '-')
            return false;

        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7)
                continue;

            if (!isdigit(static_cast<unsigned char>(date[i])))
                return false;
        }

        tm value = {};
        istringstream input(date);
        input >> get_time(&value, "%Y-%m-%d");

        if (input.fail())
            return false;

        value.tm_hour = 12;
        time_t converted = mktime(&value);

        if (converted == -1)
            return false;

        tm* check = localtime(&converted);

        if (check == nullptr)
            return false;

        return check->tm_year == value.tm_year &&
               check->tm_mon == value.tm_mon &&
               check->tm_mday == value.tm_mday;
    }

public:

    FoodManager() {
        loadData();
    }

    ~FoodManager() {
        deleteTree(root);
    }

    // Registration
    void registerUser() {

        User user;

        cout << "\n========== REGISTRATION ==========\n";

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter name: ";
        getline(cin, user.name);

        cout << "Enter phone number: ";
        getline(cin, user.phone);

        cout << "Create password: ";
        getline(cin, user.password);

        cout << "\nSelect role:\n";
        cout << "1. Donor\n";
        cout << "2. Individual Receiver\n";
        cout << "3. NGO\n";

        int choice;
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
            user.role = "Donor";
        else if (choice == 2)
            user.role = "Receiver";
        else if (choice == 3)
            user.role = "NGO";
        else {
            cout << "Invalid role!\n";
            return;
        }

        for (const auto& existing : users) {
            if (existing.phone == user.phone) {
                cout << "Phone number already registered!\n";
                return;
            }
        }

        user.id = nextUserId++;

        user.verified = (user.role != "NGO");

        users.push_back(user);

        saveUsers();

        cout << "\nRegistration successful!\n";
        cout << "Your User ID: " << user.id << "\n";

        if (user.role == "NGO")
            cout << "Your NGO account is pending admin verification.\n";
    }

    // Login
    int login(string requiredRole = "") {

        string phone, password;

        cout << "\n========== LOGIN ==========\n";

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter phone number: ";
        getline(cin, phone);

        cout << "Enter password: ";
        getline(cin, password);

        for (int i = 0; i < static_cast<int>(users.size()); i++) {

            if (users[i].phone == phone &&
                users[i].password == password) {

                if (!requiredRole.empty() &&
                    users[i].role != requiredRole) {

                    cout << "Incorrect account type!\n";
                    return -1;
                }

                if (users[i].role == "NGO" &&
                    !users[i].verified) {

                    cout << "Your NGO account is not verified yet.\n";
                    return -1;
                }

                cout << "\nWelcome, " << users[i].name << "!\n";
                return users[i].id;
            }
        }

        cout << "Invalid phone number or password!\n";
        return -1;
    }

    // Admin NGO verification
    void verifyNGOs() {

        cout << "\n========== NGO VERIFICATION ==========\n";

        bool found = false;

        for (auto& user : users) {

            if (user.role == "NGO" && !user.verified) {

                found = true;

                cout << "\nNGO ID: " << user.id;
                cout << "\nName: " << user.name;
                cout << "\nPhone: " << user.phone;

                cout << "\nApprove this NGO? (1 = Yes, 0 = No): ";

                int choice;
                cin >> choice;

                if (choice == 1) {
                    user.verified = true;
                    cout << "NGO verified successfully!\n";
                }
            }
        }

        if (!found)
            cout << "No pending NGO accounts.\n";

        saveUsers();
    }

    // Add food donation
    void addDonation(int donorId) {

        FoodItem food;

        food.id = nextFoodId++;
        food.donorId = donorId;

        cout << "\n========== ADD FOOD DONATION ==========\n";

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter food name: ";
        getline(cin, food.foodName);

        cout << "Enter quantity (packets): ";
        cin >> food.quantity;

        if (food.quantity <= 0) {
            cout << "Quantity must be greater than zero!\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter pickup location: ";
        getline(cin, food.location);

        cout << "Enter expiry date (YYYY-MM-DD): ";
        getline(cin, food.expiry);

        if (!validDate(food.expiry) || isExpired(food.expiry)) {
            cout << "Please enter a valid future expiry date!\n";
            return;
        }

        cout << "Enter price (0 for free): ";
        cin >> food.price;

        if (food.price < 0) {
            cout << "Price cannot be negative!\n";
            return;
        }

        food.status = "Available";
        food.claimedBy = "";

        foods.push_back(food);

        foodIndex[food.id] = static_cast<int>(foods.size()) - 1;
        root = insertNode(root, food.id);

        undoStack.push(food.id);

        donationHistory.push_back(
            "Food donated: " + food.foodName +
            " | Food ID: " + to_string(food.id)
        );

        saveFoods();
        saveHistory();

        cout << "\nFood donation added successfully!\n";
        cout << "Food ID: " << food.id << "\n";
    }

    // Display available food using priority queue
    void displayAvailableFood() {

        updateExpiredFoods();

        priority_queue<
            pair<string, int>,
            vector<pair<string, int>>,
            greater<pair<string, int>>
        > pq;

        for (const auto& food : foods) {
            if (food.status == "Available")
                pq.push({food.expiry, food.id});
        }

        cout << "\n========== AVAILABLE FOOD ==========\n";

        if (pq.empty()) {
            cout << "No available food donations.\n";
            return;
        }

        while (!pq.empty()) {

            int id = pq.top().second;
            pq.pop();

            int index = foodIndex[id];

            const FoodItem& food = foods[index];

            cout << "\nFood ID: " << food.id;
            cout << "\nFood: " << food.foodName;
            cout << "\nQuantity: " << food.quantity;
            cout << "\nLocation: " << food.location;
            cout << "\nExpiry: " << food.expiry;
            cout << "\nPrice: Rs. " << food.price;
            cout << "\nDonor ID: " << food.donorId;
            cout << "\n------------------------------\n";
        }
    }

    // Claim food
    void claimFood(int receiverId) {

        updateExpiredFoods();

        int foodId;

        cout << "\nEnter Food ID to claim: ";
        cin >> foodId;

        if (foodIndex.find(foodId) == foodIndex.end() ||
            !searchNode(root, foodId)) {

            cout << "Food ID not found!\n";
            return;
        }

        int index = foodIndex[foodId];

        if (foods[index].status != "Available") {
            cout << "This food is no longer available!\n";
            return;
        }

        claimQueue.push({foodId, to_string(receiverId)});

        // Process the next request
        auto request = claimQueue.front();
        claimQueue.pop();

        int requestedFoodId = request.first;

        int requestedIndex = foodIndex[requestedFoodId];

        if (foods[requestedIndex].status != "Available") {
            cout << "Food has already been claimed.\n";
            return;
        }

        foods[requestedIndex].status = "Claimed";
        foods[requestedIndex].claimedBy = request.second;

        donationHistory.push_back(
            "Food ID " + to_string(foodId) +
            " claimed by User ID " + request.second
        );

        saveFoods();
        saveHistory();

        cout << "\nFood claimed successfully!\n";
        cout << "Please coordinate pickup with the donor.\n";
    }

    // Undo latest donation
    void undoDonation(int donorId) {

        if (undoStack.empty()) {
            cout << "No recent donation to undo.\n";
            return;
        }

        int foodId = undoStack.top();
        int index = foodIndex[foodId];

        if (foods[index].donorId != donorId ||
            foods[index].status != "Available") {

            cout << "Latest donation cannot be undone.\n";
            return;
        }

        undoStack.pop();

        foods[index].status = "Cancelled";

        donationHistory.push_back(
            "Donation cancelled: Food ID " + to_string(foodId)
        );

        saveFoods();
        saveHistory();

        cout << "Latest donation cancelled successfully!\n";
    }

    // Search food using BST and hash table
    void searchFood() {

        int id;

        cout << "\nEnter Food ID to search: ";
        cin >> id;

        if (!searchNode(root, id) ||
            foodIndex.find(id) == foodIndex.end()) {

            cout << "Food not found!\n";
            return;
        }

        int index = foodIndex[id];

        const FoodItem& food = foods[index];

        cout << "\n========== FOOD DETAILS ==========\n";
        cout << "Food ID: " << food.id << "\n";
        cout << "Name: " << food.foodName << "\n";
        cout << "Quantity: " << food.quantity << "\n";
        cout << "Location: " << food.location << "\n";
        cout << "Expiry: " << food.expiry << "\n";
        cout << "Price: Rs. " << food.price << "\n";
        cout << "Status: " << food.status << "\n";
    }

    // Show donor's donations
    void showMyDonations(int donorId) {

        cout << "\n========== MY DONATIONS ==========\n";

        bool found = false;

        for (const auto& food : foods) {

            if (food.donorId == donorId) {

                found = true;

                cout << "\nFood ID: " << food.id;
                cout << "\nFood: " << food.foodName;
                cout << "\nQuantity: " << food.quantity;
                cout << "\nStatus: " << food.status;
                cout << "\n------------------------------\n";
            }
        }

        if (!found)
            cout << "No donations found.\n";
    }

    // Donation history using linked list
    void showHistory() {

        cout << "\n========== DONATION HISTORY ==========\n";

        if (donationHistory.empty()) {
            cout << "No history available.\n";
            return;
        }

        for (const auto& entry : donationHistory)
            cout << entry << "\n";
    }
};

#endif
