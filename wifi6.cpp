#include <iostream>
#include <vector>
#include <queue>
#include <numeric>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <random>

const int PACKET_SIZE_BITS = 8192;    // 1 KB = 8,192 bits
const double CODING_RATE = 5.0 / 6.0; // Coding rate (rate for 256-QAM)
const int MOD_BITS = 8;               // 256-QAM => 8 bits per symbol
const double FRAME_DURATION = 0.001;  // 1 ms per frame (more realistic for WiFi 6)
const double TOTAL_BANDWIDTH = 20e6;  // Total bandwidth 20 MHz
const double TARGET_THROUGHPUT = 100e6; // Target throughput per user (100 Mbps)
const int NUM_RU_TYPES = 9;           // Number of Resource Unit (RU) types in WiFi 6

// RU (Resource Unit) configuration for WiFi 6
struct ResourceUnit {
    int size;           // RU size in number of subcarriers
    double bandwidth;   // Bandwidth of the RU
    int max_users;      // Maximum number of users this RU can support

    ResourceUnit(int subcarriers, double bw, int max_users_count)
        : size(subcarriers), bandwidth(bw), max_users(max_users_count) {}
};

// DataPacket class
class DataPacket {
public:
    int id;
    int user_id;
    double creation_time;
    double transmission_time;
    double queuing_time;
    double total_latency;

    DataPacket(int packet_id, int user, double creation)
        : id(packet_id), user_id(user), creation_time(creation), 
          transmission_time(0), queuing_time(0), total_latency(0) {}
};

// UserDevice class
class UserDevice {
public:
    int id;
    std::queue<DataPacket> user_packet_queue;

    UserDevice(int user_id) : id(user_id) {}

    void generatePacket(int packet_id, double current_time) {
        user_packet_queue.push(DataPacket(packet_id, id, current_time));
    }
};

// WiFi6Simulator class to simulate WiFi 6 with OFDMA
class WiFi6Simulator {
private:
    std::vector<UserDevice> users;
    std::vector<ResourceUnit> resource_units;
    double simulation_duration;
    std::vector<DataPacket> transmitted_packets;
    std::mt19937 random_generator;

    // RU configurations based on WiFi 6 standard
    const std::vector<ResourceUnit> RU_CONFIGURATIONS = {
        {26, 2e6, 1},   // RU26: 2 MHz, 1 user
        {52, 4e6, 2},   // RU52: 4 MHz, 2 users
        {106, 8e6, 4},  // RU106: 8 MHz, 4 users
        {242, 16e6, 9}, // RU242: 16 MHz, 9 users
        {484, 20e6, 18} // RU484: 20 MHz, 18 users
    };

public:
    WiFi6Simulator(double duration, int num_users)
        : simulation_duration(duration), random_generator(std::random_device{}()) {
        // Initialize users
        for (int i = 1; i <= num_users; ++i) {
            users.emplace_back(i);
        }

        // Dynamically allocate Resource Units based on number of users
        allocateResourceUnits(num_users);
    }

    void runSimulation() {
        double current_time = 0.0;
        int packet_counter = 1;

        // Generate packets for all users
        std::uniform_real_distribution<> packet_gen_dist(0.8, 1.2);

        while (current_time < simulation_duration) {
            // Generate packets for each user
            for (auto& user : users) {
                // Probabilistic packet generation with slight randomness
                if (random_generator() % 2 == 0) {
                    user.generatePacket(packet_counter++, current_time);
                }
            }

            // Process packets using Resource Units
            processPacketsWithResourceUnits(current_time);

            // Move to next frame
            current_time += FRAME_DURATION;
        }

        displayResults();
    }

private:
    void allocateResourceUnits(int num_users) {
        // Choose appropriate RU configurations based on number of users
        resource_units.clear();
        
        if (num_users <= 1) {
            // Single user: use full bandwidth
            resource_units.push_back(ResourceUnit(484, TOTAL_BANDWIDTH, 1));
        } else {
            // Multi-user allocation strategy
            for (auto it = RU_CONFIGURATIONS.rbegin(); it != RU_CONFIGURATIONS.rend(); ++it) {
                if (it->max_users >= num_users) {
                    resource_units.push_back(*it);
                    break;
                }
            }

            // If no single RU fits, use multiple smaller RUs
            if (resource_units.empty()) {
                // Default to RU26 and RU52 combination
                resource_units.push_back(ResourceUnit(26, 2e6, 1));
                resource_units.push_back(ResourceUnit(52, 4e6, 2));
            }
        }
    }

