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

    int getCapability() const {
        return capability;
    }

private:
    int id;
    int capability;
    double load;
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
        // TODO: Implement Ant Colony Optimization algorithm for load balancing
        // This is just a placeholder
        for (const auto& taskLoad : taskLoads) {
            int randomServer = getRandomServer();
            servers[randomServer].addLoad(taskLoad);
        }
    }

private:
    std::vector<Server> servers;

    int getRandomServer() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, servers.size() - 1);
        return dis(gen);
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
    const int NUM_TASKS = 10000;

    std::vector<int> numTasks = {NUM_TASKS};

    std::vector<std::vector<double>> durations(5, std::vector<double>(numTasks.size()));
    std::vector<double> throughputs(5);

    // Generate random capabilities for servers
    std::vector<int> capabilities = generateRandomCapabilities(NUM_SERVERS, MIN_CAPABILITY, MAX_CAPABILITY);

    for (size_t i = 0; i < numTasks.size(); ++i) {
        std::vector<double> taskLoads = generateRandomTaskLoads(numTasks[i]);

        LoadBalancer<RandomLoadBalancing> randomBalancer(capabilities);
        double simulatedTime = 0.0;
        for (const auto& taskLoad : taskLoads) {
            randomBalancer.run({taskLoad});
            simulatedTime += 1.0; // Increment simulated time by 1 second
        }
        durations[0][i] = simulatedTime * 1e6; // Convert to microseconds
        throughputs[0] = randomBalancer.getThroughput(simulatedTime);

        LoadBalancer<RoundRobinLoadBalancing> roundRobinBalancer(capabilities);
        simulatedTime = 0.0;
        for (const auto& taskLoad : taskLoads) {
            roundRobinBalancer.run({taskLoad});
            simulatedTime += 1.0; // Increment simulated time by 1 second
        }
        durations[1][i] = simulatedTime * 1e6; // Convert to microseconds
        throughputs[1] = roundRobinBalancer.getThroughput(simulatedTime);

        LoadBalancer<WeightedRoundRobinLoadBalancing> weightedRoundRobinBalancer(capabilities);
        simulatedTime = 0.0;
        for (const auto& taskLoad : taskLoads) {
            weightedRoundRobinBalancer.run({taskLoad});
            simulatedTime += 1.0; // Increment simulated time by 1 second
        }
        durations[2][i] = simulatedTime * 1e6; // Convert to microseconds
        throughputs[2] = weightedRoundRobinBalancer.getThroughput(simulatedTime);

        LoadBalancer<ActiveClusteringLoadBalancing> activeClusteringBalancer(capabilities);
        simulatedTime = 0.0;
        for (const auto& taskLoad : taskLoads) {
            activeClusteringBalancer.run({taskLoad});
            simulatedTime += 1.0; // Increment simulated time by 1 second
        }
        durations[3][i] = simulatedTime * 1e6; // Convert to microseconds
        throughputs[3] = activeClusteringBalancer.getThroughput(simulatedTime);

        LoadBalancer<AntColonyOptimizationLoadBalancing> antColonyOptimizationBalancer(capabilities);
        simulatedTime = 0.0;
        for (const auto& taskLoad : taskLoads) {
            antColonyOptimizationBalancer.run({taskLoad});
            simulatedTime += 1.0; // Increment simulated time by 1 second
        }
        durations[4][i] = simulatedTime * 1e6; // Convert to microseconds
        throughputs[4] = antColonyOptimizationBalancer.getThroughput(simulatedTime);
    }

    printTable(durations, throughputs, numTasks);

    return 0;
}
