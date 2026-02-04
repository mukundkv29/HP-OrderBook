#include "OrderBook.h"
#include <iostream>
#include <iomanip>

void OrderBook::addOrder(std::shared_ptr<Order> order) {
    if (order->getType() != OrderType::LIMIT) {
        throw std::runtime_error("Only limit orders can be added to the book");
    }
    
    Price price = order->getPrice();
    Side side = order->getSide();
    
    // Add to appropriate book
    OrderQueue::iterator iter;
    if (side == Side::BUY) {
        auto& queue = buy_book_[price];
        queue.push_back(order);
        iter = std::prev(queue.end());
    } else {
        auto& queue = sell_book_[price];
        queue.push_back(order);
        iter = std::prev(queue.end());
    }
    
    // Store location for O(1) cancellation
    order_locations_[order->getId()] = OrderLocation{price, side, iter};
}

bool OrderBook::cancelOrder(OrderId order_id) {
    auto it = order_locations_.find(order_id);
    if (it == order_locations_.end()) {
        return false;
    }
    
    const auto& location = it->second;
    
    // Mark order as cancelled
    (*location.iter)->cancel();
    
    // Remove from book
    removeOrderFromBook(location);
    
    // Remove from lookup map
    order_locations_.erase(it);
    
    return true;
}

bool OrderBook::modifyOrder(OrderId order_id, Quantity new_quantity) {
    auto it = order_locations_.find(order_id);
    if (it == order_locations_.end()) {
        return false;
    }
    
    auto order = *(it->second.iter);
    return order->modifyQuantity(new_quantity);
}

std::optional<Price> OrderBook::getBestBid() const {
    if (buy_book_.empty()) {
        return std::nullopt;
    }
    return buy_book_.begin()->first;
}

std::optional<Price> OrderBook::getBestAsk() const {
    if (sell_book_.empty()) {
        return std::nullopt;
    }
    return sell_book_.begin()->first;
}

std::optional<Price> OrderBook::getSpread() const {
    auto bid = getBestBid();
    auto ask = getBestAsk();
    
    if (!bid.has_value() || !ask.has_value()) {
        return std::nullopt;
    }
    
    return ask.value() - bid.value();
}

Quantity OrderBook::getDepthAtPrice(Side side, Price price) const {
    Quantity total = 0;
    
    if (side == Side::BUY) {
        auto it = buy_book_.find(price);
        if (it != buy_book_.end()) {
            for (const auto& order : it->second) {
                total += order->getRemainingQuantity();
            }
        }
    } else {
        auto it = sell_book_.find(price);
        if (it != sell_book_.end()) {
            for (const auto& order : it->second) {
                total += order->getRemainingQuantity();
            }
        }
    }
    
    return total;
}

Quantity OrderBook::getTotalVolume(Side side) const {
    Quantity total = 0;
    
    if (side == Side::BUY) {
        for (const auto& [price, queue] : buy_book_) {
            for (const auto& order : queue) {
                total += order->getRemainingQuantity();
            }
        }
    } else {
        for (const auto& [price, queue] : sell_book_) {
            for (const auto& order : queue) {
                total += order->getRemainingQuantity();
            }
        }
    }
    
    return total;
}

bool OrderBook::hasOrder(OrderId order_id) const {
    return order_locations_.find(order_id) != order_locations_.end();
}

std::shared_ptr<Order> OrderBook::getOrder(OrderId order_id) const {
    auto it = order_locations_.find(order_id);
    if (it == order_locations_.end()) {
        return nullptr;
    }
    return *(it->second.iter);
}

void OrderBook::printBook(int levels) const {
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║          ORDER BOOK SNAPSHOT           ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    
    // Print spread
    auto spread = getSpread();
    if (spread.has_value()) {
        std::cout << "║ Spread: " << std::setw(28) << spread.value() << " ║\n";
        std::cout << "╠════════════════════════════════════════╣\n";
    }
    
    // Print asks (sell side) - highest first (reverse order)
    std::cout << "║              ASKS (SELL)               ║\n";
    std::cout << "║    Price    │   Quantity  │  Orders   ║\n";
    std::cout << "╟────────────┼─────────────┼───────────╢\n";
    
    int count = 0;
    for (auto it = sell_book_.rbegin(); it != sell_book_.rend() && count < levels; ++it, ++count) {
        Quantity total_qty = 0;
        for (const auto& order : it->second) {
            total_qty += order->getRemainingQuantity();
        }
        std::cout << "║ " << std::setw(10) << it->first 
                  << " │ " << std::setw(11) << total_qty
                  << " │ " << std::setw(9) << it->second.size() << " ║\n";
    }
    
    std::cout << "╠════════════════════════════════════════╣\n";
    
    // Print bids (buy side)
    std::cout << "║              BIDS (BUY)                ║\n";
    std::cout << "║    Price    │   Quantity  │  Orders   ║\n";
    std::cout << "╟────────────┼─────────────┼───────────╢\n";
    
    count = 0;
    for (const auto& [price, queue] : buy_book_) {
        if (count >= levels) break;
        Quantity total_qty = 0;
        for (const auto& order : queue) {
            total_qty += order->getRemainingQuantity();
        }
        std::cout << "║ " << std::setw(10) << price 
                  << " │ " << std::setw(11) << total_qty
                  << " │ " << std::setw(9) << queue.size() << " ║\n";
        count++;
    }
    
    std::cout << "╚════════════════════════════════════════╝\n\n";
}

void OrderBook::removeOrderFromBook(const OrderLocation& location) {
    if (location.side == Side::BUY) {
        auto it = buy_book_.find(location.price);
        if (it != buy_book_.end()) {
            it->second.erase(location.iter);
            if (it->second.empty()) {
                buy_book_.erase(it);
            }
        }
    } else {
        auto it = sell_book_.find(location.price);
        if (it != sell_book_.end()) {
            it->second.erase(location.iter);
            if (it->second.empty()) {
                sell_book_.erase(it);
            }
        }
    }
}

void OrderBook::cleanupPriceLevel(Side side, Price price) {
    if (side == Side::BUY) {
        auto it = buy_book_.find(price);
        if (it != buy_book_.end() && it->second.empty()) {
            buy_book_.erase(it);
        }
    } else {
        auto it = sell_book_.find(price);
        if (it != sell_book_.end() && it->second.empty()) {
            sell_book_.erase(it);
        }
    }
}

void OrderBook::removeOrderLocation(OrderId order_id) {
    order_locations_.erase(order_id);
}
