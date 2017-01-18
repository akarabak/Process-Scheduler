all:	main.cpp
	g++ -std=gnu++11 main.cpp processManager.cpp resourceManager.cpp resources.cpp shell.cpp str_operations.cpp -o main
