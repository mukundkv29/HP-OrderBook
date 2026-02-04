#include "MatchingEngine.h"
#include <iostream>
#include <algorithm>

OrderId MatchingEngine::submitOrder(Side side, OrderType type, Price price, Quantity quantity) {
    // Validate order
    if (quantity == 0) {
        throw std::runtime_error("Order quantity must be greater than 0");
    }
    
    if (type == OrderType::LIMIT && price <= 0) {
        throw std::runtime_error("Limit order price must be greater than 0");
    }
    
    // Generate order ID
    OrderId order_id = next_order_id_++;
    
    // Create order
    auto order = std::make_shared<Order>(order_id, side, type, price, quantity);
    
    // Match order aggressively
    matchOrder(order);
    
    // If order has remaining quantity and is a limit order, add to book
    if (!order->isFilled() && type == OrderType::LIMIT) {
        order_book_.addOrder(order);
    }
    
    return order_id;
}

bool MatchingEngine::cancelOrder(OrderId order_id) {
    return order_book_.cancelOrder(order_id);
}

bool MatchingEngine::modifyOrder(OrderId order_id, Quantity new_quantity) {
    return order_book_.modifyOrder(order_id, new_quantity);
}

Quantity MatchingEngine::getTotalVolumeTraded() const {
    Quantity total = 0;
    for (const auto& trade : trades_) {
        total += trade.quantity;
    }
    return total;
}

void MatchingEngine::matchOrder(std::shared_ptr<Order> order) {
    if (order->getSide() == Side::BUY) {
        matchBuyOrder(order);
    } else {
        matchSellOrder(order);
    }
}

void MatchingEngine::matchBuyOrder(std::shared_ptr<Order> buy_order) {
    auto& sell_book = order_book_.getSellBook();
    
    // For market orders, match at any price
    // For limit orders, match only if sell price <= buy price
    while (!buy_order->isFilled() && !sell_book.empty()) {
        auto& [best_ask_price, sell_queue] = *sell_book.begin();
        
        // Check if prices cross
        if (buy_order->getType() == OrderType::LIMIT && 
            buy_order->getPrice() < best_ask_price) {
            break; // No more matches possible
        }
        
        // Match against orders at this price level (FIFO)
        while (!buy_order->isFilled() && !sell_queue.empty()) {
            auto sell_order = sell_queue.front();
            
            // Calculate trade quantity
            Quantity trade_qty = std::min(
                buy_order->getRemainingQuantity(),
                sell_order->getRemainingQuantity()
            );
            
            // Execute trade at the passive (resting) order's price
            executeTrade(buy_order, sell_order, best_ask_price, trade_qty);
            
            // Remove filled sell order from queue
            // Critical: do this carefully to maintain iterator validity
            if (sell_order->isFilled()) {
                // Remove from order location map first
                order_book_.removeOrderLocation(sell_order->getId());
                // Then remove from queue
                sell_queue.pop_front();
            }
        }
        
        // Clean up empty price level
        if (sell_queue.empty()) {
            sell_book.erase(sell_book.begin());
        }
    }
}

void MatchingEngine::matchSellOrder(std::shared_ptr<Order> sell_order) {
    auto& buy_book = order_book_.getBuyBook();
    
    // For market orders, match at any price
    // For limit orders, match only if buy price >= sell price
    while (!sell_order->isFilled() && !buy_book.empty()) {
        auto& [best_bid_price, buy_queue] = *buy_book.begin();
        
        // Check if prices cross
        if (sell_order->getType() == OrderType::LIMIT && 
            sell_order->getPrice() > best_bid_price) {
            break; // No more matches possible
        }
        
        // Match against orders at this price level (FIFO)
        while (!sell_order->isFilled() && !buy_queue.empty()) {
            auto buy_order = buy_queue.front();
            
            // Calculate trade quantity
            Quantity trade_qty = std::min(
                sell_order->getRemainingQuantity(),
                buy_order->getRemainingQuantity()
            );
            
            // Execute trade at the passive (resting) order's price
            executeTrade(sell_order, buy_order, best_bid_price, trade_qty);
            
            // Remove filled buy order from queue
            if (buy_order->isFilled()) {
                // Remove from order location map first
                order_book_.removeOrderLocation(buy_order->getId());
                // Then remove from queue
                buy_queue.pop_front();
            }
        }
        
        // Clean up empty price level
        if (buy_queue.empty()) {
            buy_book.erase(buy_book.begin());
        }
    }
}

void MatchingEngine::executeTrade(std::shared_ptr<Order> aggressive_order,
                                  std::shared_ptr<Order> passive_order,
                                  Price trade_price,
                                  Quantity trade_quantity) {
    // Fill both orders
    aggressive_order->fill(trade_quantity);
    passive_order->fill(trade_quantity);
    
    // Create trade record
    Trade trade(
        aggressive_order->getSide() == Side::BUY ? aggressive_order->getId() : passive_order->getId(),
        aggressive_order->getSide() == Side::SELL ? aggressive_order->getId() : passive_order->getId(),
        trade_price,
        trade_quantity
    );
    
    // Store trade
    trades_.push_back(trade);
    
    // Notify callback
    notifyTrade(trade);
}

void MatchingEngine::notifyTrade(const Trade& trade) {
    if (trade_callback_) {
        trade_callback_(trade);
    }
}
