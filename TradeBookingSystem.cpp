#include "TradeBookingSystem.h"
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <ctime>

// Constructor
TradeBookingSystem::TradeBookingSystem() : totalTradesExecuted(0), totalVolumeTraded(0.0) {
    initializeDefaultSymbols();
    initializeDefaultPrices();
}

// Initialize default trading symbols
void TradeBookingSystem::initializeDefaultSymbols() {
    availableSymbols = {"AAPL", "GOOGL", "MSFT", "TSLA", "AMZN", "META", "NVDA", "JPM", "V", "JNJ"};
}

// Initialize default market prices
void TradeBookingSystem::initializeDefaultPrices() {
    currentMarketPrices["AAPL"] = 150.0;
    currentMarketPrices["GOOGL"] = 2500.0;
    currentMarketPrices["MSFT"] = 300.0;
    currentMarketPrices["TSLA"] = 200.0;
    currentMarketPrices["AMZN"] = 3000.0;
    currentMarketPrices["META"] = 250.0;
    currentMarketPrices["NVDA"] = 400.0;
    currentMarketPrices["JPM"] = 140.0;
    currentMarketPrices["V"] = 220.0;
    currentMarketPrices["JNJ"] = 160.0;
}

// Main system loop
void TradeBookingSystem::run() {
    std::string userId;
    std::cout << "=== Welcome to C++ Trade Booking System ===" << std::endl;
    std::cout << "Enter your user ID: ";
    std::cin >> userId;
    
    if (!loginUser(userId)) {
        createUserIfNotExists(userId);
    }
    
    std::cout << "Welcome, " << userId << "!" << std::endl;
    
    int choice;
    while (true) {
        displayMenu();
        std::cin >> choice;
        
        if (choice == 7) {
            std::cout << "Thank you for using the Trade Booking System. Goodbye!" << std::endl;
            break;
        }
        
        handleUserChoice(choice, userId);
    }
}

// Display main menu
void TradeBookingSystem::displayMenu() {
    std::cout << "\n=== Trade Booking System Menu ===" << std::endl;
    std::cout << "1. Place Order" << std::endl;
    std::cout << "2. Cancel Order" << std::endl;
    std::cout << "3. View Order Book" << std::endl;
    std::cout << "4. View Portfolio" << std::endl;
    std::cout << "5. View Available Symbols" << std::endl;
    std::cout << "6. View System Statistics" << std::endl;
    std::cout << "7. Exit" << std::endl;
    std::cout << "Enter your choice (1-7): ";
}

// Handle user menu choice
void TradeBookingSystem::handleUserChoice(int choice, const std::string& currentUserId) {
    switch (choice) {
        case 1:
            placeOrder(currentUserId);
            break;
        case 2:
            cancelOrder();
            break;
        case 3:
            viewOrderBook();
            break;
        case 4:
            viewPortfolio(currentUserId);
            break;
        case 5:
            displayAvailableSymbols();
            break;
        case 6:
            displaySystemStatistics();
            break;
        default:
            std::cout << "Invalid choice! Please enter 1-7." << std::endl;
    }
}

// User login
bool TradeBookingSystem::loginUser(const std::string& userId) {
    return portfolios.find(userId) != portfolios.end();
}

// Create new user
void TradeBookingSystem::createUserIfNotExists(const std::string& userId) {
    if (portfolios.find(userId) == portfolios.end()) {
        portfolios[userId] = std::make_unique<Portfolio>(userId);
        std::cout << "New user account created for: " << userId << std::endl;
    }
}

// Place order interface
void TradeBookingSystem::placeOrder(const std::string& userId) {
    std::string symbol;
    char sideChar;
    int quantity;
    double price;
    
    displayAvailableSymbols();
    std::cout << "Enter symbol: ";
    std::cin >> symbol;
    
    if (!validateSymbolInput(symbol)) {
        return;
    }
    
    std::cout << "Enter side (B for Buy, S for Sell): ";
    std::cin >> sideChar;
    
    std::cout << "Enter quantity: ";
    std::cin >> quantity;
    
    if (!validateQuantityInput(quantity)) {
        return;
    }
    
    std::cout << "Enter price: ";
    std::cin >> price;
    
    if (!validatePriceInput(price)) {
        return;
    }
    
    OrderSide side = (sideChar == 'B' || sideChar == 'b') ? OrderSide::BUY : OrderSide::SELL;
    placeOrderDirect(userId, symbol, side, quantity, price);
}

