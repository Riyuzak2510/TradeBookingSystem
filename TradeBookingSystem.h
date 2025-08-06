#ifndef TRADEBOOKINGSYSTEM_H
#define TRADEBOOKINGSYSTEM_H

#include "OrderBook.h"
#include "Portfolio.h"
#include "MatchingEngine.h"
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

class TradeBookingSystem {
private:
    // Order books for each symbol
    std::unordered_map<std::string, std::unique_ptr<OrderBook>> orderBooks;
    
    // User portfolios
    std::unordered_map<std::string, std::unique_ptr<Portfolio>> portfolios;
    
    // Available trading symbols
    std::vector<std::string> availableSymbols;
    
    // Current market prices for P&L calculations
    std::unordered_map<std::string, double> currentMarketPrices;
    
    // System statistics
    size_t totalTradesExecuted;
    double totalVolumeTraded;
    
public:
    // Constructor
    TradeBookingSystem();
    
    // Destructor
    ~TradeBookingSystem() = default;
    
    // Main system interface
    void run();
    void displayMenu();
    
    // User management
    bool loginUser(const std::string& userId);
    void createUserIfNotExists(const std::string& userId);
    
    // Order management
    void placeOrder(const std::string& userId);
    void placeOrderDirect(const std::string& userId, const std::string& symbol, 
                         OrderSide side, int quantity, double price);
    void cancelOrder();
    void cancelOrderDirect(const std::string& symbol, int orderId);
    
    // Display functions
    void viewOrderBook();
    void viewOrderBookDirect(const std::string& symbol);
    void viewPortfolio(const std::string& userId);
    void displayAvailableSymbols();
    void displaySystemStatistics();
    
    // Symbol management
    void addSymbol(const std::string& symbol);
    bool isSymbolAvailable(const std::string& symbol) const;
    const std::vector<std::string>& getAvailableSymbols() const { return availableSymbols; }
    
    // Market data
    void updateMarketPrice(const std::string& symbol, double price);
    double getMarketPrice(const std::string& symbol) const;
    void displayMarketPrices();
    
    // Portfolio management
    Portfolio* getPortfolio(const std::string& userId);
    const Portfolio* getPortfolio(const std::string& userId) const;
    
    // Order book access
    OrderBook* getOrderBook(const std::string& symbol);
    const OrderBook* getOrderBook(const std::string& symbol) const;
    
    // System utilities
    void clearAllOrders();
    void clearOrdersForSymbol(const std::string& symbol);
    void resetSystem();
    
    // Statistics
    size_t getTotalTradesExecuted() const { return totalTradesExecuted; }
    double getTotalVolumeTraded() const { return totalVolumeTraded; }
    
private:
    // Helper functions
    void processTradeResults(const std::vector<Trade>& trades);
    void updatePortfoliosWithTrades(const std::vector<Trade>& trades);
    void updateSystemStatistics(const std::vector<Trade>& trades);
    
    // Input validation
    bool validateSymbolInput(const std::string& symbol);
    bool validatePriceInput(double price);
    bool validateQuantityInput(int quantity);
    
    // Menu helpers
    void handleUserChoice(int choice, const std::string& currentUserId);
    std::string getCurrentTimeString();
    
    // Initialization
    void initializeDefaultSymbols();
    void initializeDefaultPrices();
};

#endif // TRADEBOOKINGSYSTEM_H