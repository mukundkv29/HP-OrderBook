#include "CLI.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cctype>

void CLI::setupTradeCallback() {
    engine_.setTradeCallback([](const Trade& trade) {
        std::cout << "\n🔥 TRADE EXECUTED\n";
        std::cout << "   Buy Order:  #" << trade.buy_order_id << "\n";
        std::cout << "   Sell Order: #" << trade.sell_order_id << "\n";
        std::cout << "   Price:      $" << std::fixed << std::setprecision(2) 
                  << trade.price * 0.01 << "\n";
        std::cout << "   Quantity:   " << trade.quantity << " shares\n";
        std::cout << "   Value:      $" << std::fixed << std::setprecision(2)
                  << (trade.price * trade.quantity * 0.01) << "\n";
    });
}

void CLI::printWelcome() const {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════╗
║                                                              ║
║        INTERACTIVE ORDER BOOK & MATCHING ENGINE              ║
║                    Command-Line Interface                    ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝

Welcome! You can now interact with the matching engine in real-time.

Type 'help' for available commands, or 'exit' to quit.

)" << std::endl;
}

void CLI::printHelp() const {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════╗
║                        COMMANDS                              ║
╠══════════════════════════════════════════════════════════════╣
║                                                              ║
║  ORDER COMMANDS:                                             ║
║  ───────────────                                             ║
║  buy <price> <quantity>      Place a limit buy order        ║
║  sell <price> <quantity>     Place a limit sell order       ║
║  mbuy <quantity>             Place a market buy order       ║
║  msell <quantity>            Place a market sell order      ║
║                                                              ║
║  Examples:                                                   ║
║    buy 100.50 200       # Buy 200 shares at $100.50         ║
║    sell 101.25 150      # Sell 150 shares at $101.25        ║
║    mbuy 100             # Market buy 100 shares             ║
║                                                              ║
║  ORDER MANAGEMENT:                                           ║
║  ──────────────────                                          ║
║  cancel <order_id>           Cancel an order                ║
║  modify <order_id> <qty>     Modify order quantity          ║
║  list                        List active orders             ║
║                                                              ║
║  VIEW COMMANDS:                                              ║
║  ──────────────                                              ║
║  print [levels]              Show order book (default: 5)   ║
║  stats                       Show statistics                ║
║  clear                       Clear screen                   ║
║                                                              ║
║  SYSTEM:                                                     ║
║  ────────                                                    ║
║  help                        Show this help message         ║
║  exit                        Exit the program               ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void CLI::printPrompt() const {
    std::cout << "\n> ";
    std::cout.flush();
}

std::vector<std::string> CLI::parseCommand(const std::string& command) {
    std::vector<std::string> tokens;
    std::istringstream iss(command);
    std::string token;
    
    while (iss >> token) {
        // Convert to lowercase for case-insensitive commands
        std::transform(token.begin(), token.end(), token.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        tokens.push_back(token);
    }
    
    return tokens;
}

double CLI::parsePrice(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        throw std::runtime_error("Invalid price format");
    }
}

Quantity CLI::parseQuantity(const std::string& str) {
    try {
        return std::stoull(str);
    } catch (...) {
        throw std::runtime_error("Invalid quantity format");
    }
}

Price CLI::priceToTicks(double price) {
    return static_cast<Price>(price * 100); // Assuming $0.01 tick size
}

double CLI::ticksToPrice(Price ticks) {
    return ticks * 0.01;
}

void CLI::handleBuy(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cout << "❌ Usage: buy <price> <quantity>\n";
        std::cout << "   Example: buy 100.50 200\n";
        return;
    }
    
    try {
        double price = parsePrice(args[1]);
        Quantity quantity = parseQuantity(args[2]);
        
        OrderId order_id = engine_.submitOrder(
            Side::BUY, 
            OrderType::LIMIT, 
            priceToTicks(price), 
            quantity
        );
        
        std::ostringstream desc;
        desc << "BUY " << quantity << " @ $" << std::fixed << std::setprecision(2) << price;
        order_descriptions_[order_id] = desc.str();
        
        std::cout << "✅ Order #" << order_id << " submitted: " << desc.str() << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << "\n";
    }
}

