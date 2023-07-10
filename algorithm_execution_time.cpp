#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <fmt/format.h>

// Server class
class Server {
public:
    Server(int id) : id(id), load(0) {}

    void addLoad(double taskLoad) {
        load += taskLoad;
    }

    void resetLoad() {
        load = 0;
    }

    double getLoad() const {
        return load;
    }

    int getId() const {
        return id;
    }

private:
    int id;
    double load;
};

// Load Balancer class
template <typename LoadBalancingAlgorithm>
class LoadBalancer {
public:
    LoadBalancer(int numServers) {
        // Initialize servers
        for (int i = 0; i < numServers; ++i) {
            servers.push_back(Server(i));
        }
    }

    void run(const std::vector<double>& taskLoads) {
        LoadBalancingAlgorithm algorithm(servers);
        algorithm.balanceLoad(taskLoads);
    }

    double getTotalLoad() const {
        double totalLoad = 0.0;
        for (const auto& server : servers) {
            totalLoad += server.getLoad();
        }
        return totalLoad;
    }

private:
    std::vector<Server> servers;
};

// Random algorithm
class RandomLoadBalancing {
public:
    RandomLoadBalancing(const std::vector<Server>& servers) : servers(servers) {}

    void balanceLoad(const std::vector<double>& taskLoads) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, servers.size() - 1);

        for (const auto& taskLoad : taskLoads) {
            int randomServer = dis(gen);
            servers[randomServer].addLoad(taskLoad);
        }
    }

private:
    std::vector<Server> servers;
};

// Round-Robin algorithm
class RoundRobinLoadBalancing {
public:
    RoundRobinLoadBalancing(const std::vector<Server>& servers) : servers(servers), currentServer(0) {}

    void balanceLoad(const std::vector<double>& taskLoads) {
        for (const auto& taskLoad : taskLoads) {
            servers[currentServer].addLoad(taskLoad);
            currentServer = (currentServer + 1) % servers.size();
        }
    }

private:
    std::vector<Server> servers;
    int currentServer;
};

// Weighted Round-Robin algorithm
class WeightedRoundRobinLoadBalancing {
public:
    WeightedRoundRobinLoadBalancing(const std::vector<Server>& servers) : servers(servers), currentServer(0) {}

    void balanceLoad(const std::vector<double>& taskLoads) {
        for (const auto& taskLoad : taskLoads) {
            servers[currentServer].addLoad(taskLoad);
            updateCurrentServer();
        }
    }

private:
    std::vector<Server> servers;
    int currentServer;

    void updateCurrentServer() {
        double minLoad = servers[0].getLoad();
        int minLoadServer = servers[0].getId();
        for (const auto& server : servers) {
            if (server.getLoad() < minLoad) {
                minLoad = server.getLoad();
                minLoadServer = server.getId();
            }
        }
        currentServer = minLoadServer;
    }
};

// Active Clustering algorithm
class ActiveClusteringLoadBalancing {
public:
    ActiveClusteringLoadBalancing(const std::vector<Server>& servers) : servers(servers) {}

    void balanceLoad(const std::vector<double>& taskLoads) {
        for (const auto& taskLoad : taskLoads) {
            // Find the server with the minimum load
            double minLoad = servers[0].getLoad();
            int minLoadServer = servers[0].getId();
            for (const auto& server : servers) {
                if (server.getLoad() < minLoad) {
                    minLoad = server.getLoad();
                    minLoadServer = server.getId();
                }
            }

            // Assign the task to the server with the minimum load
            servers[minLoadServer].addLoad(taskLoad);
        }
    }

private:
    std::vector<Server> servers;
};

// Ant Colony Optimization algorithm
class AntColonyOptimizationLoadBalancing {
public:
    AntColonyOptimizationLoadBalancing(const std::vector<Server>& servers) : servers(servers) {}

    void balanceLoad(const std::vector<double>& taskLoads) {
        int numTasks = taskLoads.size();
        int numServers = servers.size();

        // Ant Colony Optimization parameters
        const double alpha = 1.0; // Pheromone importance factor
        const double beta = 2.0;  // Heuristic information importance factor
        const double rho = 0.5;   // Pheromone evaporation rate
        const double Q = 1.0;     // Pheromone deposit quantity

        // Initialize pheromone trails
        std::vector<std::vector<double>> pheromones(numTasks, std::vector<double>(numServers, 1.0));

        // Perform Ant Colony Optimization
        const int numIterations = 1;
        for (int iteration = 0; iteration < numIterations; ++iteration) {
            // Move ants
//            for (int taskId = 0; taskId < numTasks; ++taskId) {
//                int currentServer = selectNextServer(taskId, pheromones, taskLoads, alpha, beta);
//                servers[currentServer].addLoad(taskLoads[taskId]);
//            }

            // Update pheromones
            updatePheromones(pheromones, taskLoads, rho, Q);

            // Reset server loads
            for (auto& server : servers) {
                server.resetLoad();
            }
        }
    }

private:
    std::vector<Server> servers;

