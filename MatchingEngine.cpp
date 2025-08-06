#include "MatchingEngine.h"
#include <iostream>
#include <algorithm>

// Main matching function using Price-Time Priority (FIFO within price levels)
std::vector<Trade> MatchingEngine::matchOrders(OrderBook& orderBook) {
    return matchWithPriceTimePriority(orderBook);
}

// Match orders using Price-Time Priority
std::vector<Trade> MatchingEngine::matchWithPriceTimePriority(OrderBook& orderBook) {
    std::vector<Trade> trades;
    
    auto& buyOrders = orderBook.getBuyOrders();
    auto& sellOrders = orderBook.getSellOrders();
    
    while (!buyOrders.empty() && !sellOrders.empty()) {
        // Get best bid (highest price) and best ask (lowest price)
        auto bestBuyPriceIt = buyOrders.begin();
        auto bestSellPriceIt = sellOrders.begin();
        
        double bestBidPrice = bestBuyPriceIt->first;
        double bestAskPrice = bestSellPriceIt->first;
        
        // Check if prices can cross (match condition)
        if (!canMatch(bestBidPrice, bestAskPrice)) {
            break; // No more matches possible
        }
        
        // Get the first orders at these price levels (FIFO within price level)
        auto& buyOrdersAtPrice = bestBuyPriceIt->second;
        auto& sellOrdersAtPrice = bestSellPriceIt->second;
        
        if (buyOrdersAtPrice.empty() || sellOrdersAtPrice.empty()) {
            // Clean up empty price levels
            if (buyOrdersAtPrice.empty()) {
                buyOrders.erase(bestBuyPriceIt);
            }
            if (sellOrdersAtPrice.empty()) {
                sellOrders.erase(bestSellPriceIt);
            }
            continue;
        }
        
        auto buyOrder = *buyOrdersAtPrice.begin();
        auto sellOrder = *sellOrdersAtPrice.begin();
        
        // Validate orders before matching
        if (!validateOrdersForMatching(buyOrder, sellOrder)) {
            std::cerr << "Invalid orders for matching" << std::endl;
            break;
        }
        
        // Determine trade parameters
        int tradeQuantity = std::min(buyOrder->quantity, sellOrder->quantity);
        double tradePrice = determineTradePrice(buyOrder, sellOrder);
        
        // Create and record the trade
        Trade trade = createTrade(buyOrder, sellOrder, tradeQuantity, tradePrice);
        trades.push_back(trade);
        
        std::cout << "TRADE EXECUTED: " << trade.toString() << std::endl;
        
        // Update order quantities
        updateOrderQuantity(buyOrder, tradeQuantity);
        updateOrderQuantity(sellOrder, tradeQuantity);
        
        // Remove fully filled orders
        if (buyOrder->quantity == 0) {
            removeFilledOrder(orderBook, buyOrder);
            buyOrdersAtPrice.erase(buyOrder);
            if (buyOrdersAtPrice.empty()) {
                buyOrders.erase(bestBuyPriceIt);
            }
        }
        
        if (sellOrder->quantity == 0) {
            removeFilledOrder(orderBook, sellOrder);
            sellOrdersAtPrice.erase(sellOrder);
            if (sellOrdersAtPrice.empty()) {
                sellOrders.erase(bestSellPriceIt);
            }
        }
    }
    
    return trades;
}