void CLI::handleSell(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cout << "❌ Usage: sell <price> <quantity>\n";
        std::cout << "   Example: sell 101.25 150\n";
        return;
    }
    
    try {
        double price = parsePrice(args[1]);
        Quantity quantity = parseQuantity(args[2]);
        
        OrderId order_id = engine_.submitOrder(
            Side::SELL, 
            OrderType::LIMIT, 
            priceToTicks(price), 
            quantity
        );
        
        std::ostringstream desc;
        desc << "SELL " << quantity << " @ $" << std::fixed << std::setprecision(2) << price;
        order_descriptions_[order_id] = desc.str();
        
        std::cout << "✅ Order #" << order_id << " submitted: " << desc.str() << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << "\n";
    }
}

void CLI::handleMarketBuy(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "❌ Usage: mbuy <quantity>\n";
        std::cout << "   Example: mbuy 100\n";
        return;
    }
    
    try {
        Quantity quantity = parseQuantity(args[1]);
        
        OrderId order_id = engine_.submitOrder(
            Side::BUY, 
            OrderType::MARKET, 
            0, // Price doesn't matter for market orders
            quantity
        );
        
        std::cout << "✅ Market BUY order #" << order_id << " executed for " << quantity << " shares\n";
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << "\n";
    }
}

void CLI::handleMarketSell(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "❌ Usage: msell <quantity>\n";
        std::cout << "   Example: msell 100\n";
        return;
    }
    
    try {
        Quantity quantity = parseQuantity(args[1]);
        
        OrderId order_id = engine_.submitOrder(
            Side::SELL, 
            OrderType::MARKET, 
            0, // Price doesn't matter for market orders
            quantity
        );
        
        std::cout << "✅ Market SELL order #" << order_id << " executed for " << quantity << " shares\n";
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << "\n";
    }
}

void CLI::handleCancel(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "❌ Usage: cancel <order_id>\n";
        std::cout << "   Example: cancel 5\n";
        return;
    }
    
    try {
        OrderId order_id = std::stoull(args[1]);
        
        if (engine_.cancelOrder(order_id)) {
            std::cout << "✅ Order #" << order_id << " cancelled successfully\n";
            order_descriptions_.erase(order_id);
        } else {
            std::cout << "❌ Order #" << order_id << " not found or already filled\n";
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << "\n";
    }
}

void CLI::handleModify(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cout << "❌ Usage: modify <order_id> <new_quantity>\n";
        std::cout << "   Example: modify 5 300\n";
        return;
    }
    
    try {
        OrderId order_id = std::stoull(args[1]);
        Quantity new_quantity = parseQuantity(args[2]);
        
        if (engine_.modifyOrder(order_id, new_quantity)) {
            std::cout << "✅ Order #" << order_id << " modified to " << new_quantity << " shares\n";
        } else {
            std::cout << "❌ Cannot modify order #" << order_id 
                     << " (may be partially filled or not found)\n";
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ Error: " << e.what() << "\n";
    }
}

void CLI::handlePrint(const std::vector<std::string>& args) {
    int levels = 5; // Default
    
    if (args.size() > 1) {
        try {
            levels = std::stoi(args[1]);
        } catch (...) {
            std::cout << "❌ Invalid number of levels, using default (5)\n";
        }
    }
    
    engine_.getOrderBook().printBook(levels);
}

void CLI::handleStats() {
    const auto& book = engine_.getOrderBook();
    
    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║            MARKET STATISTICS             ║\n";
    std::cout << "╠══════════════════════════════════════════╣\n";
    
    auto best_bid = book.getBestBid();
    auto best_ask = book.getBestAsk();
    auto spread = book.getSpread();
    
    if (best_bid.has_value()) {
        std::cout << "║ Best Bid:      $" << std::setw(22) << std::fixed << std::setprecision(2)
                  << ticksToPrice(best_bid.value()) << " ║\n";
    } else {
        std::cout << "║ Best Bid:       " << std::setw(22) << "N/A" << " ║\n";
    }
    
    if (best_ask.has_value()) {
        std::cout << "║ Best Ask:      $" << std::setw(22) << std::fixed << std::setprecision(2)
                  << ticksToPrice(best_ask.value()) << " ║\n";
    } else {
        std::cout << "║ Best Ask:       " << std::setw(22) << "N/A" << " ║\n";
    }
    
    if (spread.has_value()) {
        std::cout << "║ Spread:        $" << std::setw(22) << std::fixed << std::setprecision(2)
                  << ticksToPrice(spread.value()) << " ║\n";
    } else {
        std::cout << "║ Spread:         " << std::setw(22) << "N/A" << " ║\n";
    }
    
    std::cout << "╟──────────────────────────────────────────╢\n";
    std::cout << "║ Buy Volume:    " << std::setw(23) << book.getTotalVolume(Side::BUY) << " ║\n";
    std::cout << "║ Sell Volume:   " << std::setw(23) << book.getTotalVolume(Side::SELL) << " ║\n";
    std::cout << "╟──────────────────────────────────────────╢\n";
    std::cout << "║ Total Trades:  " << std::setw(23) << engine_.getTotalTradeCount() << " ║\n";
    std::cout << "║ Volume Traded: " << std::setw(23) << engine_.getTotalVolumeTraded() << " ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n";
}

