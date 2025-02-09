#include <iostream>
#include <vector>
#include <iomanip>
#include "controller.h"
using namespace std;

class MoneyHandler{
private:
    vector<Money> coins;
public:
    MoneyHandler(VendingMachineDB &db){
        vector<Money> coinsList = {
            {0, 100, 0},
            {0, 20, 0},
            {0, 10, 0},
            {0, 5, 0},
            {0, 1, 0}};

        vector<Money> changesList = {
            {0, 100, 30},
            {0, 20, 30},
            {0, 10, 30},
            {0, 5, 30},
            {0, 1, 30}};

        for (const auto &coin : coinsList){
            db.insertToCollections(coin.value, coin.quantity);
        };
        for (const auto &coin : changesList){
            db.insertToChangesBox(coin.value, coin.quantity);
        };
    }
};
class Menu{
private:
    vector<MenuItem> items;

public:
    Menu(VendingMachineDB &db){
        vector<MenuItem> menu = {
            {0, "Coffee", 20.0, 10},
            {0, "Tea", 22.0, 10},
            {0, "Milk", 15.00, 10},
            {0, "Soda", 14.00, 10},
            {0, "Smoothies", 24.00, 10},
            {0, "Water", 8.00, 10}};

        for (const auto &item : menu){
            db.insertItem(item.name, item.price, item.stock);
        };

        items = db.getItems();
    }

    void display(){
        cout << setw(20) << "****Menu****" << endl;
        cout << setw(5) << "Code" << setw(20) << "Name" << setw(20) << "Price" << endl;
        if (items.empty())
        {
            cout << "No items available in the menu." << endl;
            return;
        }
        for (auto item : items)
        {
            cout << setw(5) << item.id << setw(20) << item.name << setw(20) << fixed << setprecision(2) << item.price << endl;
        }
    }

    bool isOutOfStock(){
        int totalItems = items.size();
        int outOfStockCount = 0;

        for (const auto &item : items)
        {
            if (item.stock <= 0)
            {
                outOfStockCount++;
            }
        }
        return (outOfStockCount >= (totalItems / 2.0));
    }
};
class Admin{
    string choice;
public:
    Admin(VendingMachineDB &db){
        cout << "***Admin***" << endl;
        cout << " \n (1).Set Initial Stock \n (2).Refill Stock \n (3).Refill Changes \n (4).Check Collection Box \n (5).Check Changes Box \n (6).Collect Money \n (7).Logout \n "<< endl;
        cout << "Enter your choice: ";
        cin >> choice;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if(choice == "1"){
            string name;
            double price;
            int stock;
            cout << "Enter the product name: ";
            getline(cin, name);
            cout << "Enter the price: ";
            cin >> price;
            cout << "Enter the stock: ";
            cin >> stock;

            addingNewStock(db, name, price, stock);
        }else if(choice == "2"){
            int selected_id;
            int amount;
            cout << "Enter the product Id: ";
            cin >> selected_id;

            cout << "Enter the stock (0-20): ";
            cin >> amount;

            while(amount < 0 || amount >220){
                cout << "You can't filled more than 20 quantity" << endl;
                cout << "Enter the quantity (0-30): ";
                cin >> amount;
            }

            db.updateItemStockById(selected_id, amount);
        }else if(choice == "3"){
            int selected_value;
            int quantity;
            cout << "Enter the Coin Value: ";
            cin >> selected_value;

            cout << "Enter the quantity (0-30): ";
            cin >> quantity;

            while(quantity < 0 || quantity >30){
                cout << "You can't filled more than 30 coins" << endl;
                cout << "Enter the quantity (0-30): ";
                cin >> quantity;
            }

            db.updateCoinQuantity(selected_value, quantity, "changes_");
        }else if(choice == "4"){
            vector<Money> coins;
            cout << "****Collections Box****" << endl;
            coins = db.getCoins("collections_");
            
            cout << setw(5) << "ID" << setw(20) << "Value(THB)" << setw(20) << "Quantity(Max 30)" << endl;
            
            for (const auto &coin : coins){
                cout << setw(5) << coin.id << setw(20) << coin.value << setw(20) << coin.quantity << endl;
            }      
        }else if(choice == "5"){
            vector<Money> coins;
            cout << "****Changes Box****" << endl;
            coins = db.getCoins("changes_");
            
            cout << setw(5) << "ID" << setw(20) << "Value(THB)" << setw(20) << "Quantity(Max 30)" << endl;
            
            for (const auto &coin : coins){
                cout << setw(5) << coin.id << setw(20) << coin.value << setw(20) << coin.quantity << endl;
            }   
        }
        else if(choice == "6"){
            db.collectCollections();
        }
        else if(choice == "7"){
            return;
        } else{
            cout << "Please enter a valid input." << endl;
        }
    }

    void addingNewStock(VendingMachineDB &db, const string &name, double price, int stock){
        db.insertItem(name, price, stock);
        cout << "New item added: " << name << " with price " << price << " THB and stock " << stock << endl;
    }
};

