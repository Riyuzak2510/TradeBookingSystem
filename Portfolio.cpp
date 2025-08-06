#include "Portfolio.h"
#include <iomanip>
#include <algorithm>

// Constructor
Portfolio::Portfolio(const std::string& user, double initialCash) 
    : userId(user), cashBalance(initialCash) {
}

// Copy constructor
Portfolio::Portfolio(const Portfolio& other) 
    : userId(other.userId), positions(other.positions), 
      tradeHistory(other.tradeHistory), averageCosts(other.averageCosts),
      cashBalance(other.cashBalance) {
}

// Assignment operator
Portfolio& Portfolio::operator=(const Portfolio& other) {
    if (this != &other) {
        userId = other.userId;
        positions = other.positions;
        tradeHistory = other.tradeHistory;
        averageCosts = other.averageCosts;
        cashBalance = other.cashBalance;
    }
    return *this;
}

// Add trade with buyer/seller indication
void Portfolio::addTrade(const Trade& trade, bool isBuyerSide) {
    tradeHistory.push_back(trade);
    
    if (isBuyerSide) {
        addBuyTrade(trade);
    } else {
        addSellTrade(trade);
    }
}

// Process buy trade
void Portfolio::addBuyTrade(const Trade& trade) {
    const std::string& symbol = trade.symbol;
    int quantity = trade.quantity;
    double price = trade.price;
    double totalCost = quantity * price;
    
    // Update cash balance
    cashBalance -= totalCost;
    
    // Update position and average cost
    int currentPosition = positions[symbol];
    double currentAvgCost = averageCosts[symbol];
    
    if (currentPosition >= 0) {
        // Adding to long position or creating new long position
        double totalValue = (currentPosition * currentAvgCost) + totalCost;
        positions[symbol] = currentPosition + quantity;
        averageCosts[symbol] = totalValue / positions[symbol];
    } else {
        // We have a short position, reducing it
        if (quantity <= -currentPosition) {
            // Partially or fully covering short position
            positions[symbol] = currentPosition + quantity;
            // Keep the same average cost for remaining short position
        } else {
            // Covering entire short position and going long
            int excessQuantity = quantity + currentPosition; // currentPosition is negative
            positions[symbol] = excessQuantity;
            averageCosts[symbol] = price; // New average cost for the long position
        }
    }
}

// Process sell trade
void Portfolio::addSellTrade(const Trade& trade) {
    const std::string& symbol = trade.symbol;
    int quantity = trade.quantity;
    double price = trade.price;
    double totalRevenue = quantity * price;
    
    // Update cash balance
    cashBalance += totalRevenue;
    
    // Update position and average cost
    int currentPosition = positions[symbol];
    double currentAvgCost = averageCosts[symbol];
    
    if (currentPosition <= 0) {
        // Adding to short position or creating new short position
        double totalValue = (-currentPosition * currentAvgCost) + totalRevenue;
        positions[symbol] = currentPosition - quantity;
        averageCosts[symbol] = totalValue / (-positions[symbol]);
    } else {
        // We have a long position, reducing it
        if (quantity <= currentPosition) {
            // Partially or fully selling long position
            positions[symbol] = currentPosition - quantity;
            // Keep the same average cost for remaining long position
        } else {
            // Selling entire long position and going short
            int excessQuantity = quantity - currentPosition;
            positions[symbol] = -excessQuantity;
            averageCosts[symbol] = price; // New average cost for the short position
        }
    }
}

// Get position for a symbol
int Portfolio::getPosition(const std::string& symbol) const {
    auto it = positions.find(symbol);
    return (it != positions.end()) ? it->second : 0;
}

// Get average cost for a symbol
double Portfolio::getAverageCost(const std::string& symbol) const {
    auto it = averageCosts.find(symbol);
    return (it != averageCosts.end()) ? it->second : 0.0;
}

// Display complete portfolio
void Portfolio::displayPortfolio() const {
    std::cout << "\n=== Portfolio for " << userId << " ===" << std::endl;
    std::cout << "Cash Balance: $" << std::fixed << std::setprecision(2) << cashBalance << std::endl;
    
    displayPositions();
    displayTradeHistory();
}

