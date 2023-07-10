#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <iomanip>
#include <fmt/format.h>

// Server class
class Server {
public:
    Server(int id, int capability) : id(id), capability(capability), load(0) {}

    void addLoad(int time) {
        loads.push_back(time);
        total_load ++;
    }

    void reset() {
        total_load = 0;
        loads.clear();
    }

    int getLoad() const {
        return int(std::ssize(loads));
    }

    int getId() const {
        return id;
    }

    int getCapability() const {
        return capability;
    }

    int update(int time) {
        while (!loads.empty()) {
            if (loads.front() > capability * time) break;
            loads.erase(loads.begin());
        }
    }

private:
    int id;
    int capability;
    double load;
    int total_load;
    std::vector<int> loads;
};

// Load Balancer class
template <typename LoadBalancingAlgorithm>
class LoadBalancer {
public:
    LoadBalancer(const std::vector<int>& capabilities) {
        int numServers = capabilities.size();
        // Initialize servers
        for (int i = 0; i < numServers; ++i) {
            servers.push_back(Server(i, capabilities[i]));
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

    double getThroughput(double simulatedTime) const {
        double totalCapability = 0.0;
        for (const auto& server : servers) {
            totalCapability += server.getCapability();
        }
        double totalLoad = getTotalLoad();
        return totalLoad / simulatedTime / totalCapability;
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
        const int numIterations = 100;
        for (int iteration = 0; iteration < numIterations; ++iteration) {
            // Move ants
            for (int taskId = 0; taskId < numTasks; ++taskId) {
                int currentServer = selectNextServer(taskId, pheromones, taskLoads, alpha, beta);
                servers[currentServer].addLoad(taskLoads[taskId]);
            }

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

// Helper function to generate random capabilities for servers
std::vector<int> generateRandomCapabilities(int numServers, int minCapability, int maxCapability) {
    std::vector<int> capabilities(numServers);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minCapability, maxCapability);
    for (int i = 0; i < numServers; ++i) {
        capabilities[i] = dis(gen);
    }
    return capabilities;
}

// Helper function to print the table
void printTable(const std::vector<std::vector<double>>& durations, const std::vector<double>& throughputs, const std::vector<int>& numTasks) {
    std::cout << std::setw(20) << "Load Balancing Algorithm";
    for (const auto& numTask : numTasks) {
        std::cout << std::setw(20) << "Num Tasks: " << numTask;
    }
    std::cout << std::setw(20) << "Throughput (Tasks/s)" << std::endl;

    std::cout << std::setw(20) << "";
    for (size_t i = 0; i < numTasks.size(); ++i) {
        std::cout << std::setw(20) << "Execution Time (μs)";
    }
    std::cout << std::setw(20) << "" << std::endl;

    std::vector<std::string> algorithms = {"Random", "Round-Robin", "Weighted Round-Robin", "Active Clustering", "Ant Colony Optimization"};
    for (size_t i = 0; i < durations.size(); ++i) {
        std::cout << std::setw(20) << algorithms[i];
        for (const auto& duration : durations[i]) {
            std::cout << std::setw(20) << duration;
        }
        std::cout << std::setw(20) << throughputs[i] << std::endl;
    }
}

int main() {
    const int NUM_SERVERS = 20;
    const int MIN_CAPABILITY = 1;
    const int MAX_CAPABILITY = 100;
    const std::vector<int> NUM_TASKS = {100, 1000, 10000};

    std::vector<std::vector<double>> durations(5, std::vector<double>(NUM_TASKS.size()));
    std::vector<double> throughputs(5);

    // Generate random capabilities for servers
    std::vector<int> capabilities = generateRandomCapabilities(NUM_SERVERS, MIN_CAPABILITY, MAX_CAPABILITY);

    for (size_t i = 0; i < NUM_TASKS.size(); ++i) {
        std::vector<double> taskLoads = generateRandomTaskLoads(NUM_TASKS[i]);

        LoadBalancer<RandomLoadBalancing> randomBalancer(capabilities);
        randomBalancer.run(taskLoads);
        durations[0][i] = randomBalancer.getTotalLoad();
        throughputs[0] = randomBalancer.getThroughput(NUM_TASKS[i]);

        LoadBalancer<RoundRobinLoadBalancing> roundRobinBalancer(capabilities);
        roundRobinBalancer.run(taskLoads);
        durations[1][i] = roundRobinBalancer.getTotalLoad();
        throughputs[1] = roundRobinBalancer.getThroughput(NUM_TASKS[i]);

        LoadBalancer<WeightedRoundRobinLoadBalancing> weightedRoundRobinBalancer(capabilities);
        weightedRoundRobinBalancer.run(taskLoads);
        durations[2][i] = weightedRoundRobinBalancer.getTotalLoad();
        throughputs[2] = weightedRoundRobinBalancer.getThroughput(NUM_TASKS[i]);

        LoadBalancer<ActiveClusteringLoadBalancing> activeClusteringBalancer(capabilities);
        activeClusteringBalancer.run(taskLoads);
        durations[3][i] = activeClusteringBalancer.getTotalLoad();
        throughputs[3] = activeClusteringBalancer.getThroughput(NUM_TASKS[i]);

        LoadBalancer<AntColonyOptimizationLoadBalancing> antColonyOptimizationBalancer(capabilities);
        antColonyOptimizationBalancer.run(taskLoads);
        durations[4][i] = antColonyOptimizationBalancer.getTotalLoad();
        throughputs[4] = antColonyOptimizationBalancer.getThroughput(NUM_TASKS[i]);
    }

    printTable(durations, throughputs, NUM_TASKS);

    return 0;
}
