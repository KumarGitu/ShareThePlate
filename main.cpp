
#include <iostream>
#include <limits>
#include <string>

#include "Models.h"
#include "FoodManager.h"
#include "Graph.h"

using namespace std;

void donorMenu(FoodManager& manager, int userId) {

    int choice;

    do {

        cout << "\n========== DONOR DASHBOARD ==========\n";
        cout << "1. Add Food Donation\n";
        cout << "2. View Available Food\n";
        cout << "3. View My Donations\n";
        cout << "4. Undo Latest Donation\n";
        cout << "5. View Donation History\n";
        cout << "6. Logout\n";
        cout << "Enter choice: ";

        cin >> choice;

        switch (choice) {

            case 1:
                manager.addDonation(userId);
                break;

            case 2:
                manager.displayAvailableFood();
                break;

            case 3:
                manager.showMyDonations(userId);
                break;

            case 4:
                manager.undoDonation(userId);
                break;

            case 5:
                manager.showHistory();
                break;

            case 6:
                cout << "Logged out successfully!\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }

    } while (choice != 6);
}

void receiverMenu(FoodManager& manager, int userId,
                  Graph& graph) {

    int choice;

    do {

        cout << "\n========== RECEIVER DASHBOARD ==========\n";
        cout << "1. View Available Food\n";
        cout << "2. Claim Food\n";
        cout << "3. Search Food by ID\n";
        cout << "4. Find Shortest Route\n";
        cout << "5. View Donation History\n";
        cout << "6. Logout\n";
        cout << "Enter choice: ";

        cin >> choice;

        switch (choice) {

            case 1:
                manager.displayAvailableFood();
                break;

            case 2:
                manager.claimFood(userId);
                break;

            case 3:
                manager.searchFood();
                break;

            case 4:
                graph.findShortestRoute();
                break;

            case 5:
                manager.showHistory();
                break;

            case 6:
                cout << "Logged out successfully!\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }

    } while (choice != 6);
}

void adminMenu(FoodManager& manager) {

    int choice;

    do {

        cout << "\n========== ADMIN DASHBOARD ==========\n";
        cout << "1. Verify NGOs\n";
        cout << "2. View Available Food\n";
        cout << "3. Search Food by ID\n";
        cout << "4. View Donation History\n";
        cout << "5. Logout\n";
        cout << "Enter choice: ";

        cin >> choice;

        switch (choice) {

            case 1:
                manager.verifyNGOs();
                break;

            case 2:
                manager.displayAvailableFood();
                break;

            case 3:
                manager.searchFood();
                break;

            case 4:
                manager.showHistory();
                break;

            case 5:
                cout << "Admin logged out!\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }

    } while (choice != 5);
}

int main() {

    FoodManager manager;
    Graph graph;

    int choice;

    string adminUsername;
    string adminPassword;

    cout << "\n";
    cout << "============================================\n";
    cout << "              SHARETHEPLATE\n";
    cout << "       FOOD RESCUE AND REDISTRIBUTION\n";
    cout << "============================================\n";

    do {

        cout << "\n============== MAIN MENU ==============\n";
        cout << "1. Register\n";
        cout << "2. Login as Donor\n";
        cout << "3. Login as Receiver / NGO\n";
        cout << "4. Admin Login\n";
        cout << "5. Exit\n";

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {

            case 1:
                manager.registerUser();
                break;

            case 2: {

                int userId = manager.login("Donor");

                if (userId != -1)
                    donorMenu(manager, userId);

                break;
            }

            case 3: {

                int userId = manager.login();

                if (userId != -1) {

                    receiverMenu(manager, userId, graph);
                }

                break;
            }

            case 4:

                cout << "Admin username: ";
                cin >> adminUsername;

                cout << "Admin password: ";
                cin >> adminPassword;

                if (adminUsername == "admin" &&
                    adminPassword == "admin123") {

                    cout << "\nAdmin login successful!\n";
                    adminMenu(manager);

                } else {

                    cout << "\nInvalid admin credentials!\n";
                }

                break;

            case 5:

                cout << "\nThank you for using ShareThePlate!\n";
                cout << "Together, let's reduce food wastage.\n";

                break;

            default:

                cout << "\nInvalid choice! Please try again.\n";
        }

    } while (choice != 5);

    return 0;
}
