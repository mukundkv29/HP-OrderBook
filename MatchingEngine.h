#pragma once

#include "OrderBook.h"
#include <vector>
#include <functional>
#include <atomic>

/**
 * Matching engine that processes orders and executes trades.
 * 
 * Design principles:
 * - Single-threaded core for deterministic execution
 * - Price-time priority matching
 * - Handles partial fills correctly
 * - Maintains iterator validity during matching
 * - Can be extended with concurrent ingestion/networking layers
 * 
 * Matching rules:
 * - Buy orders match against sell orders (ask side)
 * - Sell orders match against buy orders (bid side)
 * - Aggressive matching: crosses as many price levels as possible
 * - FIFO within same price level (time priority)
 */
class MatchingEngine {
public:
    // Callback for trade notifications
    using TradeCallback = std::function<void(const Trade&)>;
    
    explicit MatchingEngine(OrderBook& book) 
        : order_book_(book)
        , next_order_id_(1)
    {}
    
    // Submit a new order (returns order ID)
    OrderId submitOrder(Side side, OrderType type, Price price, Quantity quantity);
    
    // Cancel an existing order
    bool cancelOrder(OrderId order_id);
    
    // Modify an existing order
    bool modifyOrder(OrderId order_id, Quantity new_quantity);
    
    // Register callback for trade notifications
    void setTradeCallback(TradeCallback callback) {
        trade_callback_ = callback;
    }
    
    // Get all executed trades
    const std::vector<Trade>& getTrades() const { return trades_; }
    
    // Statistics
    size_t getTotalTradeCount() const { return trades_.size(); }
    Quantity getTotalVolumeTraded() const;
    
    // Get the order book
    const OrderBook& getOrderBook() const { return order_book_; }
    OrderBook& getOrderBook() { return order_book_; }
    
private:
    OrderBook& order_book_;
    std::atomic<OrderId> next_order_id_;
    std::vector<Trade> trades_;
    TradeCallback trade_callback_;
    
    // Core matching logic
    void matchOrder(std::shared_ptr<Order> order);
    
    // Match a buy order against sell side
    void matchBuyOrder(std::shared_ptr<Order> order);
    
    // Match a sell order against buy side
    void matchSellOrder(std::shared_ptr<Order> order);
    
    // Execute a trade between two orders
    void executeTrade(std::shared_ptr<Order> aggressive_order,
                     std::shared_ptr<Order> passive_order,
                     Price trade_price,
                     Quantity trade_quantity);
    
    // Notify trade execution
    void notifyTrade(const Trade& trade);
};
