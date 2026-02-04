# High-Performance Limit Order Book & Matching Engine

A production-quality C++ implementation of a limit order book and matching engine, designed for low latency, deterministic behavior, and high throughput.

## 🎯 Overview

This project implements a high-performance matching engine that simulates how real exchanges match buy and sell orders. It demonstrates advanced systems programming concepts including:

- **Low-latency data structures** optimized for cache performance
- **Price-time priority** matching algorithm
- **Partial fills** and order lifecycle management
- **O(1) order cancellation** using hash map lookups
- **Deterministic execution** with single-threaded core
- **Integer-based pricing** to avoid floating-point precision issues

## 🚀 Features

### Core Functionality
- ✅ **Limit Orders**: Orders that rest in the book at specific prices
- ✅ **Market Orders**: Orders that execute immediately at best available prices
- ✅ **Price-Time Priority**: Orders matched by price first, then arrival time (FIFO)
- ✅ **Partial Fills**: Orders can be partially executed across multiple trades
- ✅ **Order Cancellation**: Fast O(1) cancellation using hash map
- ✅ **Order Modification**: Modify quantity of resting orders
- ✅ **Best Bid/Ask Tracking**: Real-time top-of-book access
- ✅ **Trade Notifications**: Callback system for trade execution events

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

## 📁 Project Structure

```
StockMarket/
├── Order.h              # Order and Trade data structures
├── OrderBook.h          # Order book interface
├── OrderBook.cpp        # Order book implementation
├── MatchingEngine.h     # Matching engine interface
├── MatchingEngine.cpp   # Matching engine implementation
├── CLI.h                # Command-line interface
├── CLI.cpp              # CLI implementation
├── main.cpp             # Main program with multiple modes
├── CMakeLists.txt       # CMake build configuration
├── Makefile             # Alternative Make build system
├── README.md            # This file
├── CLI_GUIDE.md         # Complete CLI documentation
├── INTERVIEW_GUIDE.md   # Interview preparation guide
├── QUICKSTART.md        # Quick start guide
└── examples/            # Example scenario files
    ├── basic_orders.txt
    ├── matching_demo.txt
    ├── market_orders.txt
    ├── order_management.txt
    └── scenario_intraday.txt
```

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

### Using CMake (Recommended for Production)

```bash
# Create build directory
mkdir build && cd build

# Configure (Release build)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build .

# Run
./matching_engine
```

### Build Configurations

**Release Mode** (Production):
- `-O3`: Maximum optimization
- `-march=native`: CPU-specific optimizations
- `-flto`: Link-time optimization
- `-ffast-math`: Fast floating-point operations
- `-funroll-loops`: Loop unrolling

**Debug Mode** (Development):
- `-g`: Debug symbols
- `-O0`: No optimization
- `-Wall -Wextra -Wpedantic`: All warnings enabled

## 🎮 Running the Program

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

See [CLI_GUIDE.md](CLI_GUIDE.md) for complete CLI documentation.

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

### Sample Output

```
╔══════════════════════════════════════╗
║          ORDER BOOK SNAPSHOT         ║
╠══════════════════════════════════════╣
║ Spread:                           10 ║
╠══════════════════════════════════════╣
║              ASKS (SELL)             ║
║    Price    │   Quantity  │  Orders ║
╟────────────┼─────────────┼──────────╢
║      10025 │         300 │        1 ║
║      10020 │         150 │        1 ║
║      10015 │         200 │        1 ║
║      10010 │         100 │        1 ║
╠══════════════════════════════════════╣
║              BIDS (BUY)              ║
║    Price    │   Quantity  │  Orders ║
╟────────────┼─────────────┼──────────╢
║      10000 │         100 │        1 ║
║       9995 │         200 │        1 ║
║       9990 │         150 │        1 ║
║       9985 │         300 │        1 ║
╚══════════════════════════════════════╝

🔥 TRADE EXECUTED
   Buy Order:  #15
   Sell Order: #5
   Price:      $100.10
   Quantity:   100 shares
   Total:      $10010.00
```

## 🎓 Interview-Ready Talking Points

### "How does your order book work?"

> "I implemented a high-performance limit order book using two separate `std::map` containers—one for buy orders in descending price order, and one for sell orders in ascending order. This design gives O(1) access to the best bid and ask prices, which are critical for matching.
>
> Within each price level, I use a `std::deque` to maintain FIFO ordering for time priority. For O(1) cancellation, I maintain a hash map from order ID to an iterator pointing directly to the order's position in the book."

### "What optimizations did you implement?"

> "Several key optimizations:
> 
> 1. **Integer prices** instead of floating-point to avoid precision issues and ensure deterministic comparisons
> 2. **Move semantics** throughout to avoid unnecessary copying of order objects
> 3. **Minimal heap allocations** during matching by carefully managing iterators
> 4. **Separate buy/sell maps** for cleaner logic and better cache locality
> 5. **Hash map for cancellations** to achieve O(1) lookup instead of scanning the book"

### "How do you handle partial fills?"