    int selectNextServer(int taskId, const std::vector<std::vector<double>>& pheromones,
                         const std::vector<double>& taskLoads, double alpha, double beta) {
        int numServers = servers.size();

        // Calculate selection probabilities
        std::vector<double> probabilities(numServers, 0.0);
        double totalProbability = 0.0;
        for (int serverId = 0; serverId < numServers; ++serverId) {
            double pheromone = pheromones[taskId][serverId];
            double heuristic = 1.0 / (std::pow(servers[serverId].getLoad() + taskLoads[taskId], beta));
            probabilities[serverId] = std::pow(pheromone, alpha) * heuristic;
            totalProbability += probabilities[serverId];
        }

        // Roulette wheel selection
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, totalProbability);
        double selection = dis(gen);
        double cumulativeProbability = 0.0;
        for (int serverId = 0; serverId < numServers; ++serverId) {
            cumulativeProbability += probabilities[serverId];
            if (cumulativeProbability >= selection) {
                return serverId;
            }
        }

        // If no server is selected, return the last server
        return numServers - 1;
    }

    void updatePheromones(std::vector<std::vector<double>>& pheromones,
                          const std::vector<double>& taskLoads, double rho, double Q) {
        int numTasks = taskLoads.size();
        int numServers = servers.size();

        // Evaporate pheromones
        for (int taskId = 0; taskId < numTasks; ++taskId) {
            for (int serverId = 0; serverId < numServers; ++serverId) {
                pheromones[taskId][serverId] *= (1.0 - rho);
            }
        }

        // Deposit pheromones based on server loads
        for (int taskId = 0; taskId < numTasks; ++taskId) {
            for (int serverId = 0; serverId < numServers; ++serverId) {
                double deltaPheromone = Q / (servers[serverId].getLoad() + taskLoads[taskId]);
                pheromones[taskId][serverId] += deltaPheromone;
            }
        }
    }
};

// Helper function to generate random task loads
std::vector<double> generateRandomTaskLoads(int numTasks) {
    std::vector<double> taskLoads(numTasks);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 10.0);
    for (int i = 0; i < numTasks; ++i) {
        taskLoads[i] = dis(gen);
    }
    return taskLoads;
}

// Helper function to print the table
void printTable(const std::vector<std::vector<double>>& durations, const std::vector<int>& numTasks) {
    std::cout << std::setw(20) << "Load Balancing Algorithm";
    for (const auto& numTask : numTasks) {
        std::cout << std::setw(20) << "Num Tasks: " << numTask;
    }
    std::cout << std::endl;

    std::cout << std::setw(20) << "";
    for (size_t i = 0; i < numTasks.size(); ++i) {
        std::cout << std::setw(20) << "Execution Time (μs)";
    }
    std::cout << std::endl;

    std::vector<std::string> algorithms = {"Random", "Round-Robin", "Weighted Round-Robin", "Active Clustering", "Ant Colony Optimization"};
    for (size_t i = 0; i < durations.size(); ++i) {
        std::cout << std::setw(20) << algorithms[i];
        for (const auto& duration : durations[i]) {
            std::cout << std::setw(20) << duration;
        }
        std::cout << std::endl;
    }
}

int main() {
    const int NUM_SERVERS = 20;
    std::vector<int> numTasks = {100, 1000, 10000};

    std::vector<std::vector<double>> durations(5, std::vector<double>(numTasks.size()));

    for (size_t i = 0; i < numTasks.size(); ++i) {
        std::vector<double>taskLoads = generateRandomTaskLoads(numTasks[i]);

        LoadBalancer<RandomLoadBalancing> randomBalancer(NUM_SERVERS);
        auto startTime = std::chrono::steady_clock::now();
        randomBalancer.run(taskLoads);
        auto endTime = std::chrono::steady_clock::now();
        auto randomDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        durations[0][i] = randomDuration;

        LoadBalancer<RoundRobinLoadBalancing> roundRobinBalancer(NUM_SERVERS);
        startTime = std::chrono::steady_clock::now();
        roundRobinBalancer.run(taskLoads);
        endTime = std::chrono::steady_clock::now();
        auto roundRobinDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        durations[1][i] = roundRobinDuration;

        LoadBalancer<WeightedRoundRobinLoadBalancing> weightedRoundRobinBalancer(NUM_SERVERS);
        startTime = std::chrono::steady_clock::now();
        weightedRoundRobinBalancer.run(taskLoads);
        endTime = std::chrono::steady_clock::now();
        auto weightedRoundRobinDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        durations[2][i] = weightedRoundRobinDuration;

        LoadBalancer<ActiveClusteringLoadBalancing> activeClusteringBalancer(NUM_SERVERS);
        startTime = std::chrono::steady_clock::now();
        activeClusteringBalancer.run(taskLoads);
        endTime = std::chrono::steady_clock::now();
        auto activeClusteringDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        durations[3][i] = activeClusteringDuration;

        LoadBalancer<AntColonyOptimizationLoadBalancing> antColonyOptimizationBalancer(NUM_SERVERS);
        startTime = std::chrono::steady_clock::now();
        antColonyOptimizationBalancer.run(taskLoads);
        endTime = std::chrono::steady_clock::now();
        auto antColonyOptimizationDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        durations[4][i] = antColonyOptimizationDuration;
    }

    printTable(durations, numTasks);

    return 0;
}
