#include "Trade.h"

// Initialize static member
int Trade::nextTradeId = 1;

// Constructor
Trade::Trade(const std::string& sym, int buyId, int sellId,
             const std::string& buyUser, const std::string& sellUser,
             int qty, double p)
    : tradeId(nextTradeId++), symbol(sym), buyOrderId(buyId), sellOrderId(sellId),
      buyUserId(buyUser), sellUserId(sellUser), quantity(qty), price(p),
      timestamp(std::chrono::system_clock::now()) {
}

// Copy constructor
Trade::Trade(const Trade& other)
    : tradeId(other.tradeId), symbol(other.symbol), buyOrderId(other.buyOrderId),
      sellOrderId(other.sellOrderId), buyUserId(other.buyUserId), 
      sellUserId(other.sellUserId), quantity(other.quantity), price(other.price),
      timestamp(other.timestamp) {
}

// Assignment operator
Trade& Trade::operator=(const Trade& other) {
    if (this != &other) {
        tradeId = other.tradeId;
        symbol = other.symbol;
        buyOrderId = other.buyOrderId;
        sellOrderId = other.sellOrderId;
        buyUserId = other.buyUserId;
        sellUserId = other.sellUserId;
        quantity = other.quantity;
        price = other.price;
        timestamp = other.timestamp;
    }
    return *this;
}

// String representation
std::string Trade::toString() const {
    return "Trade[" + std::to_string(tradeId) + "]: " + symbol + 
           " " + std::to_string(quantity) + "@" + std::to_string(price) +
           " Buyer: " + buyUserId + " Seller: " + sellUserId;
}