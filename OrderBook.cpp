#include "OrderBook.h"
#include <iomanip>
#include <limits>

// OrderComparator implementation
bool OrderComparator::operator()(const std::shared_ptr<Order>& a, const std::shared_ptr<Order>& b) const {
    if (a->timestamp != b->timestamp) {
        return a->timestamp < b->timestamp; // FIFO: earlier orders first
    }
    return a->orderId < b->orderId; // Fallback to order ID for deterministic ordering
}

// Constructor
OrderBook::OrderBook(const std::string& sym) : symbol(sym) {
}

// Copy constructor
OrderBook::OrderBook(const OrderBook& other) 
    : symbol(other.symbol), buyOrders(other.buyOrders), 
      sellOrders(other.sellOrders), orderLookup(other.orderLookup) {
}

// Assignment operator
OrderBook& OrderBook::operator=(const OrderBook& other) {
    if (this != &other) {
        symbol = other.symbol;
        buyOrders = other.buyOrders;
        sellOrders = other.sellOrders;
        orderLookup = other.orderLookup;
    }
    return *this;
}

// Add order to the book
void OrderBook::addOrder(std::shared_ptr<Order> order) {
    if (!order || !order->isValid()) {
        std::cerr << "Invalid order cannot be added to order book" << std::endl;
        return;
    }
    
    // Add to lookup map for fast access
    orderLookup[order->orderId] = order;
    
    // Add to appropriate side of the book
    if (order->side == OrderSide::BUY) {
        buyOrders[order->price].insert(order);
    } else {
        sellOrders[order->price].insert(order);
    }
}

// Cancel order from the book
bool OrderBook::cancelOrder(int orderId) {
    auto it = orderLookup.find(orderId);
    if (it == orderLookup.end()) {
        return false; // Order not found
    }
    
    auto order = it->second;
    
    // Remove from appropriate side
    if (order->side == OrderSide::BUY) {
        auto priceIt = buyOrders.find(order->price);
        if (priceIt != buyOrders.end()) {
            auto& priceLevel = priceIt->second;
            priceLevel.erase(order);
            if (priceLevel.empty()) {
                buyOrders.erase(priceIt);
            }
        }
    } else {
        auto priceIt = sellOrders.find(order->price);
        if (priceIt != sellOrders.end()) {
            auto& priceLevel = priceIt->second;
            priceLevel.erase(order);
            if (priceLevel.empty()) {
                sellOrders.erase(priceIt);
            }
        }
    }
    
    // Remove from lookup
    orderLookup.erase(orderId);
    return true;
}

// Get order by ID
std::shared_ptr<Order> OrderBook::getOrder(int orderId) const {
    auto it = orderLookup.find(orderId);
    return (it != orderLookup.end()) ? it->second : nullptr;
}

// Display order book summary
void OrderBook::displayOrderBook() const {
    std::cout << "\n=== Order Book for " << symbol << " ===" << std::endl;
    
    // Display sell orders (asks) in descending price order
    std::cout << "SELL ORDERS (ASKS):" << std::endl;
    if (sellOrders.empty()) {
        std::cout << "  No sell orders" << std::endl;
    } else {
        for (auto it = sellOrders.rbegin(); it != sellOrders.rend(); ++it) {
            int totalQty = 0;
            for (const auto& order : it->second) {
                totalQty += order->quantity;
            }
            std::cout << "  $" << std::fixed << std::setprecision(2) << it->first 
                     << " x " << totalQty << " (" << it->second.size() << " orders)" << std::endl;
        }
    }
    
    // Display spread
    double spread = getSpread();
    std::cout << "--- SPREAD: $" << std::fixed << std::setprecision(2) << spread << " ---" << std::endl;
    
    // Display buy orders (bids) in descending price order
    std::cout << "BUY ORDERS (BIDS):" << std::endl;
    if (buyOrders.empty()) {
        std::cout << "  No buy orders" << std::endl;
    } else {
        for (const auto& entry : buyOrders) {
            const auto& price = entry.first;
            const auto& orders = entry.second;
            int totalQty = 0;
            for (const auto& order : orders) {
                totalQty += order->quantity;
            }
            std::cout << "  $" << std::fixed << std::setprecision(2) << price 
                      << " x " << totalQty << " (" << orders.size() << " orders)" << std::endl;
        }
    }
    
    std::cout << "Total Orders: " << getTotalOrderCount() << std::endl;
}

// Display detailed order book
void OrderBook::displayOrderBookDetailed() const {
    std::cout << "\n=== Detailed Order Book for " << symbol << " ===" << std::endl;
    
    std::cout << "SELL ORDERS:" << std::endl;
    for (auto it = sellOrders.rbegin(); it != sellOrders.rend(); ++it) {
        std::cout << "  Price $" << std::fixed << std::setprecision(2) << it->first << ":" << std::endl;
        for (const auto& order : it->second) {
            std::cout << "    " << order->toString() << std::endl;
        }
    }
    
    std::cout << "BUY ORDERS:" << std::endl;
    for (auto it = buyOrders.begin(); it != buyOrders.end(); ++it) {
        const double& price = it->first;
        const auto& orders = it->second;
        std::cout << "  Price $" << std::fixed << std::setprecision(2) << price << ":" << std::endl;
        for (const auto& order : orders) {
            std::cout << "    " << order->toString() << std::endl;
        }
    }
}

// Getters for matching engine (non-const)
std::map<double, std::set<std::shared_ptr<Order>, OrderComparator>, std::greater<double>>& 
OrderBook::getBuyOrders() {
    return buyOrders;
}

std::map<double, std::set<std::shared_ptr<Order>, OrderComparator>>& 
OrderBook::getSellOrders() {
    return sellOrders;
}

std::unordered_map<int, std::shared_ptr<Order>>& 
OrderBook::getOrderLookup() {
    return orderLookup;
}

// Const versions
const std::map<double, std::set<std::shared_ptr<Order>, OrderComparator>, std::greater<double>>& 
OrderBook::getBuyOrders() const {
    return buyOrders;
}

const std::map<double, std::set<std::shared_ptr<Order>, OrderComparator>>& 
OrderBook::getSellOrders() const {
    return sellOrders;
}

const std::unordered_map<int, std::shared_ptr<Order>>& 
OrderBook::getOrderLookup() const {
    return orderLookup;
}

// Utility functions
bool OrderBook::isEmpty() const {
    return buyOrders.empty() && sellOrders.empty();
}

size_t OrderBook::getBuyOrderCount() const {
    size_t count = 0;
    for (const auto& pair : buyOrders) {
        const auto& price = pair.first;
        const auto& orders = pair.second;
        count += orders.size();
    }
    return count;
}

size_t OrderBook::getSellOrderCount() const {
    size_t count = 0;
    for (const auto& elem : sellOrders) {
        const auto& price = elem.first;
        const auto& orders = elem.second;
        count += orders.size();
    }
    return count;
}

size_t OrderBook::getTotalOrderCount() const {
    return getBuyOrderCount() + getSellOrderCount();
}

double OrderBook::getBestBidPrice() const {
    return buyOrders.empty() ? 0.0 : buyOrders.begin()->first;
}

double OrderBook::getBestAskPrice() const {
    return sellOrders.empty() ? std::numeric_limits<double>::max() : sellOrders.begin()->first;
}

double OrderBook::getSpread() const {
    if (buyOrders.empty() || sellOrders.empty()) {
        return std::numeric_limits<double>::max();
    }
    return getBestAskPrice() - getBestBidPrice();
}