void CLI::handleClear() {
    // Clear screen (works on Unix-like systems and Windows)
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    printWelcome();
}

void CLI::handleList() {
    const auto& book = engine_.getOrderBook();
    
    std::cout << "\n╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    ACTIVE ORDERS                         ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Order ID  │  Description                               ║\n";
    std::cout << "╟────────────┼────────────────────────────────────────────╢\n";
    
    if (order_descriptions_.empty()) {
        std::cout << "║            │  No active orders                         ║\n";
    } else {
        for (const auto& [order_id, desc] : order_descriptions_) {
            if (book.hasOrder(order_id)) {
                std::cout << "║ " << std::setw(10) << order_id << " │  " 
                         << std::left << std::setw(42) << desc << std::right << " ║\n";
            }
        }
    }
    
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
}

void CLI::handleExit() {
    std::cout << "\n👋 Thank you for using the Matching Engine!\n";
    std::cout << "Final statistics:\n";
    handleStats();
    running_ = false;
}

bool CLI::executeCommand(const std::string& command) {
    if (command.empty() || command[0] == '#') {
        return true; // Skip empty lines and comments
    }
    
    auto tokens = parseCommand(command);
    if (tokens.empty()) {
        return true;
    }
    
    const std::string& cmd = tokens[0];
    
    try {
        if (cmd == "buy") {
            handleBuy(tokens);
        } else if (cmd == "sell") {
            handleSell(tokens);
        } else if (cmd == "mbuy") {
            handleMarketBuy(tokens);
        } else if (cmd == "msell") {
            handleMarketSell(tokens);
        } else if (cmd == "cancel") {
            handleCancel(tokens);
        } else if (cmd == "modify") {
            handleModify(tokens);
        } else if (cmd == "print" || cmd == "p") {
            handlePrint(tokens);
        } else if (cmd == "stats" || cmd == "s") {
            handleStats();
        } else if (cmd == "clear" || cmd == "cls") {
            handleClear();
        } else if (cmd == "list" || cmd == "l") {
            handleList();
        } else if (cmd == "help" || cmd == "h" || cmd == "?") {
            printHelp();
        } else if (cmd == "exit" || cmd == "quit" || cmd == "q") {
            handleExit();
            return false;
        } else {
            std::cout << "❌ Unknown command: " << cmd << "\n";
            std::cout << "   Type 'help' for available commands\n";
        }
    } catch (const std::exception& e) {
        std::cout << "❌ Error executing command: " << e.what() << "\n";
    }
    
    return running_;
}

void CLI::runInteractive() {
    printWelcome();
    
    std::string line;
    while (running_) {
        printPrompt();
        
        if (!std::getline(std::cin, line)) {
            break; // EOF or error
        }
        
        if (!executeCommand(line)) {
            break;
        }
    }
}

void CLI::executeFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "❌ Cannot open file: " << filename << "\n";
        return;
    }
    
    std::cout << "📄 Executing commands from: " << filename << "\n\n";
    
    std::string line;
    int line_number = 0;
    
    while (std::getline(file, line)) {
        line_number++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::cout << "Line " << line_number << ": " << line << "\n";
        
        if (!executeCommand(line)) {
            break;
        }
    }
    
    std::cout << "\n✅ Finished executing " << filename << "\n";
}