// Match a specific new order against existing orders in the book
std::vector<Trade> MatchingEngine::matchOrder(OrderBook& orderBook, std::shared_ptr<Order> newOrder) {
    std::vector<Trade> trades;
    
    if (!newOrder || !newOrder->isValid()) {
        std::cerr << "Invalid order cannot be matched" << std::endl;
        return trades;
    }
    
    auto& buyOrders = orderBook.getBuyOrders();
    auto& sellOrders = orderBook.getSellOrders();
    
    if (newOrder->side == OrderSide::BUY) {
        // New buy order - match against sell orders
        while (!sellOrders.empty() && newOrder->quantity > 0) {
            auto bestSellPriceIt = sellOrders.begin();
            double bestAskPrice = bestSellPriceIt->first;
            
            if (!canMatch(newOrder->price, bestAskPrice)) {
                break; // No match possible
            }
            
            auto& sellOrdersAtPrice = bestSellPriceIt->second;
            if (sellOrdersAtPrice.empty()) {
                sellOrders.erase(bestSellPriceIt);
                continue;
            }
            
            auto sellOrder = *sellOrdersAtPrice.begin();
            
            int tradeQuantity = std::min(newOrder->quantity, sellOrder->quantity);
            double tradePrice = sellOrder->price; // Use existing order's price
            
            Trade trade = createTrade(newOrder, sellOrder, tradeQuantity, tradePrice);
            trades.push_back(trade);
            
            // Update quantities
            updateOrderQuantity(newOrder, tradeQuantity);
            updateOrderQuantity(sellOrder, tradeQuantity);
            
            // Remove filled sell order
            if (sellOrder->quantity == 0) {
                removeFilledOrder(orderBook, sellOrder);
                sellOrdersAtPrice.erase(sellOrder);
                if (sellOrdersAtPrice.empty()) {
                    sellOrders.erase(bestSellPriceIt);
                }
            }
        }
        
        // Add remaining quantity to buy side if not fully filled
        if (newOrder->quantity > 0) {
            orderBook.addOrder(newOrder);
        }
        
    } else { // OrderSide::SELL
        // New sell order - match against buy orders
        while (!buyOrders.empty() && newOrder->quantity > 0) {
            auto bestBuyPriceIt = buyOrders.begin();
            double bestBidPrice = bestBuyPriceIt->first;
            
            if (!canMatch(bestBidPrice, newOrder->price)) {
                break; // No match possible
            }
            
            auto& buyOrdersAtPrice = bestBuyPriceIt->second;
            if (buyOrdersAtPrice.empty()) {
                buyOrders.erase(bestBuyPriceIt);
                continue;
            }
            
            auto buyOrder = *buyOrdersAtPrice.begin();
            
            int tradeQuantity = std::min(newOrder->quantity, buyOrder->quantity);
            double tradePrice = buyOrder->price; // Use existing order's price
            
            Trade trade = createTrade(buyOrder, newOrder, tradeQuantity, tradePrice);
            trades.push_back(trade);
            
            // Update quantities
            updateOrderQuantity(newOrder, tradeQuantity);
            updateOrderQuantity(buyOrder, tradeQuantity);
            
            // Remove filled buy order
            if (buyOrder->quantity == 0) {
                removeFilledOrder(orderBook, buyOrder);
                buyOrdersAtPrice.erase(buyOrder);
                if (buyOrdersAtPrice.empty()) {
                    buyOrders.erase(bestBuyPriceIt);
                }
            }
        }
        
        // Add remaining quantity to sell side if not fully filled
        if (newOrder->quantity > 0) {
            orderBook.addOrder(newOrder);
        }
    }
    
    return trades;
}

// FIFO matching (same as Price-Time Priority for this implementation)
std::vector<Trade> MatchingEngine::matchWithFIFO(OrderBook& orderBook) {
    return matchWithPriceTimePriority(orderBook);
}

// Create a trade between two orders
Trade MatchingEngine::createTrade(const std::shared_ptr<Order>& buyOrder, 
                                const std::shared_ptr<Order>& sellOrder,
                                int quantity, double price) {
    return Trade(buyOrder->symbol, buyOrder->orderId, sellOrder->orderId,
                buyOrder->userId, sellOrder->userId, quantity, price);
}

// Update order quantity after partial execution
void MatchingEngine::updateOrderQuantity(std::shared_ptr<Order> order, int executedQuantity) {
    if (order && executedQuantity > 0) {
        order->quantity -= executedQuantity;
        if (order->quantity < 0) {
            order->quantity = 0; // Safety check
        }
    }
}

// Remove completely filled order from lookup table
void MatchingEngine::removeFilledOrder(OrderBook& orderBook, std::shared_ptr<Order> order) {
    if (order) {
        auto& orderLookup = orderBook.getOrderLookup();
        orderLookup.erase(order->orderId);
    }
}

// Validate that orders can be matched
bool MatchingEngine::validateOrdersForMatching(const std::shared_ptr<Order>& buyOrder,
                                             const std::shared_ptr<Order>& sellOrder) {
    if (!buyOrder || !sellOrder) {
        return false;
    }
    
    if (!buyOrder->isValid() || !sellOrder->isValid()) {
        return false;
    }
    
    if (buyOrder->symbol != sellOrder->symbol) {
        return false;
    }
    
    if (buyOrder->side != OrderSide::BUY || sellOrder->side != OrderSide::SELL) {
        return false;
    }
    
    if (buyOrder->quantity <= 0 || sellOrder->quantity <= 0) {
        return false;
    }
    
    return true;
}