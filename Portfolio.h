#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include "Trade.h"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>

class Portfolio {
private:
    std::string userId;
    std::unordered_map<std::string, int> positions; // symbol -> net position (positive = long, negative = short)
    std::vector<Trade> tradeHistory;
    std::unordered_map<std::string, double> averageCosts; // symbol -> average cost per share
    double cashBalance;
    
public:
    // Constructor
    explicit Portfolio(const std::string& user, double initialCash = 100000.0);
    
    // Destructor
    ~Portfolio() = default;
    
    // Copy constructor
    Portfolio(const Portfolio& other);
    
    // Assignment operator
    Portfolio& operator=(const Portfolio& other);
    
    // Trade management
    void addTrade(const Trade& trade, bool isBuyerSide);
    void addBuyTrade(const Trade& trade);
    void addSellTrade(const Trade& trade);
    
    // Portfolio queries
    int getPosition(const std::string& symbol) const;
    double getAverageCost(const std::string& symbol) const;
    double getCashBalance() const { return cashBalance; }
    const std::vector<Trade>& getTradeHistory() const { return tradeHistory; }
    const std::unordered_map<std::string, int>& getAllPositions() const { return positions; }
    
    // Display functions
    void displayPortfolio() const;
    void displayPositions() const;
    void displayTradeHistory(int maxTrades = 10) const;
    void displayPnLSummary() const;
    
    // Portfolio calculations
    double calculateUnrealizedPnL(const std::unordered_map<std::string, double>& currentPrices) const;
    double calculateRealizedPnL() const;
    double getTotalPortfolioValue(const std::unordered_map<std::string, double>& currentPrices) const;
    
    // Utility functions
    const std::string& getUserId() const { return userId; }
    bool hasPosition(const std::string& symbol) const;
    size_t getTradeCount() const { return tradeHistory.size(); }
    
    // Position management
    void clearPosition(const std::string& symbol);
    void setCashBalance(double balance) { cashBalance = balance; }
    void adjustCashBalance(double amount) { cashBalance += amount; }
};

#endif // PORTFOLIO_H