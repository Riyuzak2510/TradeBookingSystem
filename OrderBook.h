#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include <map>
#include <set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>

// Comparator for time-based ordering within price levels (FIFO)
struct OrderComparator {
    bool operator()(const std::shared_ptr<Order>& a, const std::shared_ptr<Order>& b) const;
};

class OrderBook {
private:
    std::string symbol;
    // Buy orders: higher price first, then FIFO
    std::map<double, std::set<std::shared_ptr<Order>, OrderComparator>, std::greater<double>> buyOrders;
    // Sell orders: lower price first, then FIFO
    std::map<double, std::set<std::shared_ptr<Order>, OrderComparator>> sellOrders;
    // Fast order lookup by ID
    std::unordered_map<int, std::shared_ptr<Order>> orderLookup;
    
public:
    // Constructor
    explicit OrderBook(const std::string& sym);
    
    // Destructor
    ~OrderBook() = default;
    
    // Copy constructor
    OrderBook(const OrderBook& other);
    
    // Assignment operator
    OrderBook& operator=(const OrderBook& other);
    
    // Order management
    void addOrder(std::shared_ptr<Order> order);
    bool cancelOrder(int orderId);
    std::shared_ptr<Order> getOrder(int orderId) const;
    
    // Display functions
    void displayOrderBook() const;
    void displayOrderBookDetailed() const;
    
    // Getters for matching engine
    std::map<double, std::set<std::shared_ptr<Order>, OrderComparator>, std::greater<double>>& getBuyOrders();
    std::map<double, std::set<std::shared_ptr<Order>, OrderComparator>>& getSellOrders();
    std::unordered_map<int, std::shared_ptr<Order>>& getOrderLookup();
    
    // Const versions for read-only access
    const std::map<double, std::set<std::shared_ptr<Order>, OrderComparator>, std::greater<double>>& getBuyOrders() const;
    const std::map<double, std::set<std::shared_ptr<Order>, OrderComparator>>& getSellOrders() const;
    const std::unordered_map<int, std::shared_ptr<Order>>& getOrderLookup() const;
    
    // Utility functions
    const std::string& getSymbol() const { return symbol; }
    bool isEmpty() const;
    size_t getBuyOrderCount() const;
    size_t getSellOrderCount() const;
    size_t getTotalOrderCount() const;
    
    // Best price getters
    double getBestBidPrice() const;
    double getBestAskPrice() const;
    double getSpread() const;
};

#endif // ORDERBOOK_H