// Place order directly
void TradeBookingSystem::placeOrderDirect(const std::string& userId, const std::string& symbol, 
                                        OrderSide side, int quantity, double price) {
    // Create order book if it doesn't exist
    if (orderBooks.find(symbol) == orderBooks.end()) {
        orderBooks[symbol] = std::make_unique<OrderBook>(symbol);
    }
    
    // Create the order
    auto order = std::make_shared<Order>(symbol, side, quantity, price, userId);
    std::cout << "\nPlacing: " << order->toString() << std::endl;
    
    // Use matching engine to process the order
    auto trades = MatchingEngine::matchOrder(*orderBooks[symbol], order);
    
    // Process trade results
    processTradeResults(trades);
    
    if (trades.empty()) {
        std::cout << "Order placed in book (no immediate matches)" << std::endl;
    } else {
        std::cout << "Order processed with " << trades.size() << " trade(s)" << std::endl;
    }
}

// Cancel order interface
void TradeBookingSystem::cancelOrder() {
    std::string symbol;
    int orderId;
    
    std::cout << "Enter symbol: ";
    std::cin >> symbol;
    
    if (!validateSymbolInput(symbol)) {
        return;
    }
    
    std::cout << "Enter order ID to cancel: ";
    std::cin >> orderId;
    
    cancelOrderDirect(symbol, orderId);
}

// Cancel order directly
void TradeBookingSystem::cancelOrderDirect(const std::string& symbol, int orderId) {
    auto it = orderBooks.find(symbol);
    if (it != orderBooks.end()) {
        if (it->second->cancelOrder(orderId)) {
            std::cout << "Order " << orderId << " cancelled successfully!" << std::endl;
        } else {
            std::cout << "Order " << orderId << " not found!" << std::endl;
        }
    } else {
        std::cout << "No order book exists for symbol " << symbol << std::endl;
    }
}

// View order book interface
void TradeBookingSystem::viewOrderBook() {
    std::string symbol;
    std::cout << "Enter symbol: ";
    std::cin >> symbol;
    
    if (validateSymbolInput(symbol)) {
        viewOrderBookDirect(symbol);
    }
}

// View order book directly
void TradeBookingSystem::viewOrderBookDirect(const std::string& symbol) {
    auto it = orderBooks.find(symbol);
    if (it != orderBooks.end()) {
        it->second->displayOrderBook();
    } else {
        std::cout << "No order book exists for symbol " << symbol << std::endl;
        std::cout << "Place an order first to create the order book." << std::endl;
    }
}

// View portfolio
void TradeBookingSystem::viewPortfolio(const std::string& userId) {
    auto it = portfolios.find(userId);
    if (it != portfolios.end()) {
        it->second->displayPortfolio();
        
        // Show unrealized P&L with current market prices
        double unrealizedPnL = it->second->calculateUnrealizedPnL(currentMarketPrices);
        double totalValue = it->second->getTotalPortfolioValue(currentMarketPrices);
        
        std::cout << "\nMARKET VALUATION:" << std::endl;
        std::cout << "Unrealized P&L: $" << std::fixed << std::setprecision(2) << unrealizedPnL << std::endl;
        std::cout << "Total Portfolio Value: $" << std::fixed << std::setprecision(2) << totalValue << std::endl;
    } else {
        std::cout << "No portfolio found for user " << userId << std::endl;
    }
}

