all:
	g++ -g -Wall -Wextra -std=c++14 sendmail.cpp -lboost_system -lssl -lpthread -lcrypto
