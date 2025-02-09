#include <sqlite3.h>
#include <string>
using namespace std;

struct MenuItem{
    int id;
    string name;
    double price;
    int stock;
};
struct Money{
    int id;
    int value;
    int quantity;
};
class VendingMachineDB{
private:
    sqlite3 *db;
    string student_id;
    string db_name;

public:
    VendingMachineDB(string name, string id) : db_name(name), student_id(id){
        if (sqlite3_open(db_name.c_str(), &db) != SQLITE_OK)
        {
            throw runtime_error("Error opening database: " + string(sqlite3_errmsg(db)));
        }
    };

    // clean up (close db )
    ~VendingMachineDB(){
        if (db)
        {
            sqlite3_close(db);
        }
    };

    void createStockTable(){
        string table_name = "stock_" + student_id;
        string sql = "CREATE TABLE IF NOT EXISTS " + table_name +
                     R"(
                    ( 
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    name TEXT NOT NULL,
                    price REAL NOT NULL,
                    stock INTEGER NOT NULL
                    )
                )";

        char *errMessage = nullptr;
        int res = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMessage);
        if (res != SQLITE_OK)
        {
            cerr << "SQL Error: " << errMessage << endl;
        }
    };

    void insertItem(const string name, double price, int stock){
        string table_name = "stock_" + student_id;
        string sql = "INSERT INTO " + table_name + "(name, price, stock) VALUES (?,?,?);";

        sqlite3_stmt *stmt; // where the prepared sql is stored
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 2, price);
            sqlite3_bind_int(stmt, 3, stock);

            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                std::cerr << "Error inserting item.\n";
            }
            sqlite3_finalize(stmt);
        }
        else
        {
            std::cerr << "Error preparing statement.\n";
        }
    }

    vector<MenuItem> getItems(){
        vector<MenuItem> items;
        string table_name = "stock_" + student_id;
        string sql = "SELECT * FROM " + table_name + ";";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                MenuItem item;
                item.id = sqlite3_column_int(stmt, 0);
                item.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                item.price = sqlite3_column_double(stmt, 2);
                item.stock = sqlite3_column_int(stmt, 3);

                items.push_back(item);
            }
            sqlite3_finalize(stmt);
        }
        else
        {
            std::cerr << "Error fetching items.\n";
        }

        return items;
    };

    MenuItem getItemById(int id){
        string table_name = "stock_" + student_id;
        string sql = "SELECT * FROM " + table_name + " WHERE id = ?;";
        sqlite3_stmt *stmt;

        MenuItem item = {0, "", 0.0, 0};

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, id);

            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                item.id = sqlite3_column_int(stmt, 0);
                item.name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
                item.price = sqlite3_column_double(stmt, 2);
                item.stock = sqlite3_column_int(stmt, 3);
            }
            else
            {
                std::cerr << "Item with ID " << id << " not found.\n";
            }

            sqlite3_finalize(stmt);
        }
        else
        {
            std::cerr << "Error preparing statement.\n";
        }

        return item;
    };

    void updateStockById(int id){
        string table_name = "stock_" + student_id;
        string sql = "UPDATE " + table_name + " SET stock = stock - 1 WHERE id = ? AND stock > 0;"; //only if it's > 0
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, id);

            if (sqlite3_step(stmt) == SQLITE_DONE)
            {
                cout << "Purchase successfully for item ID " << id << ".\n";
            }
            else
            {
                cerr << "Failed to update stock.\n";
            }

            sqlite3_finalize(stmt);
        }
        else
        {
            cerr << "Error preparing statement for stock update.\n";
        }
    }

   void updateItemStockById(int id, int quantityToAdd){
        string table_name = "stock_" + student_id;
        string sql = "UPDATE " + table_name + " SET stock = stock + ? WHERE id = ?;";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, quantityToAdd);
            sqlite3_bind_int(stmt, 2, id);

            if (sqlite3_step(stmt) == SQLITE_DONE)
            {
                cout << "Stock updated successfully for item ID " << id << ".\n";
            }
            else
            {
                cerr << "Failed to update stock.\n";
            }

            sqlite3_finalize(stmt);
        }
        else
        {
            cerr << "Error preparing statement for stock update.\n";
        }
    }

    // money
    void createCollectionBoxTable(){
        string table_name = "collections_" + student_id;
        string sql = "CREATE TABLE IF NOT EXISTS " + table_name + 
            R"(
                (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    value INTEGER NOT NULL,
                    quantity INTEGER NOT NULL CHECK(quantity <= 30)
                )
            )";
            char *errMessage = nullptr;
            int res = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMessage);
            if(res != SQLITE_OK){
                cerr << "Collection box SQL Error: " << errMessage << endl;
            }
    };

    void createChangesBoxTable(){
        string table_name = "changes_" + student_id;
        string sql = "CREATE TABLE IF NOT EXISTS " + table_name + 
            R"(
                (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    value INTEGER NOT NULL,
                    quantity INTEGER NOT NULL CHECK(quantity <= 30)
                )
            )";
            char *errMessage = nullptr;
            int res = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMessage);
            if(res != SQLITE_OK){
                cerr << "Collection box SQL Error: " << errMessage << endl;
            }
    };

    void insertToCollections(int value, int quantity){
        string table_name = "collections_" + student_id;
        string sql = "INSERT INTO " + table_name + "(value, quantity) VALUES (?,?);";
        sqlite3_stmt *stmt;
        
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK){
            sqlite3_bind_int(stmt, 1, value);
            sqlite3_bind_int(stmt, 2, quantity);

            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                std::cerr << "Error inserting item.\n";
            }
            sqlite3_finalize(stmt);
        }else{
            std::cerr << "Error preparing statement.\n";
        }
    }

    void insertToChangesBox(int value, int quantity){
        string table_name = "changes_" + student_id;
        string sql = "INSERT INTO " + table_name + "(value, quantity) VALUES (?,?);";
        sqlite3_stmt *stmt;
        
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK){
            sqlite3_bind_int(stmt, 1, value);
            sqlite3_bind_int(stmt, 2, quantity);

            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                std::cerr << "Error inserting item.\n";
            }
            sqlite3_finalize(stmt);
        }else{
            std::cerr << "Error preparing statement.\n";
        }
    }

    bool isAnyCoinQuantityAtLimit(){
        string table_name = "collections_" + student_id;
        string check_sql = "SELECT value, quantity FROM " + table_name + ";";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, check_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                int coinValue = sqlite3_column_int(stmt, 0); 
                int currentQuantity = sqlite3_column_int(stmt, 1);

                if (currentQuantity >= 30)
                {
                    // cout << "Coin " << coinValue << " THB has reached the limit of 30.\n";
                    sqlite3_finalize(stmt);
                    return true;
                }
            }
            sqlite3_finalize(stmt);
        }
        else
        {
            cerr << "Error querying coin quantities.\n";
        }

        return false;
    }

    bool isChangesCoinQuantityAtLimit(){
        string table_name = "changes_" + student_id;
        string check_sql = "SELECT value, quantity FROM " + table_name + ";";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, check_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                int coinValue = sqlite3_column_int(stmt, 0); 
                int currentQuantity = sqlite3_column_int(stmt, 1);

                if (currentQuantity <= 0 || currentQuantity > 30)
                {
                    sqlite3_finalize(stmt);
                    return true;
                }
            }
            sqlite3_finalize(stmt);
        }
        else
        {
            cerr << "Error querying coin quantities.\n";
        }

        return false;
    }

   void reduceChangesCoinQuantity(int coinValue) {
        string table_name = "changes_" + student_id;
        string sql = "UPDATE " + table_name + " SET quantity = quantity - 1 WHERE value = ? AND quantity > 0;";
        sqlite3_stmt *stmt;
        
        bool flag = false;
        flag = isChangesCoinQuantityAtLimit();
        
        if(flag) {
            cout << "Coin " << coinValue << " THB has reached the limit.\n";
            return;
        }
        
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, coinValue);
            
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "";
            } else {
                cerr << "Failed to update coin quantity for " << coinValue << " THB.\n";
            }
            
            sqlite3_finalize(stmt);
        } else {
            cerr << "Error preparing statement for coin quantity update.\n";
        }
    }
    
    // both user and admin
    void updateCoinQuantity(int coinValue, int quantityToAdd, string tableName){
        string table_name = tableName + student_id;
        string sql = "UPDATE " + table_name + " SET quantity = quantity + ? WHERE value = ?;";
        sqlite3_stmt *stmt;
        bool flag = false;
        bool changesFlag = false;

        flag = isAnyCoinQuantityAtLimit();
        changesFlag = isChangesCoinQuantityAtLimit();

        if(flag || changesFlag){
            return;
        }

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 2, coinValue);
            sqlite3_bind_int(stmt, 1, quantityToAdd); 

            if (sqlite3_step(stmt) == SQLITE_DONE)
            {
                cout << "Coin quantity updated successfully for " << coinValue << " THB.\n";
            }
            else
            {
                cerr << "Cannot update coin quantity for " << coinValue << " THB. A limit has already been reached.\n";
            }

            sqlite3_finalize(stmt);
        }
        else
        {
            cerr << "Error preparing statement for coin quantity update.\n";
        }
    }

     vector<Money> getCoins(string tableName){
        vector<Money> coins;
        string table_name = tableName + student_id;
        string sql = "SELECT * FROM " + table_name + ";";
        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                Money coin;
                coin.id = sqlite3_column_int(stmt, 0);
                coin.value = sqlite3_column_int(stmt, 1);
                coin.quantity = sqlite3_column_int(stmt, 2);

                coins.push_back(coin);
            }
            sqlite3_finalize(stmt);
        }
        else
        {
            std::cerr << "Error fetching coins.\n";
        }

        return coins;
    };

    void collectCollections() {
        string table_name = "collections_" + student_id;
        string sql = "UPDATE " + table_name + " SET quantity = 0 WHERE quantity > 0;";
        sqlite3_stmt *stmt;
        vector<Money> coins;
        coins = getCoins("collections_");
        double total = 0.0;

        for (const auto &coin : coins){
            total += ( coin.value * coin.quantity);
        };
            
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "Successfully Collected " << total << " THB.\n";
            } else {
                cerr << "Failed to update coin quantities to 0.\n";
            }
            
            sqlite3_finalize(stmt);
        } else {
            cerr << "Error preparing statement for updating coin quantities.\n";
        }
    }
};