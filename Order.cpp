#include "Order.h"

// Initialize static member
int Order::nextOrderId = 1;

// Constructor
Order::Order(const std::string& sym, OrderSide s, int qty, double p, 
             const std::string& user, OrderType t)
    : orderId(nextOrderId++), symbol(sym), side(s), quantity(qty), 
      price(p), userId(user), type(t), 
      timestamp(std::chrono::system_clock::now()) {
}

// Copy constructor
Order::Order(const Order& other)
    : orderId(other.orderId), symbol(other.symbol), side(other.side),
      quantity(other.quantity), price(other.price), timestamp(other.timestamp),
      type(other.type), userId(other.userId) {
}

// Assignment operator
Order& Order::operator=(const Order& other) {
    if (this != &other) {
        orderId = other.orderId;
        symbol = other.symbol;
        side = other.side;
        quantity = other.quantity;
        price = other.price;
        timestamp = other.timestamp;
        type = other.type;
        userId = other.userId;
    }
    return *this;
}

// Validation
bool Order::isValid() const {
    return quantity > 0 && price > 0 && !symbol.empty() && !userId.empty();
}

// String representation
std::string Order::toString() const {
    return "Order[" + std::to_string(orderId) + "]: " + symbol + 
           " " + (side == OrderSide::BUY ? "BUY" : "SELL") + 
           " " + std::to_string(quantity) + "@" + std::to_string(price) +
           " User: " + userId;
}