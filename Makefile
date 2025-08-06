all:
	g++ -std=c++14 -Wall -Wextra -O2 -o trading_system \
		main.cpp \
		Order.cpp \
		Trade.cpp \
		OrderBook.cpp \
		Portfolio.cpp \
		MatchingEngine.cpp \
		TradeBookingSystem.cpp
