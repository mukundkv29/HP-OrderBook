#pragma once

#include <cstdint>
#include <string>
#include <chrono>
#include <stdexcept>

// Price represented as integer ticks to avoid floating-point precision issues
// For example: $100.25 with tick size $0.01 = 10025 ticks
using Price = int64_t;
using Quantity = uint64_t;
using OrderId = uint64_t;

enum class Side {
    BUY,
    SELL
};

enum class OrderType {
    LIMIT,
    MARKET
};

enum class OrderStatus {
    NEW,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED
};

/**
 * Represents a single order in the order book.
 * Designed to minimize memory footprint and copying.
 */
class Order {
public:
    Order(OrderId id, Side side, OrderType type, Price price, Quantity quantity)
        : id_(id)
        , side_(side)
        , type_(type)
        , price_(price)
        , initial_quantity_(quantity)
        , remaining_quantity_(quantity)
        , status_(OrderStatus::NEW)
        , timestamp_(std::chrono::steady_clock::now())
    {}

    // Getters
    OrderId getId() const { return id_; }
    Side getSide() const { return side_; }
    OrderType getType() const { return type_; }
    Price getPrice() const { return price_; }
    Quantity getInitialQuantity() const { return initial_quantity_; }
    Quantity getRemainingQuantity() const { return remaining_quantity_; }
    Quantity getFilledQuantity() const { return initial_quantity_ - remaining_quantity_; }
    OrderStatus getStatus() const { return status_; }
    
    // Check if order is fully filled
    bool isFilled() const { return remaining_quantity_ == 0; }
    
    // Fill the order (reduce remaining quantity)
    void fill(Quantity quantity) {
        if (quantity > remaining_quantity_) {
            throw std::runtime_error("Cannot fill more than remaining quantity");
        }
        remaining_quantity_ -= quantity;
        
        if (remaining_quantity_ == 0) {
            status_ = OrderStatus::FILLED;
        } else if (remaining_quantity_ < initial_quantity_) {
            status_ = OrderStatus::PARTIALLY_FILLED;
        }
    }
    
    // Cancel the order
    void cancel() {
        status_ = OrderStatus::CANCELLED;
    }
    
    // Modify order quantity (only allowed if not partially filled)
    bool modifyQuantity(Quantity new_quantity) {
        if (status_ == OrderStatus::PARTIALLY_FILLED) {
            return false; // Can't modify partially filled orders in this implementation
        }
        initial_quantity_ = new_quantity;
        remaining_quantity_ = new_quantity;
        return true;
    }

private:
    OrderId id_;
    Side side_;
    OrderType type_;
    Price price_;
    Quantity initial_quantity_;
    Quantity remaining_quantity_;
    OrderStatus status_;
    std::chrono::steady_clock::time_point timestamp_;
};

// Trade execution result
struct Trade {
    OrderId buy_order_id;
    OrderId sell_order_id;
    Price price;
    Quantity quantity;
    std::chrono::steady_clock::time_point timestamp;
    
    Trade(OrderId buy_id, OrderId sell_id, Price p, Quantity q)
        : buy_order_id(buy_id)
        , sell_order_id(sell_id)
        , price(p)
        , quantity(q)
        , timestamp(std::chrono::steady_clock::now())
    {}
};
