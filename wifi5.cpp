#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <queue>
#include <cmath>

// WiFi 5 Constants
const double BANDWIDTH = 20e6;        // 20 MHz
const int SPATIAL_STREAMS = 8;        // Up to 8 spatial streams in WiFi 5
const int MOD_BITS = 8;               // 256-QAM => 8 bits per symbol
const double CODING_RATE = 5.0 / 6.0; // Coding rate
const int PACKET_SIZE_BITS = 8192;    // 1 KB = 8,192 bits

// Packet class to represent a transmitted packet
class DataPacket {
public:
    int id;
    int size_bits;
    double creation_time;
    double transmission_time;
    double queuing_time;
    double total_latency;

    DataPacket(int packet_id, int size, double creation)
        : id(packet_id), size_bits(size), creation_time(creation),
          transmission_time(0), queuing_time(0), total_latency(0) {}
};

// UserDevice class to simulate each user transmitting data
class UserDevice {
public:
    int id;

    UserDevice(int user_id) : id(user_id) {}

    DataPacket sendPacket(double current_time, int packet_id, double data_rate) {
        DataPacket packet(packet_id, PACKET_SIZE_BITS, current_time);

        // Transmission time calculation
        packet.transmission_time = static_cast<double>(packet.size_bits) / data_rate;

        return packet;
    }
};

// WiFiSimulator class to simulate the WiFi environment
class WiFiSimulator {
private:
    double data_rate;
    double simulation_duration;
    std::vector<UserDevice> users;
    std::queue<DataPacket> packet_queue; // Queue for contention
    std::vector<DataPacket> transmitted_packets;

public:
    WiFiSimulator(double frequency, double duration, int num_users)
        : simulation_duration(duration) {
        // Calculate data rate for WiFi 5 with MU-MIMO support
        data_rate = SPATIAL_STREAMS * MOD_BITS * CODING_RATE * BANDWIDTH;

        for (int i = 1; i <= num_users; ++i) {
            users.emplace_back(i);
        }
    }

    void runSimulation() {
        double current_time = 0.0;
        int packet_counter = 1;

        while (current_time < simulation_duration) {
            // Generate packets for each user
            for (auto& user : users) {
                DataPacket packet = user.sendPacket(current_time, packet_counter++, data_rate);
                packet_queue.push(packet);
            }

            // Process packets from the queue
            while (!packet_queue.empty() && current_time < simulation_duration) {
                DataPacket& packet = packet_queue.front();

                // Queuing time is the time spent waiting in the queue
                packet.queuing_time = std::max(0.0, current_time - packet.creation_time);

                // Total latency = transmission time + queuing time
                packet.total_latency = packet.transmission_time + packet.queuing_time;

                // Transmit packet
                transmitted_packets.push_back(packet);

                // Advance current time by transmission time
                current_time += packet.transmission_time;

                // Remove packet from the queue
                packet_queue.pop();
            }

            // If channel is busy, advance time slightly to simulate contention
            if (!packet_queue.empty()) {
                current_time += 0.001; // Increment time to simulate contention effects
            }
        }

        displayResults();
    }

private:
    void displayResults() {
        if (transmitted_packets.empty()) {
            std::cout << "No packets transmitted.\n";
            return;
        }

        // Throughput considering contention
        double contention_factor = 1.0 + std::log(users.size());
        double throughput = (transmitted_packets.size() * PACKET_SIZE_BITS) /
                                      (simulation_duration * 1e6 * contention_factor); // Mbps

        std::vector<double> latencies;
        for (const auto& packet : transmitted_packets) {
            latencies.push_back(packet.total_latency * 1000.0); // Convert to ms
        }

        double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        double max_latency = *std::max_element(latencies.begin(), latencies.end());

        std::cout << "\nSimulation Results for WiFi 5:\n";
        std::cout << "Total Users: " << users.size() << "\n";
        std::cout << " Throughput: " << std::fixed << std::setprecision(2)
                  << throughput << " Mbps\n";
        std::cout << "Total Packets Transmitted: " << transmitted_packets.size() << "\n";
        std::cout << "Average Packet Latency: " << std::fixed << std::setprecision(4)
                  << avg_latency << " ms\n";
        std::cout << "Maximum Packet Latency: " << std::fixed << std::setprecision(4)
                  << max_latency << " ms\n";
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

    WiFiSimulator simulator(2.4e9, simulation_duration, num_users);
    simulator.runSimulation();

    return 0;
}

