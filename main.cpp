#include <iostream>
#include <vector>
#include <algorithm>
#include <limits> // Include the header file for numeric_limits
#include <cstdlib>
#include <ctime>

using namespace std;

// Structure to represent a solution
struct Solution {
    char id;
    double cost;
    double feedback;
    double crowdingDistance;

    // Equality operator for Solution struct
    bool operator==(const Solution& other) const {
        return id == other.id && cost == other.cost && feedback == other.feedback;
    }
};

// Generate a random solution
Solution generateRandomSolution() {
    char id = 'A' + rand() % 26; // Random ID from 'A' to 'Z'
    double cost = rand() % 100;
    double feedback = rand() % 100;
    return {id, cost, feedback, 0.0};
}

// Perform crossover between two parent solutions
Solution crossover(const Solution& parent1, const Solution& parent2) {
    double alpha = (double)rand() / RAND_MAX; // Random weight
    double cost = alpha * parent1.cost + (1 - alpha) * parent2.cost;
    double feedback = alpha * parent1.feedback + (1 - alpha) * parent2.feedback;
    return {'X', cost, feedback, 0.0}; // Assign a unique ID
}

// Perform mutation on a solution
void mutate(Solution& solution, double mutationRate) {
    if ((double)rand() / RAND_MAX < mutationRate) {
        solution.cost += ((double)rand() / RAND_MAX - 0.5) * 0.1; // Mutation within [-0.05, 0.05]
        solution.feedback += ((double)rand() / RAND_MAX - 0.5) * 0.1;
    }
}

void calculateCrowdingDistance(vector<Solution>& solutions, double maxCost, double minCost, double maxFeedback, double minFeedback) {
    int n = solutions.size();
    for (int i = 0; i < n; ++i) {
        solutions[i].crowdingDistance = 0.0;
    }

    for (int obj = 0; obj < 2; ++obj) {
        // Sort solutions aesc based on cost for obj = 0 and feedback for obj = 1
        sort(solutions.begin(), solutions.end(), [obj](const Solution& a, const Solution& b) {
            return (obj == 0) ? (a.cost < b.cost) : (a.feedback < b.feedback);
        });

        // Set crowding distance for extreme solutions
        solutions[0].crowdingDistance = solutions[n - 1].crowdingDistance = numeric_limits<double>::infinity();

        double range = (obj == 0) ? maxCost - minCost : maxFeedback - minFeedback;
        if (range == 0) continue; // Avoid division by zero

        cout<<range;

        for (int i = 1; i < n - 1; ++i) {
            double objValue = (obj == 0) ? solutions[i+1].cost : solutions[i+1].feedback;
            solutions[i].crowdingDistance += (objValue - (obj == 0 ? solutions[i - 1].cost : solutions[i - 1].feedback)) / range;
        }
    }
}


// Perform non-dominated sorting
vector<vector<Solution>> nonDominatedSorting(const vector<Solution>& population) {
    vector<Solution> remainingPopulation = population;
    vector<vector<Solution>> fronts;

    while (!remainingPopulation.empty()) {
        vector<Solution> currentFront;
        for (const auto& sol : remainingPopulation) {
            bool isDominated = false;
            for (const auto& other : remainingPopulation) {
                if (sol.cost > other.cost && sol.feedback > other.feedback) {
                    isDominated = true;
                    break;
                }
            }
            if (!isDominated) {
                currentFront.push_back(sol);
            }
        }
        for (const auto& sol : currentFront) {
            auto it = find(remainingPopulation.begin(), remainingPopulation.end(), sol);
            if (it != remainingPopulation.end()) {
                remainingPopulation.erase(it);
            }
        }
        fronts.push_back(currentFront);
    }
    return fronts;
}

int main() {
    // Initialize the population with specific solutions
    vector<Solution> population = {
      // ID, Cost, Feedback, Crowding Distance
        {'A', 20, 2.2, 0.0},
        {'B', 60, 4.4, 0.0},
        {'C', 65, 3.5, 0.0},
        {'D', 15, 4.4, 0.0},
        {'E', 55, 4.5, 0.0},
        {'F', 50, 1.8, 0.0},
        {'G', 80, 4.0, 0.0},
        {'H', 25, 4.6, 0.0}
    };

    double minCost = numeric_limits<double>::max();
    double maxCost = numeric_limits<double>::min();
    double minFeedback = numeric_limits<double>::max();
    double maxFeedback = numeric_limits<double>::min();

    for (int i = 0; i < population.size(); ++i) {
      minCost = min(minCost, population[i].cost);
      maxCost = max(maxCost, population[i].cost);
      minFeedback = min(minFeedback, population[i].feedback);
      maxFeedback = max(maxFeedback, population[i].feedback);
    }

    // Perform non-dominated sorting
    vector<vector<Solution>> fronts = nonDominatedSorting(population);

    // Perform crowding distance calculation
    for (auto& front : fronts) {
        calculateCrowdingDistance(front, maxCost, minCost, maxFeedback, minFeedback);
    }

    // Shortlist top 4 parents based on crowding distance
    vector<Solution> topParents;
    for (auto& front : fronts) {
        sort(front.begin(), front.end(), [](const Solution& a, const Solution& b) {
            return a.crowdingDistance > b.crowdingDistance;
        });

        for (size_t i = 0; i < front.size() && topParents.size() < 4; ++i) {
            topParents.push_back(front[i]);
        }
    }

    // Output all fronts
    for (size_t i = 0; i < fronts.size(); ++i) {
        cout << "Front " << i << ":" << endl;
        for (const auto& sol : fronts[i]) {
            cout << "ID: " << sol.id << ", Cost: " << sol.cost << ", Feedback: " << sol.feedback << ", Crowding Distance: " << sol.crowdingDistance << endl;
        }
        cout << endl;
    }

    // Output the top 4 parents
    cout << "Top 4 Parents:" << endl;
    for (const auto& parent : topParents) {
        cout << "ID: " << parent.id << ", Cost: " << parent.cost << ", Feedback: " << parent.feedback << ", Crowding Distance: " << parent.crowdingDistance << endl;
    }

    return 0;
}