// Display current positions
void Portfolio::displayPositions() const {
    std::cout << "\nCURRENT POSITIONS:" << std::endl;
    if (positions.empty()) {
        std::cout << "  No positions" << std::endl;
        return;
    }
    
    std::cout << std::setw(8) << "Symbol" << std::setw(10) << "Position" 
              << std::setw(12) << "Avg Cost" << std::setw(12) << "Market Val" << std::endl;
    std::cout << std::string(42, '-') << std::endl;
    
    for (const auto& p : positions) {
        const std::string& symbol = p.first;
        int position = p.second;
        if (position != 0) {
            double avgCost = getAverageCost(symbol);
            double marketValue = position * avgCost;
            std::cout << std::setw(8) << symbol
                      << std::setw(10) << position
                      << std::setw(12) << std::fixed << std::setprecision(2) << avgCost
                      << std::setw(12) << std::fixed << std::setprecision(2) << marketValue;
            if (position > 0) {
                std::cout << " (LONG)";
            } else {
                std::cout << " (SHORT)";
            }
            std::cout << std::endl;
        }
    }
}

// Display trade history
void Portfolio::displayTradeHistory(int maxTrades) const {
    std::cout << "\nRECENT TRADES (Last " << maxTrades << "):" << std::endl;
    if (tradeHistory.empty()) {
        std::cout << "  No trades executed" << std::endl;
        return;
    }
    
    int count = 0;
    for (auto it = tradeHistory.rbegin(); it != tradeHistory.rend() && count < maxTrades; ++it, ++count) {
        std::cout << "  " << it->toString();
        
        // Indicate if this user was buyer or seller
        if (it->buyUserId == userId) {
            std::cout << " [BUY]";
        } else if (it->sellUserId == userId) {
            std::cout << " [SELL]";
        }
        std::cout << std::endl;
    }
}

// Display P&L summary
void Portfolio::displayPnLSummary() const {
    std::cout << "\nP&L SUMMARY:" << std::endl;
    double realizedPnL = calculateRealizedPnL();
    std::cout << "Realized P&L: $" << std::fixed << std::setprecision(2) << realizedPnL << std::endl;
    std::cout << "Note: Unrealized P&L requires current market prices" << std::endl;
}

// Calculate realized P&L from completed trades
double Portfolio::calculateRealizedPnL() const {
    // This is a simplified calculation
    // In practice, you'd need more sophisticated P&L tracking
    double totalCashFlow = 0.0;
    
    for (const auto& trade : tradeHistory) {
        if (trade.buyUserId == userId) {
            totalCashFlow -= trade.quantity * trade.price; // Cash out for buying
        } else if (trade.sellUserId == userId) {
            totalCashFlow += trade.quantity * trade.price; // Cash in for selling
        }
    }
    
    return totalCashFlow;
}

// Calculate unrealized P&L
double Portfolio::calculateUnrealizedPnL(const std::unordered_map<std::string, double>& currentPrices) const {
    double unrealizedPnL = 0.0;
    
    for (const auto& kv : positions) {
        const std::string& symbol = kv.first;
        int position = kv.second;
        if (position != 0) {
            auto priceIt = currentPrices.find(symbol);
            if (priceIt != currentPrices.end()) {
                double currentPrice = priceIt->second;
                double avgCost = getAverageCost(symbol);
                if (position > 0) {
                    unrealizedPnL += position * (currentPrice - avgCost);
                } else {
                    unrealizedPnL += (-position) * (avgCost - currentPrice);
                }
            }
        }
    }
    
    return unrealizedPnL;
}

// Calculate total portfolio value
double Portfolio::getTotalPortfolioValue(const std::unordered_map<std::string, double>& currentPrices) const {
    double totalValue = cashBalance;
    
    for (const auto& entry : positions) {
        const std::string& symbol = entry.first;
        int position = entry.second;
        if (position != 0) {
            auto priceIt = currentPrices.find(symbol);
            if (priceIt != currentPrices.end()) {
                double currentPrice = priceIt->second;
                totalValue += position * currentPrice;
            }
        }
    }
    
    return totalValue;
}

// Check if has position in symbol
bool Portfolio::hasPosition(const std::string& symbol) const {
    auto it = positions.find(symbol);
    return (it != positions.end()) && (it->second != 0);
}

// Clear position for a symbol
void Portfolio::clearPosition(const std::string& symbol) {
    positions[symbol] = 0;
    averageCosts[symbol] = 0.0;
}