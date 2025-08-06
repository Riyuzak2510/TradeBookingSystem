#ifndef TRADE_H
#define TRADE_H

#include <string>
#include <chrono>

class Trade {
private:
    static int nextTradeId;
    
public:
    int tradeId;
    std::string symbol;
    int buyOrderId;
    int sellOrderId;
    std::string buyUserId;
    std::string sellUserId;
    int quantity;
    double price;
    std::chrono::system_clock::time_point timestamp;
    
    // Constructor
    Trade(const std::string& sym, int buyId, int sellId, 
          const std::string& buyUser, const std::string& sellUser,
          int qty, double p);
    
    // Copy constructor
    Trade(const Trade& other);
    
    // Assignment operator
    Trade& operator=(const Trade& other);
    
    // Destructor
    ~Trade() = default;
    
    // String representation
    std::string toString() const;
    
    // Getters
    int getTradeId() const { return tradeId; }
    const std::string& getSymbol() const { return symbol; }
    int getBuyOrderId() const { return buyOrderId; }
    int getSellOrderId() const { return sellOrderId; }
    const std::string& getBuyUserId() const { return buyUserId; }
    const std::string& getSellUserId() const { return sellUserId; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp; }
    
    // Static method to get next trade ID
    static int getNextTradeId() { return nextTradeId; }
};

#endif // TRADE_H