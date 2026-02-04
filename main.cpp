#include "CLI.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <chrono>

// Forward declarations of demo functions
void runBasicDemo();
void runPerformanceTest();
void runStressTest();

void printUsage(const char* program_name) {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════╗
║     HIGH-PERFORMANCE ORDER BOOK & MATCHING ENGINE            ║
║                    Usage Information                         ║
╚══════════════════════════════════════════════════════════════╝

USAGE:
  )" << program_name << R"( [mode] [options]

MODES:
  (no arguments)         Run interactive CLI mode
  --demo                 Run demonstration scenarios
  --interactive, -i      Run interactive CLI mode (explicit)
  --file <filename>      Execute commands from file
  --benchmark, -b        Run performance benchmarks
  --help, -h             Show this help message

EXAMPLES:
  )" << program_name << R"(                    # Interactive mode
  )" << program_name << R"( --demo             # Run demo scenarios
  )" << program_name << R"( --file orders.txt  # Execute from file
  )" << program_name << R"( --benchmark        # Run benchmarks

INTERACTIVE COMMANDS:
  buy <price> <qty>      Place limit buy order
  sell <price> <qty>     Place limit sell order
  mbuy <qty>             Market buy order
  msell <qty>            Market sell order
  cancel <id>            Cancel order
  print                  Show order book
  stats                  Show statistics
  help                   Show all commands
  exit                   Exit program

For full documentation, see README.md
)" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Parse command-line arguments
        if (argc > 1) {
            std::string mode = argv[1];
            
            if (mode == "--help" || mode == "-h") {
                printUsage(argv[0]);
                return 0;
            }
            else if (mode == "--demo" || mode == "-d") {
                // Run demo mode
                std::cout << R"(
╔══════════════════════════════════════════════════════════════╗
║                                                              ║
║     HIGH-PERFORMANCE ORDER BOOK & MATCHING ENGINE            ║
║                    Demo Mode                                 ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝
)" << std::endl;
                
                runBasicDemo();
                runPerformanceTest();
                runStressTest();
                
                std::cout << "\n✅ Demo completed! Run with --interactive for CLI mode.\n\n";
                return 0;
            }
            else if (mode == "--benchmark" || mode == "-b") {
                // Run benchmarks only
                std::cout << "\n🚀 Running Performance Benchmarks...\n\n";
                runPerformanceTest();
                runStressTest();
                return 0;
            }
            else if (mode == "--file" || mode == "-f") {
                // Execute from file
                if (argc < 3) {
                    std::cerr << "❌ Error: --file requires a filename\n";
                    std::cerr << "   Usage: " << argv[0] << " --file <filename>\n";
                    return 1;
                }
                
                OrderBook book;
                MatchingEngine engine(book);
                CLI cli(engine);
                
                cli.executeFile(argv[2]);
                return 0;
            }
            else if (mode == "--interactive" || mode == "-i") {
                // Explicit interactive mode
                // Fall through to interactive mode below
            }
            else {
                std::cerr << "❌ Unknown option: " << mode << "\n\n";
                printUsage(argv[0]);
                return 1;
            }
        }
        
        // Default: Run interactive CLI mode
        OrderBook book;
        MatchingEngine engine(book);
        CLI cli(engine);
        
        cli.runInteractive();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Fatal error: " << e.what() << "\n";
        return 1;
    }
}

// ============================================================================
// DEMO FUNCTIONS (from original main.cpp)
// ============================================================================

double ticksToDollars(Price ticks, double tick_size = 0.01) {
    return ticks * tick_size;
}

Price dollarsToTicks(double dollars, double tick_size = 0.01) {
    return static_cast<Price>(dollars / tick_size);
}

