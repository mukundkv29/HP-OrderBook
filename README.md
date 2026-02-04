# High-Performance Limit Order Book & Matching Engine

A production-quality C++ implementation of a limit order book and matching engine, designed for low latency, deterministic behavior, and high throughput.

## Overview

This project implements a high-performance matching engine that simulates how real exchanges match buy and sell orders. It demonstrates advanced systems programming concepts including:

- **Low-latency data structures** optimized for cache performance
- **Price-time priority** matching algorithm
- **Partial fills** and order lifecycle management
- **O(1) order cancellation** using hash map lookups
- **Deterministic execution** with single-threaded core
- **Integer-based pricing** to avoid floating-point precision issues

## Features

### Core Functionality
-  **Limit Orders**: Orders that rest in the book at specific prices
-  **Market Orders**: Orders that execute immediately at best available prices
-  **Price-Time Priority**: Orders matched by price first, then arrival time (FIFO)
-  **Partial Fills**: Orders can be partially executed across multiple trades
-  **Order Cancellation**: Fast O(1) cancellation using hash map
-  **Order Modification**: Modify quantity of resting orders
-  **Best Bid/Ask Tracking**: Real-time top-of-book access
-  **Trade Notifications**: Callback system for trade execution events

### Technical Highlights

#### Data Structures
```cpp
// Two separate maps for optimal ordering
std::map<Price, std::deque<Order>, std::greater<Price>> buyBook;  // Descending
std::map<Price, std::deque<Order>, std::less<Price>> sellBook;    // Ascending

// O(1) order lookup for cancellation
std::unordered_map<OrderId, OrderLocation> orderLocations;
```

#### Performance Characteristics
| Operation | Time Complexity | Notes |
|-----------|----------------|-------|
| Insert Order | O(log N) | N = number of price levels |
| Cancel Order | O(1) | Hash map lookup |
| Match Order | O(M) | M = number of fills |
| Best Bid/Ask | O(1) | Map begin() |
| Modify Order | O(1) | Direct access via hash map |

## 🛠️ Building the Project

### Using Make (Recommended for Quick Start)

```bash
# Build in release mode (optimized)
make

# Build in debug mode
make debug

# Build and run
make run

# Clean build artifacts
make clean
```

##  Running the Program

The matching engine supports **multiple modes of operation**:

### Interactive Mode (Default)

```bash
./matching_engine
```

Type commands in real-time to interact with the order book.

### Demo Mode

```bash
./matching_engine --demo
```

Runs automated demonstration scenarios.

### Batch Mode

```bash
./matching_engine --file examples/basic_orders.txt
```

Execute commands from a file for scripted scenarios.

### Benchmark Mode

```bash
./matching_engine --benchmark
```

Run performance tests only.

### Interactive Commands

When running in interactive mode, you can use these commands:

```bash
buy 100.50 200         # Place limit buy order
sell 101.25 150        # Place limit sell order
mbuy 100               # Market buy order
msell 75               # Market sell order
cancel 5               # Cancel order #5
modify 3 500           # Modify order quantity
print                  # Show order book
stats                  # Show statistics
list                   # List active orders
help                   # Show all commands
exit                   # Exit program
```

### Demo Scenarios (Demo Mode)

1. **Building the Order Book**: Add limit orders to both sides
2. **Price-Time Priority**: Multiple orders at same price level
3. **Aggressive Matching**: Order sweeps multiple price levels
4. **Order Cancellation**: O(1) cancellation demonstration
5. **Market Orders**: Price-agnostic immediate execution
6. **Edge Cases**: Large spreads, partial fills, empty book
7. **Performance Benchmark**: 10,000 orders insertion test
8. **Stress Test**: High-frequency operations