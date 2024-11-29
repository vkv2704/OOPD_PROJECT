# WiFi Simulation Project (WiFi 4, WiFi 5, WiFi 6)

## Overview

This project simulates WiFi networks using OFDMA (Orthogonal Frequency Division Multiple Access) for WiFi 6, and compares the throughput and latency across different WiFi standards: WiFi 4, WiFi 5, and WiFi 6. It is designed to model the performance characteristics of each standard under different network conditions.

The simulation calculates:
- **Throughput**: The total amount of data transmitted over the network.
- **Latency**: The time it takes for a packet to travel from source to destination.
- **Packet Transmission**: The total number of packets transmitted in a given time.

## Table of Contents

- [WiFi 4 Simulation](#wifi-4-simulation)
- [WiFi 5 Simulation](#wifi-5-simulation)
- [WiFi 6 Simulation](#wifi-6-simulation)
- [How to Run the Simulation](#how-to-run-the-simulation)
- [Dependencies](#dependencies)
- [Results and Observations](#results-and-observations)
- [Acknowledgments](#acknowledgments)

## WiFi 4 Simulation

WiFi 4 (IEEE 802.11n) utilizes **OFDM** (Orthogonal Frequency Division Multiplexing) for channel access. The simulation models the following characteristics of WiFi 4:
- Maximum throughput per user: 72 Mbps (based on 20 MHz channels and MIMO).
- Basic data rates for packet transmission.
- Simple packet transmission logic without OFDMA.

### Key Features:
- Simulates WiFi 4's channel utilization with basic OFDM.
- Does not use OFDMA or MU-MIMO, making it less efficient than later WiFi generations.

## WiFi 5 Simulation

WiFi 5 (IEEE 802.11ac) introduces improvements over WiFi 4, including **beamforming** and **MU-MIMO** (Multi-User MIMO). The simulation for WiFi 5 models these features:
- Maximum throughput per user: Up to 1 Gbps (with wider channels and 256-QAM).
- Supports beamforming for better signal quality and range.

### Key Features:
- Simulates MU-MIMO (multiple users served simultaneously).
- Models 80 MHz and 160 MHz channels, with faster data rates compared to WiFi 4.

## WiFi 6 Simulation

WiFi 6 (IEEE 802.11ax) is the latest standard that incorporates **OFDMA** (Orthogonal Frequency Division Multiple Access), **MU-MIMO**, **1024-QAM**, and **BSS Coloring** for enhanced efficiency in dense environments. This simulation focuses on:
- Fixed bandwidth of 20 MHz, divided into smaller 2 MHz sub-channels for efficient OFDMA.
- Support for multiple users, each getting its own sub-channel in a time slot.

### Key Features:
- Uses OFDMA for better channel utilization and lower latency.
- Models the behavior of WiFi 6 under different user loads and packet transmission scenarios.
- Supports a wide range of throughput rates based on user count and network conditions.

## How to Run the Simulation

### Prerequisites:
- A C++ compiler (e.g., GCC or Clang).
- Basic knowledge of C++ for compiling and running the code.

### Steps:
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-repository/wifi-simulation.git
   cd wifi-simulation
Compile the Code: For Linux/macOS:

makefile

For Windows: Use a C++ IDE (e.g., Visual Studio) or MinGW to compile the code.

Run the Simulation:


./wifi4
./wifi5
./wifi6


##Acknowledgments
This simulation project was developed using concepts from IEEE WiFi standards and the following tools and libraries:

C++ Standard Library (STL)
ChatGPT (OpenAI) for code assistance and suggestions
