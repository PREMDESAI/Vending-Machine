# Vending Machine Management System
A C++ implementation of a vending machine management system with SQLite database integration. This project was created as part of my first-year second-semester midterm homework at KMITL University.

## Features
### User Mode (Customer)
- Browse available items with prices
- Select items using unique codes
- Process payments with multiple Thai Baht denominations:
(100 THB, 20 THB, 10 THB, 5 THB, 1 THB)
- Automatic change calculation and dispensing
- Real-time stock tracking
- Automatic service stop when:
  - 50% or more items are out of stock
  - Change box is empty
  - Collection box is full (even if one domination is full)
### Admin Mode
- Set initial stock levels
- Restock items
- Monitor money in change and collection boxes
- Collect money from collection box
- Refill change box
  
## Dependencies
- SQLite3
- C++ Standard Library

## Getting Started 
### Prerequisites
- C++ compiler
- SQLite3 library
### Installation
1. Clone the repository
```bash
git clone https://github.com/KyiThantSin/vending-machine.git
```
2. Navigate to the project directory
```bash
  cd vending_machine
```
3. Compile the project
```bash
  g++ -std=c++11 -o vending_machine main.cpp -lsqlite3
```
