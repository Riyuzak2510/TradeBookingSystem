#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <chrono>
#include <iostream>

enum class OrderSide { BUY, SELL };
enum class OrderType { MARKET, LIMIT };

class Order {
private:
    static int nextOrderId;
    
public:
    int orderId;
    std::string symbol;
    OrderSide side;
    int quantity;
    double price;
    std::chrono::system_clock::time_point timestamp;
    OrderType type;
    std::string userId;
    
    // Constructor
    Order(const std::string& sym, OrderSide s, int qty, double p, 
          const std::string& user, OrderType t = OrderType::LIMIT);
    
    // Copy constructor
    Order(const Order& other);
    
    // Assignment operator
    Order& operator=(const Order& other);
    
    // Destructor
    ~Order() = default;
    
    // Validation
    bool isValid() const;
    
    // String representation
    std::string toString() const;
    
    // Getters
    int getOrderId() const { return orderId; }
    const std::string& getSymbol() const { return symbol; }
    OrderSide getSide() const { return side; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    const std::string& getUserId() const { return userId; }
    OrderType getType() const { return type; }
    
    // Setters
    void setQuantity(int qty) { quantity = qty; }
    void setPrice(double p) { price = p; }
    
    // Static method to get next order ID
    static int getNextOrderId() { return nextOrderId; }
};

#endif // ORDER_H