void onTrade(const Trade& trade) {
    std::cout << "🔥 TRADE EXECUTED\n";
    std::cout << "   Buy Order:  #" << trade.buy_order_id << "\n";
    std::cout << "   Sell Order: #" << trade.sell_order_id << "\n";
    std::cout << "   Price:      $" << std::fixed << std::setprecision(2) 
              << ticksToDollars(trade.price) << "\n";
    std::cout << "   Quantity:   " << trade.quantity << " shares\n";
    std::cout << "   Total:      $" << std::fixed << std::setprecision(2)
              << ticksToDollars(trade.price) * trade.quantity << "\n\n";
}

void runBasicDemo() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  BASIC FUNCTIONALITY DEMO\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    OrderBook book;
    MatchingEngine engine(book);
    engine.setTradeCallback(onTrade);
    
    std::cout << "📊 Scenario 1: Building the Order Book\n";
    std::cout << std::string(60, '-') << "\n";
    
    // Add orders
    std::cout << "Adding orders...\n";
    engine.submitOrder(Side::BUY, OrderType::LIMIT, dollarsToTicks(100.00), 100);
    engine.submitOrder(Side::BUY, OrderType::LIMIT, dollarsToTicks(99.95), 200);
    engine.submitOrder(Side::SELL, OrderType::LIMIT, dollarsToTicks(100.10), 100);
    engine.submitOrder(Side::SELL, OrderType::LIMIT, dollarsToTicks(100.15), 200);
    
    book.printBook(5);
    
    std::cout << "\n📊 Scenario 2: Aggressive Matching\n";
    std::cout << std::string(60, '-') << "\n";
    std::cout << "Submitting aggressive BUY order...\n\n";
    
    engine.submitOrder(Side::BUY, OrderType::LIMIT, dollarsToTicks(100.15), 150);
    
    book.printBook(5);
    
    std::cout << "\n📊 Final Statistics\n";
    std::cout << std::string(60, '-') << "\n";
    std::cout << "Total trades: " << engine.getTotalTradeCount() << "\n";
    std::cout << "Volume traded: " << engine.getTotalVolumeTraded() << " shares\n";
    std::cout << std::string(60, '=') << "\n\n";
}

void runPerformanceTest() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  PERFORMANCE BENCHMARK\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    OrderBook book;
    MatchingEngine engine(book);
    
    const int NUM_ORDERS = 10000;
    
    std::cout << "Inserting " << NUM_ORDERS << " limit orders...\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ORDERS / 2; i++) {
        Price price = dollarsToTicks(100.0 - (i % 100) * 0.01);
        engine.submitOrder(Side::BUY, OrderType::LIMIT, price, 100);
    }
    
    for (int i = 0; i < NUM_ORDERS / 2; i++) {
        Price price = dollarsToTicks(100.0 + (i % 100) * 0.01);
        engine.submitOrder(Side::SELL, OrderType::LIMIT, price, 100);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "✅ Inserted " << NUM_ORDERS << " orders in " 
              << duration.count() / 1000.0 << " ms\n";
    std::cout << "   Average: " << duration.count() / (double)NUM_ORDERS 
              << " μs per order\n\n";
    
    std::cout << std::string(60, '=') << "\n\n";
}

void runStressTest() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  STRESS TEST\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    OrderBook book;
    MatchingEngine engine(book);
    
    const int ITERATIONS = 1000;
    
    std::cout << "Running " << ITERATIONS << " mixed operations...\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < ITERATIONS; i++) {
        engine.submitOrder(Side::BUY, OrderType::LIMIT, 
                          dollarsToTicks(99.0 + (i % 10) * 0.1), 100);
        engine.submitOrder(Side::SELL, OrderType::LIMIT,
                          dollarsToTicks(101.0 + (i % 10) * 0.1), 100);
        
        if (i % 20 == 0) {
            engine.submitOrder(Side::BUY, OrderType::LIMIT, dollarsToTicks(101.5), 50);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "✅ Completed in " << duration.count() << " ms\n";
    std::cout << "   Trades executed: " << engine.getTotalTradeCount() << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}
