#pragma once

#include "Order.h"
#include <map>
#include <deque>
#include <unordered_map>
#include <memory>
#include <optional>
#include <functional>

/**
 * High-performance limit order book implementation.
 * 
 * Key design decisions:
 * - Two separate maps for buy/sell sides (opposite ordering requirements)
 * - std::map for price levels (O(log N) ordered access)
 * - std::deque for FIFO queue within each price level
 * - Hash map for O(1) order lookup and cancellation
 * - Integer prices for deterministic behavior
 * 
 * Complexity:
 * - Insert: O(log N) where N is number of price levels
 * - Cancel: O(1) lookup + O(1) erase = O(1)
 * - Match: O(M) where M is number of orders matched
 * - Best bid/ask: O(1)
 */
class OrderBook {
public:
    // Price level: FIFO queue of orders at a specific price
    using OrderQueue = std::deque<std::shared_ptr<Order>>;
    
    // Buy side: highest price first (descending)
    using BuyBook = std::map<Price, OrderQueue, std::greater<Price>>;
    
    // Sell side: lowest price first (ascending)
    using SellBook = std::map<Price, OrderQueue, std::less<Price>>;
    
    // For O(1) cancellation: order_id -> (price, iterator)
    struct OrderLocation {
        Price price;
        Side side;
        OrderQueue::iterator iter;
    };
    
    OrderBook() = default;
    
    // Add a limit order to the book
    void addOrder(std::shared_ptr<Order> order);
    
    // Cancel an order (O(1) operation)
    bool cancelOrder(OrderId order_id);
    
    // Modify an order
    bool modifyOrder(OrderId order_id, Quantity new_quantity);
    
    // Get best bid price (highest buy price)
    std::optional<Price> getBestBid() const;
    
    // Get best ask price (lowest sell price)
    std::optional<Price> getBestAsk() const;
    
    // Get spread (ask - bid)
    std::optional<Price> getSpread() const;
    
    // Get depth at a specific price level
    Quantity getDepthAtPrice(Side side, Price price) const;
    
    // Get total volume on a side
    Quantity getTotalVolume(Side side) const;
    
    // Check if order exists
    bool hasOrder(OrderId order_id) const;
    
    // Get order by ID
    std::shared_ptr<Order> getOrder(OrderId order_id) const;
    
    // Print book state (for debugging/visualization)
    void printBook(int levels = 5) const;
    
    // Get reference to buy/sell books (for matching engine)
    BuyBook& getBuyBook() { return buy_book_; }
    SellBook& getSellBook() { return sell_book_; }
    const BuyBook& getBuyBook() const { return buy_book_; }
    const SellBook& getSellBook() const { return sell_book_; }
    
    // Remove order from location tracking (for matching engine)
    void removeOrderLocation(OrderId order_id);
    
private:
    BuyBook buy_book_;
    SellBook sell_book_;
    std::unordered_map<OrderId, OrderLocation> order_locations_;
    
    // Helper: remove order from book
    void removeOrderFromBook(const OrderLocation& location);
    
    // Helper: clean up empty price levels
    void cleanupPriceLevel(Side side, Price price);
};