// Display available symbols
void TradeBookingSystem::displayAvailableSymbols() {
    std::cout << "\nAvailable symbols: ";
    for (size_t i = 0; i < availableSymbols.size(); ++i) {
        std::cout << availableSymbols[i];
        if (i < availableSymbols.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

// Display system statistics
void TradeBookingSystem::displaySystemStatistics() {
    std::cout << "\n=== System Statistics ===" << std::endl;
    std::cout << "Total Trades Executed: " << totalTradesExecuted << std::endl;
    std::cout << "Total Volume Traded: $" << std::fixed << std::setprecision(2) << totalVolumeTraded << std::endl;
    std::cout << "Active Users: " << portfolios.size() << std::endl;
    std::cout << "Active Order Books: " << orderBooks.size() << std::endl;
    
    // Show total orders in all books
    size_t totalOrders = 0;
    for (std::unordered_map<std::string, std::unique_ptr<OrderBook>>::const_iterator it = orderBooks.begin(); it != orderBooks.end(); ++it) {
        totalOrders += it->second->getTotalOrderCount();
    }
    std::cout << "Total Pending Orders: " << totalOrders << std::endl;
    
    displayMarketPrices();
}

// Display current market prices
void TradeBookingSystem::displayMarketPrices() {
    std::cout << "\nCurrent Market Prices:" << std::endl;
    for (const auto& pair : currentMarketPrices) {
    std::cout << "  " << pair.first << ": $" << std::fixed << std::setprecision(2) << pair.second << std::endl;
    }
}

// Process trade results
void TradeBookingSystem::processTradeResults(const std::vector<Trade>& trades) {
    if (trades.empty()) return;
    
    updatePortfoliosWithTrades(trades);
    updateSystemStatistics(trades);
    
    std::cout << "\n=== Trade Execution Summary ===" << std::endl;
    for (const auto& trade : trades) {
        std::cout << trade.toString() << std::endl;
    }
}

// Update portfolios with executed trades
void TradeBookingSystem::updatePortfoliosWithTrades(const std::vector<Trade>& trades) {
    for (const auto& trade : trades) {
        // Update buyer's portfolio
        auto buyerIt = portfolios.find(trade.buyUserId);
        if (buyerIt != portfolios.end()) {
            buyerIt->second->addTrade(trade, true); // true = buyer side
        }
        
        // Update seller's portfolio
        auto sellerIt = portfolios.find(trade.sellUserId);
        if (sellerIt != portfolios.end()) {
            sellerIt->second->addTrade(trade, false); // false = seller side
        }
    }
}

// Update system statistics
void TradeBookingSystem::updateSystemStatistics(const std::vector<Trade>& trades) {
    for (const auto& trade : trades) {
        totalTradesExecuted++;
        totalVolumeTraded += trade.quantity * trade.price;
    }
}

// Validation functions
bool TradeBookingSystem::validateSymbolInput(const std::string& symbol) {
    if (!isSymbolAvailable(symbol)) {
        std::cout << "Symbol '" << symbol << "' is not available for trading!" << std::endl;
        displayAvailableSymbols();
        return false;
    }
    return true;
}

bool TradeBookingSystem::validatePriceInput(double price) {
    if (price <= 0) {
        std::cout << "Price must be greater than 0!" << std::endl;
        return false;
    }
    return true;
}

bool TradeBookingSystem::validateQuantityInput(int quantity) {
    if (quantity <= 0) {
        std::cout << "Quantity must be greater than 0!" << std::endl;
        return false;
    }
    return true;
}

// Symbol management
bool TradeBookingSystem::isSymbolAvailable(const std::string& symbol) const {
    return std::find(availableSymbols.begin(), availableSymbols.end(), symbol) != availableSymbols.end();
}

void TradeBookingSystem::addSymbol(const std::string& symbol) {
    if (!isSymbolAvailable(symbol)) {
        availableSymbols.push_back(symbol);
        currentMarketPrices[symbol] = 100.0; // Default price
        std::cout << "Symbol " << symbol << " added to trading system" << std::endl;
    }
}

// Market data management
void TradeBookingSystem::updateMarketPrice(const std::string& symbol, double price) {
    if (price > 0) {
        currentMarketPrices[symbol] = price;
    }
}

double TradeBookingSystem::getMarketPrice(const std::string& symbol) const {
    auto it = currentMarketPrices.find(symbol);
    return (it != currentMarketPrices.end()) ? it->second : 0.0;
}

// Access functions
Portfolio* TradeBookingSystem::getPortfolio(const std::string& userId) {
    auto it = portfolios.find(userId);
    return (it != portfolios.end()) ? it->second.get() : nullptr;
}

const Portfolio* TradeBookingSystem::getPortfolio(const std::string& userId) const {
    auto it = portfolios.find(userId);
    return (it != portfolios.end()) ? it->second.get() : nullptr;
}

OrderBook* TradeBookingSystem::getOrderBook(const std::string& symbol) {
    auto it = orderBooks.find(symbol);
    return (it != orderBooks.end()) ? it->second.get() : nullptr;
}

const OrderBook* TradeBookingSystem::getOrderBook(const std::string& symbol) const {
    auto it = orderBooks.find(symbol);
    return (it != orderBooks.end()) ? it->second.get() : nullptr;
}

// Utility functions
void TradeBookingSystem::clearAllOrders() {
    for (auto& pair : orderBooks) {
    const std::string& symbol = pair.first;
    pair.second = std::make_unique<OrderBook>(symbol);
}
std::cout << "All orders cleared from system" << std::endl;
}

void TradeBookingSystem::clearOrdersForSymbol(const std::string& symbol) {
    auto it = orderBooks.find(symbol);
    if (it != orderBooks.end()) {
        it->second = std::make_unique<OrderBook>(symbol);
        std::cout << "Orders cleared for symbol " << symbol << std::endl;
    }
}

void TradeBookingSystem::resetSystem() {
    orderBooks.clear();
    portfolios.clear();
    totalTradesExecuted = 0;
    totalVolumeTraded = 0.0;
    std::cout << "System reset completed" << std::endl;
}