    void processPacketsWithResourceUnits(double current_time) {
        for (const auto& ru : resource_units) {
            std::vector<UserDevice*> active_users;

            // Collect users with packets
            for (auto& user : users) {
                if (!user.user_packet_queue.empty() && active_users.size() < ru.max_users) {
                    active_users.push_back(&user);
                }
            }

            // Process packets for this RU
            if (!active_users.empty()) {
                double ru_data_rate = ru.bandwidth * MOD_BITS * CODING_RATE;

                for (auto* user : active_users) {
                    if (!user->user_packet_queue.empty()) {
                        DataPacket& packet = user->user_packet_queue.front();

                        // Calculate transmission time
                        double transmission_time = static_cast<double>(PACKET_SIZE_BITS) / ru_data_rate;

                        // Update packet metadata
                        packet.transmission_time = transmission_time;
                        packet.queuing_time = current_time - packet.creation_time;
                        packet.total_latency = packet.transmission_time + packet.queuing_time;

                        // Record transmitted packet
                        transmitted_packets.push_back(packet);

                        // Remove packet from user's queue
                        user->user_packet_queue.pop();
                    }
                }
            }
        }
    }

    void displayResults() {
        if (transmitted_packets.empty()) {
            std::cout << "No packets transmitted.\n";
            return;
        }

        // Throughput calculation
        double total_throughput = (transmitted_packets.size() * PACKET_SIZE_BITS) / (simulation_duration * 1e6);

        // Latency calculations
        std::vector<double> latencies;
        for (const auto& packet : transmitted_packets) {
            latencies.push_back(packet.total_latency * 1000.0); // Convert to ms
        }

        double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        double max_latency = *std::max_element(latencies.begin(), latencies.end());

        // Per-user statistics
        std::unordered_map<int, int> user_packet_counts;
        for (const auto& packet : transmitted_packets) {
            user_packet_counts[packet.user_id]++;
        }

        std::cout << "\nSimulation Results for WiFi 6 with OFDMA:\n";
        std::cout << "Total Users: " << users.size() << "\n";
        std::cout << "Resource Units Used: " << resource_units.size() << "\n";
        std::cout << "Total Throughput: " << std::fixed << std::setprecision(2) << total_throughput << " Mbps\n";
        std::cout << "Total Packets Transmitted: " << transmitted_packets.size() << "\n";
        std::cout << "Average Packet Latency: " << std::fixed << std::setprecision(4) << avg_latency << " ms\n";
        std::cout << "Maximum Packet Latency: " << std::fixed << std::setprecision(4) << max_latency << " ms\n";

        // User-wise packet distribution
        std::cout << "\nPacket Distribution per User:\n";
        for (const auto& [user_id, packet_count] : user_packet_counts) {
            std::cout << "User " << user_id << ": " << packet_count << " packets\n";
        }
    }
};

int main() {
    int num_users;
    double simulation_duration;

    std::cout << "Enter number of users: ";
    std::cin >> num_users;

    std::cout << "Enter simulation duration (in seconds): ";
    std::cin >> simulation_duration;

    if (num_users <= 0 || simulation_duration <= 0) {
        std::cerr << "Invalid input. Users and duration must be positive.\n";
        return 1;
    }

    WiFi6Simulator simulator(simulation_duration, num_users);
    simulator.runSimulation();

    return 0;
}
