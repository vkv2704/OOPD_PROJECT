#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>
#include <iomanip>

// WiFi 4 Constants
const double BANDWIDTH = 20e6;        // 20 MHz
const int SPATIAL_STREAMS = 1;        // Single spatial stream
const int MOD_BITS = 8;               // 256-QAM => 8 bits per symbol
const double CODING_RATE = 5.0 / 6.0; // Coding rate
const int PACKET_SIZE_BITS = 8192;    // 1 KB = 8,192 bits
const int NUM_FREQUENCY_CHANNELS = 20; // 20 frequency channels (4x5 as per requirement)

// Packet class to represent a transmitted packet
class DataPacket {
public:
    int id;
    int size_bits;
    double creation_time;
    double transmission_time;
    double backoff_time;
    double total_latency;

    DataPacket(int packet_id, int size, double creation)
        : id(packet_id), size_bits(size), creation_time(creation),
          transmission_time(0), backoff_time(0), total_latency(0) {}
};

// UserDevice class to simulate each user transmitting data
class UserDevice {
public:
    int id;
    double last_transmission_time;

    UserDevice(int user_id) : id(user_id), last_transmission_time(0) {}

    DataPacket sendPacket(double current_time, int packet_id, double data_rate,
                          double max_backoff, int num_users) {
        std::random_device rd;
        std::mt19937 gen(rd());

        // Adjust backoff distribution based on number of users
        double backoff_factor = 1.0 + std::min(num_users, 100) * 0.01;
        std::uniform_real_distribution<> backoff_dist(0, max_backoff * backoff_factor);

        double backoff_time = backoff_dist(gen);

        DataPacket packet(packet_id, PACKET_SIZE_BITS, current_time);
        packet.backoff_time = backoff_time;

        // Transmission time calculation
        packet.transmission_time = static_cast<double>(packet.size_bits) / data_rate;

        // Adjust total latency with contention factor
        double contention_factor = 1.0 + std::log(1.0 + num_users) / 2.0;
        packet.total_latency = backoff_time + packet.transmission_time * contention_factor;

        last_transmission_time = current_time + packet.total_latency;
        return packet;
    }
};

// AccessPoint class to represent the central controller of the WiFi system
class AccessPoint {
public:
    double frequency_channel;

    AccessPoint(double frequency) : frequency_channel(frequency) {}

    bool isChannelBusy(const std::vector<UserDevice>& users, double current_time) {
        for (const auto& user : users) {
            if (user.last_transmission_time > current_time) {
                return true;
            }
        }
        return false;
    }
};

// WiFiSimulator class to simulate the WiFi environment and user interactions
class WiFiSimulator {
private:
    AccessPoint access_point;
    double data_rate;
    double simulation_duration;
    double max_backoff_time;
    std::vector<UserDevice> users;
    std::vector<DataPacket> transmitted_packets;
    std::priority_queue<DataPacket, std::vector<DataPacket>, 
                        bool(*)(const DataPacket&, const DataPacket&)> packet_queue;

    static bool packetComparator(const DataPacket& p1, const DataPacket& p2) {
        return p1.backoff_time > p2.backoff_time;
    }

public:
    WiFiSimulator(double frequency, double duration, int num_users, double backoff = 0.02e-3)
        : access_point(frequency), simulation_duration(duration),
          max_backoff_time(backoff),
          packet_queue(packetComparator) {
        data_rate = SPATIAL_STREAMS * MOD_BITS * CODING_RATE * BANDWIDTH;

        for (int i = 1; i <= num_users; ++i) {
            users.emplace_back(i);
        }
    }

    void runSimulation() {
        double current_time = 0.0;
        int packet_counter = 1;

        while (current_time < simulation_duration) {
            for (auto& user : users) {
                if (!access_point.isChannelBusy(users, current_time)) {
                    DataPacket packet = user.sendPacket(current_time, packet_counter++,
                                                        data_rate, max_backoff_time, users.size());
                    packet_queue.push(packet);
                    transmitted_packets.push_back(packet);
                    current_time += packet.total_latency;
                }
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

        double actual_throughput = (transmitted_packets.size() * PACKET_SIZE_BITS) / 
                                   (simulation_duration * 1e6);

        std::vector<double> latencies, backoff_times;
        for (const auto& packet : transmitted_packets) {
            latencies.push_back(packet.total_latency * 1000.0);
            backoff_times.push_back(packet.backoff_time * 1000.0);
        }

        double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        double max_latency = *std::max_element(latencies.begin(), latencies.end());

        std::cout << "\nSimulation Results for WiFi 4 :\n";
        std::cout << "Total Users: " << users.size() << "\n";
        std::cout << "Throughput: " << std::fixed << std::setprecision(2) 
                  << actual_throughput << " Mbps\n";
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

