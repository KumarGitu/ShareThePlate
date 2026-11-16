
#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <climits>
#include <algorithm>

using namespace std;

class Graph {

private:

    static const int V = 4;

    string locations[V] = {
        "Hostel",
        "Campus",
        "Hotel",
        "NGO"
    };

    vector<pair<int, int>> adj[V];

    int findLocation(string name) {

        for (int i = 0; i < V; i++) {
            if (locations[i] == name)
                return i;
        }

        return -1;
    }

    void addEdge(int u, int v, int distance) {
        adj[u].push_back({v, distance});
        adj[v].push_back({u, distance});
    }

public:

    Graph() {

        // Sample road distances in kilometres
        addEdge(0, 1, 2);
        addEdge(1, 2, 5);
        addEdge(2, 3, 3);
        addEdge(0, 3, 12);
        addEdge(1, 3, 7);
    }

    void findShortestRoute() {

        string source, destination;

        cout << "\n========== ROUTE FINDER ==========\n";

        cout << "\nAvailable locations:\n";

        for (int i = 0; i < V; i++)
            cout << locations[i] << "\n";

        cout << "\nEnter starting location: ";
        cin >> source;

        cout << "Enter destination: ";
        cin >> destination;

        int start = findLocation(source);
        int end = findLocation(destination);

        if (start == -1 || end == -1) {
            cout << "Location not found!\n";
            cout << "Use exact spelling and capitalization.\n";
            return;
        }

        vector<int> distance(V, INT_MAX);
        vector<int> parent(V, -1);

        priority_queue<
            pair<int, int>,
            vector<pair<int, int>>,
            greater<pair<int, int>>
        > pq;

        distance[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {

            int currentDistance = pq.top().first;
            int u = pq.top().second;

            pq.pop();

            if (currentDistance > distance[u])
                continue;

            for (auto edge : adj[u]) {

                int v = edge.first;
                int weight = edge.second;

                if (distance[u] != INT_MAX &&
                    distance[u] + weight < distance[v]) {

                    distance[v] = distance[u] + weight;
                    parent[v] = u;

                    pq.push({distance[v], v});
                }
            }
        }

        if (distance[end] == INT_MAX) {
            cout << "No route available.\n";
            return;
        }

        vector<int> path;

        for (int current = end; current != -1;
             current = parent[current]) {

            path.push_back(current);
        }

        reverse(path.begin(), path.end());

        cout << "\nShortest route:\n";

        for (int i = 0; i < static_cast<int>(path.size()); i++) {

            cout << locations[path[i]];

            if (i != static_cast<int>(path.size()) - 1)
                cout << " -> ";
        }

        cout << "\nTotal distance: "
             << distance[end] << " km\n";
    }
};

#endif