class User{
private:
    int slected_code;
    MenuItem item;
    int payment = 0;

public:
    User(VendingMachineDB &db){
        cout << "Enter a code number to choose the item: ";
        cin >> slected_code;
        cout << endl;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a valid input." << endl;
            return;
        }

        item = db.getItemById(slected_code);
        if (item.stock <= 0)
        {
            cout << "Sorry, Your selected item " << item.name << " is OUT OF STOCK." << endl;
            return;
        }
        cout << "Your selected Item: " << endl;
        cout << setw(5) << "Code" << setw(20) << "Name" << setw(20) << "Price ( THB ) " << endl;
        cout << setw(5) << item.id << setw(20) << item.name << setw(20) << item.price << endl;

        purchase(db);
    };

    bool isValidDenomination(int value)
    {
        vector<int> validDenominations = {100, 20, 10, 5, 1};
        for (int denom : validDenominations)
        {
            if (value == denom)
            {
                return true;
            }
        }
        return false;
    }

    void calculateAndDispenseChange(VendingMachineDB &db, double changeAmount){
        vector<int> denominations;

        if (changeAmount >= 100){
            denominations = {100, 20, 10, 5, 1};
        }
        else if (changeAmount >= 20){
            denominations = {20, 10, 5, 1};
        }
        else if(changeAmount >= 10){
            denominations = {10, 5, 1};
        }else{
            denominations = {5,1};
        }

        // calculate
        for (int denom : denominations)
        {
            while (changeAmount >= denom)
            {
                if (!db.isChangesCoinQuantityAtLimit()){
                    db.reduceChangesCoinQuantity(denom);
                    changeAmount -= denom;
                }
                else{
                    cout << "Unable to provide exact change. Transaction cancelled." << endl;
                    cout << "Returning original payment: " << payment << " THB" << endl;
                    return;
                }
            }
        }
    }

    void purchase(VendingMachineDB &db)
    {
        int amount = 0;

        while (amount < item.price)
        {
            cout << "Please enter the payment amount (100 THB, 20 THB, 10 THB, 5 THB, 1 THB): ";
            cin >> amount;

            bool flag = false;
            bool changesFlag = false;
            flag = db.isAnyCoinQuantityAtLimit();
            changesFlag = db.isChangesCoinQuantityAtLimit();

            if (flag)
            {
                cout << "--------------------------" << endl;
                cout << "Sorry, The collection box is fulled. We can't purchase the item at the moment." << endl;
                cout << "Please took your previous inserted Amount: " << amount << " THB" << endl;
                cout << "--------------------------" << endl;
                return;
            }
            else if (changesFlag)
            {
                cout << "--------------------------" << endl;
                cout << "Sorry, The Changes box is empty. We can't purchase the item at the moment." << endl;
                cout << "Please took your previous inserted Amount: " << amount << " THB" << endl;
                cout << "--------------------------" << endl;
                return;
            }

            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Please enter a valid input." << endl;
            }

            if (isValidDenomination(amount))
            {
                payment += amount;
                cout << "Payment accepted. Total payment so far: " << payment << " THB.\n";
                db.updateCoinQuantity(amount, 1, "collections_");

                if (payment >= item.price)
                {
                    cout << "Payment Succeess!. Change: " << payment - item.price << " THB" << endl;
                    db.updateStockById(item.id);
                    cout << "--------------------------" << endl;
                    double changeAmount = payment - item.price;
                    if (changeAmount > 0)
                    {
                        calculateAndDispenseChange(db, changeAmount);
                    }
                    break;
                }
                else
                {
                    cout << "You still need to pay " << item.price - payment << " THB" << endl;
                }
            }
            else
            {
                cout << "Please insert the valid amount (100 THB, 20 THB, 10 THB, 5 THB, 1 THB)." << endl;
            }
        }
    }
};

int main(){
    string choice;
    cout << "*****Vending Machine*****" << endl;

    // database create
    VendingMachineDB db("vendingMachine.db", "67011158");
    db.createStockTable();
    db.createCollectionBoxTable();
    db.createChangesBoxTable();
    Menu menu(db);
    MoneyHandler coins(db);

    while (true){
        cout << "Please choose a login mode \n (1).User (Selling Mode) \n (2).Admin \n (3).Exit \n Your choice: ";
        cin >> choice;

        if(choice == "1"){
            menu.display();
            User user(db);
            bool flag = false;

            flag = db.isAnyCoinQuantityAtLimit();

            if (flag){
                break;
            }
            if (menu.isOutOfStock()){
                cout << "--------------------------" << endl;
                cout << "Items are OUT OF STOCK at the moment." << endl;
                cout << "--------------------------" << endl;
                break;
            }
        }else if (choice == "2"){
            Admin admin(db);
        }else if (choice == "3"){
            cout << "Exiting the program. Thank you!\n";
            break;
        }else{
            cout << "Please enter a valid choice." << endl;
        }
    }

    return 0;
}
