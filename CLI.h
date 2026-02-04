#pragma once

#include "MatchingEngine.h"
#include <string>
#include <vector>
#include <sstream>
#include <map>

/**
 * Command-line interface for the matching engine.
 * Supports interactive mode, batch mode, and file input.
 */
class CLI {
public:
    explicit CLI(MatchingEngine& engine) : engine_(engine), running_(true) {
        setupTradeCallback();
    }
    
    // Run interactive mode
    void runInteractive();
    
    // Execute a single command
    bool executeCommand(const std::string& command);
    
    // Execute commands from a file
    void executeFile(const std::string& filename);
    
    // Print help message
    void printHelp() const;
    
    // Print welcome message
    void printWelcome() const;
    
private:
    MatchingEngine& engine_;
    bool running_;
    std::map<OrderId, std::string> order_descriptions_; // For tracking orders
    
    void setupTradeCallback();
    void printPrompt() const;
    
    // Command handlers
    void handleBuy(const std::vector<std::string>& args);
    void handleSell(const std::vector<std::string>& args);
    void handleMarketBuy(const std::vector<std::string>& args);
    void handleMarketSell(const std::vector<std::string>& args);
    void handleCancel(const std::vector<std::string>& args);
    void handleModify(const std::vector<std::string>& args);
    void handlePrint(const std::vector<std::string>& args);
    void handleStats();
    void handleClear();
    void handleList();
    void handleExit();
    
    // Utility functions
    std::vector<std::string> parseCommand(const std::string& command);
    double parsePrice(const std::string& str);
    Quantity parseQuantity(const std::string& str);
    Price priceToTicks(double price);
    double ticksToPrice(Price ticks);
};
