
#ifndef MODELS_H
#define MODELS_H

#include <iostream>
#include <string>
using namespace std;

// Stores information about every registered user
struct User {
    int id;
    string name;
    string phone;
    string password;
    string role;
    bool verified;
};

// Stores information about donated food
struct FoodItem {
    int id;
    int donorId;
    string foodName;
    int quantity;
    string location;
    string expiry;
    double price;
    string status;
    string claimedBy;
};

#endif