> "When matching orders, I calculate the minimum quantity between the aggressive and passive orders. Both orders are filled by that amount, and their remaining quantities are updated. If an order is completely filled, it's removed from the book; if partially filled, it stays with reduced quantity.
>
> The challenge is maintaining iterator validity—I'm careful to erase from the deque only after I'm done with the current level, and I use the front() element before popping to avoid iterator invalidation issues."

### "Could this be made concurrent?"

> "The core matching engine is single-threaded by design for deterministic execution—same inputs always produce the same trades in the same order.
>
> Concurrency can be added around the edges: market data ingestion, order validation, risk checks, and trade reporting can all run in separate threads using lock-free queues to communicate with the matching core. This is similar to how real exchange matching engines work—deterministic core with concurrent I/O layers."

### "How would you reduce latency further?"

> "Several approaches:
> 
> 1. **Custom memory allocators** (memory pools) to avoid heap allocation overhead
> 2. **Flat arrays instead of trees** for price levels if price range is bounded
> 3. **Lock-free queues** for inter-thread communication
> 4. **NUMA-aware allocation** on multi-socket systems
> 5. **CPU pinning** for the matching thread
> 6. **Kernel bypass networking** (like DPDK) for order ingestion"

## 🔬 Technical Deep Dives

### Why Two Maps?

If we used a single map for both buy and sell orders:
- We'd need to scan to find both best bid (highest buy) and best ask (lowest sell)
- Custom comparators become complex and error-prone
- Matching logic is harder to reason about

Two maps provide:
- O(1) access to top of each side
- Clean separation reflecting supply/demand
- Simpler, more maintainable matching logic

### Integer Prices vs Floating Point

**Problem with floats:**
```cpp
// Floating point issues
100.1 + 0.2 != 100.3  // Precision error
Ordering can be non-deterministic
```

**Solution with integers:**
```cpp
// $100.25 with tick size $0.01
Price price = 10025;  // Store as integer ticks
// Exact comparisons, deterministic ordering ✅
```

### Matching Algorithm

```cpp
// Match a BUY order against SELL side
while (!buy_order->isFilled() && !sell_book.empty()) {
    auto& [best_ask, sell_queue] = *sell_book.begin();
    
    // Check if prices cross
    if (buy_order->getPrice() < best_ask) break;
    
    // Match FIFO within price level
    while (!buy_order->isFilled() && !sell_queue.empty()) {
        auto sell_order = sell_queue.front();
        Quantity qty = min(buy_qty, sell_qty);
        
        executeTrade(buy_order, sell_order, best_ask, qty);
        
        if (sell_order->isFilled()) {
            sell_queue.pop_front();  // Remove filled order
        }
    }
    
    // Clean up empty price level
    if (sell_queue.empty()) {
        sell_book.erase(sell_book.begin());
    }
}
```

## 📊 Performance Benchmarks

Typical results on modern hardware (Intel i7/i9):

```
Inserting 10,000 orders:  ~8-12 ms
  Average: ~1 μs per order

Matching 5,000 shares:    ~50-100 μs
  Trades executed: 50-100

Stress test (1,000 iterations):
  Total operations: 4,000
  Throughput: ~400,000 ops/sec
```

## 🎯 Use Cases

- **Quantitative Finance**: Understanding exchange mechanics
- **Algorithmic Trading**: Backtesting and strategy development
- **Education**: Learning low-latency C++ and data structures
- **Interviews**: Demonstrating systems programming skills

## 🔮 Future Enhancements

Possible extensions to make this even more realistic:

- [ ] **Iceberg orders** (hidden quantity)
- [ ] **Stop orders** (triggered by price movements)
- [ ] **Fill-or-Kill (FOK)** and **Immediate-or-Cancel (IOC)** order types
- [ ] **Self-trade prevention** (same participant on both sides)
- [ ] **Market-by-price (MBP)** and **Market-by-order (MBO)** data feeds
- [ ] **Multi-threaded order ingestion** with lock-free queues
- [ ] **Persistence** (save/restore book state)
- [ ] **Network interface** (FIX protocol)
- [ ] **Risk checks** (position limits, credit checks)

## 📚 Learning Resources

To understand this project deeply:

1. **Data Structures**: Maps, deques, hash tables, iterators
2. **C++ Concepts**: Move semantics, smart pointers, templates
3. **Trading Systems**: Order types, matching algorithms, market microstructure
4. **Performance**: Cache optimization, memory allocation, algorithmic complexity

## 🤝 Contributing

This is a learning/interview project. Feel free to:
- Experiment with optimizations
- Add new order types
- Implement additional features
- Profile and benchmark

## 📝 License

MIT License - Free to use for learning, interviews, and personal projects.

## ✨ Author

Built as a demonstration of production-quality C++ systems programming for quantitative finance roles.

---

**Interview Tip**: When discussing this project, focus on:
1. **Why** you made design decisions (not just what you did)
2. **Tradeoffs** between different approaches
3. **Edge cases** you handled
4. **Performance** characteristics and optimizations
5. **Real-world** similarities to actual exchange systems
