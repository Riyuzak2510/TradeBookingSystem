#ifndef MATCHINGENGINE_H
#define MATCHINGENGINE_H

#include "OrderBook.h"
#include "Trade.h"
#include <vector>
#include <memory>

class MatchingEngine {
public:
    // Main matching function - processes all possible matches in an order book
    static std::vector<Trade> matchOrders(OrderBook& orderBook);
    
    // Match a specific order against the order book
    static std::vector<Trade> matchOrder(OrderBook& orderBook, std::shared_ptr<Order> newOrder);
    
    // Helper functions for different matching strategies
    static std::vector<Trade> matchWithFIFO(OrderBook& orderBook);
    static std::vector<Trade> matchWithPriceTimePriority(OrderBook& orderBook);
    
private:
    // Internal helper functions
    static Trade createTrade(const std::shared_ptr<Order>& buyOrder, 
                           const std::shared_ptr<Order>& sellOrder,
                           int quantity, double price);
    
    static void updateOrderQuantity(std::shared_ptr<Order> order, int executedQuantity);
    
    static bool canMatch(double bidPrice, double askPrice);
    
    static double determineTradePrice(const std::shared_ptr<Order>& buyOrder,
                                    const std::shared_ptr<Order>& sellOrder);
    
    static void removeFilledOrder(OrderBook& orderBook, std::shared_ptr<Order> order);
    
    // Validation functions
    static bool validateOrdersForMatching(const std::shared_ptr<Order>& buyOrder,
                                        const std::shared_ptr<Order>& sellOrder);
};

// Inline helper functions for performance
inline bool MatchingEngine::canMatch(double bidPrice, double askPrice) {
    return bidPrice >= askPrice;
}

inline double MatchingEngine::determineTradePrice(const std::shared_ptr<Order>& buyOrder,
                                                const std::shared_ptr<Order>& sellOrder) {
    // Use the price of the order that was placed first (already in the book)
    // This is a common convention in many exchanges
    return (buyOrder->timestamp < sellOrder->timestamp) ? buyOrder->price : sellOrder->price;
}

#endif // MATCHINGENGINE_H