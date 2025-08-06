# C++ Trade Booking System - Compilation Guide

## Project Structure
```
TradingSystem/
├── headers/
│   ├── Order.h
│   ├── Trade.h
│   ├── OrderBook.h
│   ├── Portfolio.h
│   ├── MatchingEngine.h
│   └── TradeBookingSystem.h
├── src/
│   ├── Order.cpp
│   ├── Trade.cpp
│   ├── OrderBook.cpp
│   ├── Portfolio.cpp
│   ├── MatchingEngine.cpp
│   └── TradeBookingSystem.cpp
├── main.cpp
├── Makefile
└── README.md
```

## Compilation Instructions

### Method 1: Using g++ directly
```bash
# Compile all files together
g++ -std=c++14 -Wall -Wextra -O2 -o trading_system \
    main.cpp \
    Order.cpp \
    Trade.cpp \
    OrderBook.cpp \
    Portfolio.cpp \
    MatchingEngine.cpp \
    TradeBookingSystem.cpp

# Run the system
./trading_system
```

### Method 2: Using Makefile (recommended)
Create a `Makefile`:

```makefile
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2
TARGET = trading_system
SOURCES = main.cpp Order.cpp Trade.cpp OrderBook.cpp Portfolio.cpp MatchingEngine.cpp TradeBookingSystem.cpp
OBJECTS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJECTS)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

clean:
    rm -f $(OBJECTS) $(TARGET)

.PHONY: clean
```

Then compile with:
```bash
make
./trading_system
```

### Method 3: Separate compilation
```bash
# Compile object files
g++ -std=c++14 -c Order.cpp -o Order.o
g++ -std=c++14 -c Trade.cpp -o Trade.o
g++ -std=c++14 -c OrderBook.cpp -o OrderBook.o
g++ -std=c++14 -c Portfolio.cpp -o Portfolio.o
g++ -std=c++14 -c MatchingEngine.cpp -o MatchingEngine.o
g++ -std=c++14 -c TradeBookingSystem.cpp -o TradeBookingSystem.o
g++ -std=c++14 -c main.cpp -o main.o

# Link everything
g++ -std=c++14 -o trading_system main.o Order.o Trade.o OrderBook.o Portfolio.o MatchingEngine.o TradeBookingSystem.o

# Run
./trading_system
```

## Dependencies
- C++14 compatible compiler (g++ 5.0+ or clang++ 3.4+)
- Standard Template Library (STL)

## Usage Example
```bash
$ ./trading_system
=== C++ Trade Booking System ===
Features:
- Multiple trading symbols support
- Buy/Sell orders (Long/Short positions)
- Real-time FIFO order matching
- Portfolio tracking and P&L calculation
- Complete trade history

=== Welcome to C++ Trade Booking System ===
Enter your user ID: trader1
New user account created for: trader1
Welcome, trader1!

=== Trade Booking System Menu ===
1. Place Order
2. Cancel Order
3. View Order Book
4. View Portfolio
5. View Available Symbols
6. View System Statistics
7. Exit
Enter your choice (1-7):
```

## File Dependencies
- `Order.h/.cpp` - Base order class (no dependencies)
- `Trade.h/.cpp` - Trade record class (no dependencies)  
- `OrderBook.h/.cpp` - Order book management (depends on Order)
- `Portfolio.h/.cpp` - Portfolio tracking (depends on Trade)
- `MatchingEngine.h/.cpp` - Order matching logic (depends on OrderBook, Trade)
- `TradeBookingSystem.h/.cpp` - Main system (depends on all above)
- `main.cpp` - Entry point (depends on TradeBookingSystem)

## Troubleshooting

### Common Compilation Errors:
1. **Missing C++17 support**: Update compiler or use `-std=c++11` (some features may not work)
2. **Linking errors**: Ensure all .cpp files are included in compilation
3. **Header not found**: Make sure all .h files are in the same directory or use `-I` flag

### Runtime Issues:
1. **Segmentation fault**: Usually indicates memory access issues, check shared_ptr usage
2. **Invalid orders**: System validates input, check error messages
3. **Portfolio discrepancies**: Verify trade processing logic in Portfolio class

This modular design makes the code maintainable and allows for easy testing of individual components.