#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iomanip>

using namespace std;

// --- DATA STRUCTURES ---

struct MenuItem {
    string name;
    double price;
    int prepTime;
    int salesCount;

    MenuItem() : price(0), prepTime(0), salesCount(0) {}
    MenuItem(const string& n, double p, int pt, int sc = 0)
        : name(n), price(p), prepTime(pt), salesCount(sc) {
    }
};

struct BSTNode {
    MenuItem data;
    BSTNode* left;
    BSTNode* right;

    BSTNode(const MenuItem& item) : data(item), left(nullptr), right(nullptr) {}
};

class MenuBST {
    BSTNode* root;

    BSTNode* insertRec(BSTNode* node, const MenuItem& item) {
        if (!node) return new BSTNode(item);
        if (item.name < node->data.name)
            node->left = insertRec(node->left, item);
        else if (item.name > node->data.name)
            node->right = insertRec(node->right, item);
        else {
            node->data.price = item.price;
            node->data.prepTime = item.prepTime;
        }
        return node;
    }

    BSTNode* findMin(BSTNode* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    BSTNode* deleteRec(BSTNode* node, const string& name) {
        if (!node) return nullptr;
        if (name < node->data.name)
            node->left = deleteRec(node->left, name);
        else if (name > node->data.name)
            node->right = deleteRec(node->right, name);
        else {
            if (!node->left && !node->right) {
                delete node;
                return nullptr;
            }
            if (!node->left) {
                BSTNode* temp = node->right;
                delete node;
                return temp;
            }
            if (!node->right) {
                BSTNode* temp = node->left;
                delete node;
                return temp;
            }
            BSTNode* successor = findMin(node->right);
            node->data = successor->data;
            node->right = deleteRec(node->right, successor->data.name);
        }
        return node;
    }

    BSTNode* searchRec(BSTNode* node, const string& name) const {
        if (!node) return nullptr;
        if (name == node->data.name) return node;
        if (name < node->data.name) return searchRec(node->left, name);
        return searchRec(node->right, name);
    }

    void inorderRec(BSTNode* node) const {
        if (!node) return;
        inorderRec(node->left);
        cout << left << setw(22) << node->data.name
            << setw(10) << fixed << setprecision(2) << node->data.price
            << setw(12) << node->data.prepTime << " min"
            << setw(10) << node->data.salesCount << endl;
        inorderRec(node->right);
    }

    void collectRec(BSTNode* node, vector<MenuItem>& items) const {
        if (!node) return;
        collectRec(node->left, items);
        items.push_back(node->data);
        collectRec(node->right, items);
    }

    void destroyRec(BSTNode* node) {
        if (!node) return;
        destroyRec(node->left);
        destroyRec(node->right);
        delete node;
    }

    void copyRec(BSTNode* src, BSTNode*& dest) {
        if (!src) { dest = nullptr; return; }
        dest = new BSTNode(src->data);
        copyRec(src->left, dest->left);
        copyRec(src->right, dest->right);
    }

public:
    MenuBST() : root(nullptr) {}

    MenuBST(const MenuBST& other) : root(nullptr) {
        copyRec(other.root, root);
    }

    MenuBST& operator=(const MenuBST& other) {
        if (this != &other) {
            destroyRec(root);
            root = nullptr;
            copyRec(other.root, root);
        }
        return *this;
    }

    ~MenuBST() { destroyRec(root); }

    void insert(const MenuItem& item) { root = insertRec(root, item); }

    bool remove(const string& name) {
        if (!search(name)) return false;
        root = deleteRec(root, name);
        return true;
    }

    MenuItem* search(const string& name) {
        BSTNode* node = searchRec(root, name);
        return node ? &node->data : nullptr;
    }

    const MenuItem* search(const string& name) const {
        BSTNode* node = searchRec(root, name);
        return node ? &node->data : nullptr;
    }

    void displayInOrder() const {
        if (!root) {
            cout << "  (Menu is empty)\n";
            return;
        }
        cout << left << setw(22) << "Item Name"
            << setw(10) << "Price"
            << setw(12) << "Prep Time"
            << setw(10) << "Sold" << endl;
        cout << string(54, '-') << endl;
        inorderRec(root);
    }

    void collectAll(vector<MenuItem>& items) const {
        items.clear();
        collectRec(root, items);
    }

    bool isEmpty() const { return root == nullptr; }
};

struct Order {
    int orderId;
    string customerName;
    string customerLocation;
    string restaurantName;
    string restaurantLocation;
    vector<string> itemNames;
    double totalPrice;
    int totalPrepTime;
    string status;

    Order() : orderId(0), totalPrice(0), totalPrepTime(0), status("pending") {}
};

struct OrderNode {
    Order data;
    OrderNode* next;

    OrderNode(const Order& o) : data(o), next(nullptr) {}
};

class OrderQueue {
    OrderNode* front;
    OrderNode* rear;
    int count;

public:
    OrderQueue() : front(nullptr), rear(nullptr), count(0) {}

    OrderQueue(const OrderQueue& other) : front(nullptr), rear(nullptr), count(0) {
        OrderNode* cur = other.front;
        while (cur) {
            enqueue(cur->data);
            cur = cur->next;
        }
    }

    OrderQueue& operator=(const OrderQueue& other) {
        if (this != &other) {
            clear();
            OrderNode* cur = other.front;
            while (cur) {
                enqueue(cur->data);
                cur = cur->next;
            }
        }
        return *this;
    }

    ~OrderQueue() { clear(); }

    void enqueue(const Order& o) {
        OrderNode* node = new OrderNode(o);
        if (!rear) {
            front = rear = node;
        }
        else {
            rear->next = node;
            rear = node;
        }
        count++;
    }

    bool dequeue(Order& out) {
        if (!front) return false;
        OrderNode* temp = front;
        out = front->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        count--;
        return true;
    }

    bool peek(Order& out) const {
        if (!front) return false;
        out = front->data;
        return true;
    }

    bool removeById(int orderId) {
        OrderNode* cur = front;
        OrderNode* prev = nullptr;
        while (cur) {
            if (cur->data.orderId == orderId) {
                if (prev) prev->next = cur->next;
                else front = cur->next;
                if (cur == rear) rear = prev;
                delete cur;
                count--;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    Order* findById(int orderId) {
        OrderNode* cur = front;
        while (cur) {
            if (cur->data.orderId == orderId) return &cur->data;
            cur = cur->next;
        }
        return nullptr;
    }

    int waitingTimeBefore(int orderId) const {
        int total = 0;
        OrderNode* cur = front;
        while (cur) {
            if (cur->data.orderId == orderId) break;
            total += cur->data.totalPrepTime;
            cur = cur->next;
        }
        return total;
    }

    int totalWaitingTime() const {
        int total = 0;
        OrderNode* cur = front;
        while (cur) {
            total += cur->data.totalPrepTime;
            cur = cur->next;
        }
        return total;
    }

    bool isEmpty() const { return front == nullptr; }
    int size() const { return count; }

    void displayAll() const {
        if (!front) {
            cout << "  (No orders)\n";
            return;
        }
        OrderNode* cur = front;
        int pos = 1;
        while (cur) {
            cout << "  [" << pos++ << "] Order #" << cur->data.orderId
                << " | Customer: " << cur->data.customerName
                << " | $" << fixed << setprecision(2) << cur->data.totalPrice
                << " | Prep: " << cur->data.totalPrepTime << " min"
                << " | Status: " << cur->data.status << endl;
            cout << "      Items: ";
            for (size_t i = 0; i < cur->data.itemNames.size(); i++) {
                if (i > 0) cout << ", ";
                cout << cur->data.itemNames[i];
            }
            cout << endl;
            cur = cur->next;
        }
    }

    void clear() {
        while (front) {
            OrderNode* temp = front;
            front = front->next;
            delete temp;
        }
        rear = nullptr;
        count = 0;
    }

    void toVector(vector<Order>& orders) const {
        orders.clear();
        OrderNode* cur = front;
        while (cur) {
            orders.push_back(cur->data);
            cur = cur->next;
        }
    }
};

struct HistoryNode {
    Order data;
    HistoryNode* next;

    HistoryNode(const Order& o) : data(o), next(nullptr) {}
};

class OrderStack {
    HistoryNode* top;
    int count;

public:
    OrderStack() : top(nullptr), count(0) {}

    OrderStack(const OrderStack& other) : top(nullptr), count(0) {
        vector<Order> temp;
        HistoryNode* cur = other.top;
        while (cur) {
            temp.push_back(cur->data);
            cur = cur->next;
        }
        for (int i = (int)temp.size() - 1; i >= 0; i--)
            push(temp[i]);
    }

    OrderStack& operator=(const OrderStack& other) {
        if (this != &other) {
            clear();
            vector<Order> temp;
            HistoryNode* cur = other.top;
            while (cur) {
                temp.push_back(cur->data);
                cur = cur->next;
            }
            for (int i = (int)temp.size() - 1; i >= 0; i--)
                push(temp[i]);
        }
        return *this;
    }

    ~OrderStack() { clear(); }

    void push(const Order& o) {
        HistoryNode* node = new HistoryNode(o);
        node->next = top;
        top = node;
        count++;
    }

    bool pop(Order& out) {
        if (!top) return false;
        HistoryNode* temp = top;
        out = top->data;
        top = top->next;
        delete temp;
        count--;
        return true;
    }

    bool isEmpty() const { return top == nullptr; }
    int size() const { return count; }

    void displayAll() const {
        if (!top) {
            cout << "  (No history)\n";
            return;
        }
        HistoryNode* cur = top;
        int pos = 1;
        while (cur) {
            cout << "  [" << pos++ << "] Order #" << cur->data.orderId
                << " | " << cur->data.restaurantName
                << " | $" << fixed << setprecision(2) << cur->data.totalPrice
                << " | Status: " << cur->data.status << endl;
            cout << "      Items: ";
            for (size_t i = 0; i < cur->data.itemNames.size(); i++) {
                if (i > 0) cout << ", ";
                cout << cur->data.itemNames[i];
            }
            cout << endl;
            cur = cur->next;
        }
    }

    void clear() {
        while (top) {
            HistoryNode* temp = top;
            top = top->next;
            delete temp;
        }
        count = 0;
    }

    void toVector(vector<Order>& orders) const {
        orders.clear();
        HistoryNode* cur = top;
        while (cur) {
            orders.push_back(cur->data);
            cur = cur->next;
        }
    }
};

class MaxHeap {
    vector<MenuItem> heap;

    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx].salesCount <= heap[parent].salesCount) break;
            MenuItem temp = heap[idx];
            heap[idx] = heap[parent];
            heap[parent] = temp;
            idx = parent;
        }
    }

    void heapifyDown(int idx) {
        int n = (int)heap.size();
        while (true) {
            int largest = idx;
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            if (left < n && heap[left].salesCount > heap[largest].salesCount)
                largest = left;
            if (right < n && heap[right].salesCount > heap[largest].salesCount)
                largest = right;
            if (largest == idx) break;
            MenuItem temp = heap[idx];
            heap[idx] = heap[largest];
            heap[largest] = temp;
            idx = largest;
        }
    }

public:
    void buildFromItems(const vector<MenuItem>& items) {
        heap = items;
        if (heap.empty()) return;
        for (int i = (int)heap.size() / 2 - 1; i >= 0; i--)
            heapifyDown(i);
    }

    bool isEmpty() const { return heap.empty(); }

    MenuItem extractMax() {
        MenuItem maxItem = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) heapifyDown(0);
        return maxItem;
    }

    int size() const { return (int)heap.size(); }
};

// --- CLASSES ---

string trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r'))
        start++;
    size_t end = s.size();
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r'))
        end--;
    return s.substr(start, end - start);
}

vector<string> split(const string& s, char delim) {
    vector<string> parts;
    stringstream ss(s);
    string part;
    while (getline(ss, part, delim)) {
        part = trim(part);
        parts.push_back(part);
    }
    return parts;
}

string joinItems(const vector<string>& items) {
    string result;
    for (size_t i = 0; i < items.size(); i++) {
        if (i > 0) result += ",";
        result += items[i];
    }
    return result;
}

vector<string> parseItems(const string& s) {
    vector<string> items;
    if (s.empty()) return items;
    stringstream ss(s);
    string part;
    while (getline(ss, part, ',')) {
        part = trim(part);
        if (!part.empty()) items.push_back(part);
    }
    return items;
}

int nextOrderId = 1;

class Restaurant {
public:
    string ownerName;
    string ownerPassword;
    string name;
    string location;
    MenuBST menu;
    OrderQueue pendingOrders;
    double totalRevenue;
    int totalOrdersPlaced;

    Restaurant() : totalRevenue(0), totalOrdersPlaced(0) {}

    Restaurant(const string& owner, const string& pw, const string& rname, const string& loc)
        : ownerName(owner), ownerPassword(pw), name(rname), location(loc),
        totalRevenue(0), totalOrdersPlaced(0) {
    }
};

class Customer {
public:
    string name;
    string password;
    string location;
    OrderStack orderHistory;

    Customer() {}
    Customer(const string& n, const string& pw, const string& loc)
        : name(n), password(pw), location(loc) {
    }
};

class Rider {
public:
    string name;
    string password;
    string location;
    OrderQueue assignedDeliveries;
    OrderStack deliveryHistory;

    Rider() {}
    Rider(const string& n, const string& pw, const string& loc)
        : name(n), password(pw), location(loc) {
    }
};

class Application {
    vector<Customer> customers;
    vector<Rider> riders;
    vector<Restaurant> restaurants;

    Customer* loggedInCustomer;
    Rider* loggedInRider;
    Restaurant* loggedInRestaurant;

    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void pause() {
        cout << "\nPress Enter to continue...";
        cin.ignore(10000, '\n');
    }

    int readInt(const string& prompt) {
        int val;
        while (true) {
            cout << prompt;
            if (cin >> val) {
                cin.ignore(10000, '\n');
                return val;
            }
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Enter a number.\n";
        }
    }

    double readDouble(const string& prompt) {
        double val;
        while (true) {
            cout << prompt;
            if (cin >> val) {
                cin.ignore(10000, '\n');
                return val;
            }
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Enter a number.\n";
        }
    }

    string readLine(const string& prompt) {
        cout << prompt;
        string line;
        getline(cin, line);
        return trim(line);
    }

    Customer* findCustomer(const string& name) {
        for (size_t i = 0; i < customers.size(); i++)
            if (customers[i].name == name) return &customers[i];
        return nullptr;
    }

    Rider* findRider(const string& name) {
        for (size_t i = 0; i < riders.size(); i++)
            if (riders[i].name == name) return &riders[i];
        return nullptr;
    }

    Restaurant* findRestaurant(const string& name) {
        for (size_t i = 0; i < restaurants.size(); i++)
            if (restaurants[i].name == name) return &restaurants[i];
        return nullptr;
    }

    Restaurant* findRestaurantByOwner(const string& owner) {
        for (size_t i = 0; i < restaurants.size(); i++)
            if (restaurants[i].ownerName == owner) return &restaurants[i];
        return nullptr;
    }

    Rider* selectRiderForDelivery() {
        if (riders.empty()) return nullptr;
        Rider* best = &riders[0];
        for (size_t i = 1; i < riders.size(); i++) {
            if (riders[i].assignedDeliveries.size() < best->assignedDeliveries.size())
                best = &riders[i];
        }
        return best;
    }

    // --- FILE HANDLING ---

    void saveUsers() {
        ofstream file("users.txt");
        if (!file) return;
        for (size_t i = 0; i < customers.size(); i++)
            file << "CUSTOMER|" << customers[i].name << "|"
            << customers[i].password << "|" << customers[i].location << "\n";
        for (size_t i = 0; i < riders.size(); i++)
            file << "RIDER|" << riders[i].name << "|"
            << riders[i].password << "|" << riders[i].location << "\n";
        for (size_t i = 0; i < restaurants.size(); i++)
            file << "OWNER|" << restaurants[i].ownerName << "|"
            << restaurants[i].ownerPassword << "|" << restaurants[i].location << "|"
            << restaurants[i].name << "|"
            << restaurants[i].totalRevenue << "|"
            << restaurants[i].totalOrdersPlaced << "\n";
        file.close();
    }

    void saveMenus() {
        ofstream file("menu.txt");
        if (!file) return;
        for (size_t i = 0; i < restaurants.size(); i++) {
            vector<MenuItem> items;
            restaurants[i].menu.collectAll(items);
            for (size_t j = 0; j < items.size(); j++)
                file << restaurants[i].name << "|" << items[j].name << "|"
                << items[j].price << "|" << items[j].prepTime << "|"
                << items[j].salesCount << "\n";
        }
        file.close();
    }

    void saveOrders() {
        ofstream file("orders.txt");
        if (!file) return;
        for (size_t i = 0; i < restaurants.size(); i++) {
            vector<Order> orders;
            restaurants[i].pendingOrders.toVector(orders);
            for (size_t j = 0; j < orders.size(); j++) {
                Order& o = orders[j];
                file << "RESTAURANT|" << restaurants[i].name << "|" << o.orderId << "|"
                    << o.customerName << "|" << o.customerLocation << "|"
                    << o.restaurantLocation << "|" << joinItems(o.itemNames) << "|"
                    << o.totalPrice << "|" << o.totalPrepTime << "|" << o.status << "\n";
            }
        }
        for (size_t i = 0; i < riders.size(); i++) {
            vector<Order> orders;
            riders[i].assignedDeliveries.toVector(orders);
            for (size_t j = 0; j < orders.size(); j++) {
                Order& o = orders[j];
                file << "RIDER|" << riders[i].name << "|" << o.orderId << "|"
                    << o.customerName << "|" << o.customerLocation << "|"
                    << o.restaurantName << "|" << o.restaurantLocation << "|"
                    << joinItems(o.itemNames) << "|"
                    << o.totalPrice << "|" << o.totalPrepTime << "|" << o.status << "\n";
            }
        }
        file.close();
    }

    void saveHistory() {
        ofstream file("history.txt");
        if (!file) return;
        for (size_t i = 0; i < customers.size(); i++) {
            vector<Order> orders;
            customers[i].orderHistory.toVector(orders);
            for (size_t j = 0; j < orders.size(); j++) {
                Order& o = orders[j];
                file << "CUSTOMER|" << customers[i].name << "|" << o.orderId << "|"
                    << o.restaurantName << "|" << o.restaurantLocation << "|"
                    << joinItems(o.itemNames) << "|" << o.totalPrice << "|"
                    << o.totalPrepTime << "|" << o.status << "\n";
            }
        }
        for (size_t i = 0; i < riders.size(); i++) {
            vector<Order> orders;
            riders[i].deliveryHistory.toVector(orders);
            for (size_t j = 0; j < orders.size(); j++) {
                Order& o = orders[j];
                file << "RIDER|" << riders[i].name << "|" << o.orderId << "|"
                    << o.restaurantName << "|" << o.restaurantLocation << "|"
                    << o.customerName << "|" << o.customerLocation << "|"
                    << joinItems(o.itemNames) << "|" << o.totalPrice << "|"
                    << o.status << "\n";
            }
        }
        file.close();
    }

    void saveAll() {
        saveUsers();
        saveMenus();
        saveOrders();
        saveHistory();
    }

    void loadUsers() {
        ifstream file("users.txt");
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<string> p = split(line, '|');
            if (p.size() < 4) continue;
            if (p[0] == "CUSTOMER") {
                Customer c(p[1], p[2], p[3]);
                customers.push_back(c);
            }
            else if (p[0] == "RIDER") {
                Rider r(p[1], p[2], p[3]);
                riders.push_back(r);
            }
            else if (p[0] == "OWNER" && p.size() >= 5) {
                Restaurant rest(p[1], p[2], p[4], p[3]);
                if (p.size() >= 7) {
                    rest.totalRevenue = atof(p[5].c_str());
                    rest.totalOrdersPlaced = atoi(p[6].c_str());
                }
                restaurants.push_back(rest);
            }
        }
        file.close();
    }

    void loadMenus() {
        ifstream file("menu.txt");
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<string> p = split(line, '|');
            if (p.size() < 5) continue;
            Restaurant* r = findRestaurant(p[0]);
            if (!r) continue;
            MenuItem item(p[1], atof(p[2].c_str()), atoi(p[3].c_str()), atoi(p[4].c_str()));
            r->menu.insert(item);
        }
        file.close();
    }

    void loadOrders() {
        ifstream file("orders.txt");
        if (!file) return;
        string line;
        int maxId = 0;
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<string> p = split(line, '|');
            Order o;
            o.orderId = atoi(p[2].c_str());
            if (o.orderId > maxId) maxId = o.orderId;
            o.customerName = p[3];
            o.customerLocation = p[4];

            if (p[0] == "RESTAURANT" && p.size() >= 10) {
                o.restaurantName = p[1];
                o.restaurantLocation = p[5];
                o.itemNames = parseItems(p[6]);
                o.totalPrice = atof(p[7].c_str());
                o.totalPrepTime = atoi(p[8].c_str());
                o.status = p[9];
                Restaurant* r = findRestaurant(p[1]);
                if (r) r->pendingOrders.enqueue(o);
            }
            else if (p[0] == "RIDER" && p.size() >= 11) {
                o.restaurantName = p[5];
                o.restaurantLocation = p[6];
                o.itemNames = parseItems(p[7]);
                o.totalPrice = atof(p[8].c_str());
                o.totalPrepTime = atoi(p[9].c_str());
                o.status = p[10];
                Rider* rider = findRider(p[1]);
                if (rider) rider->assignedDeliveries.enqueue(o);
            }
        }
        file.close();
        nextOrderId = maxId + 1;
    }

    void loadHistory() {
        ifstream file("history.txt");
        if (!file) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<string> p = split(line, '|');
            Order o;
            if (p[0] == "CUSTOMER" && p.size() >= 8) {
                o.orderId = atoi(p[2].c_str());
                o.restaurantName = p[3];
                o.restaurantLocation = p[4];
                o.itemNames = parseItems(p[5]);
                o.totalPrice = atof(p[6].c_str());
                o.totalPrepTime = atoi(p[7].c_str());
                o.status = p.size() > 8 ? p[8] : "delivered";
                Customer* c = findCustomer(p[1]);
                if (c) c->orderHistory.push(o);
            }
            else if (p[0] == "RIDER" && p.size() >= 9) {
                o.orderId = atoi(p[2].c_str());
                o.restaurantName = p[3];
                o.restaurantLocation = p[4];
                o.customerName = p[5];
                o.customerLocation = p[6];
                o.itemNames = parseItems(p[7]);
                o.totalPrice = atof(p[8].c_str());
                o.status = p.size() > 9 ? p[9] : "delivered";
                Rider* r = findRider(p[1]);
                if (r) r->deliveryHistory.push(o);
            }
        }
        file.close();
    }

    void loadAll() {
        loadUsers();
        loadMenus();
        loadOrders();
        loadHistory();
    }

    // --- CUSTOMER MENUS ---

    void customerSignUp() {
        clearScreen();
        cout << "=== Customer Sign Up ===\n\n";
        string name = readLine("Enter name: ");
        if (findCustomer(name)) {
            cout << "Customer already exists.\n";
            pause();
            return;
        }
        string password = readLine("Enter password: ");
        string location = readLine("Enter your location: ");
        customers.push_back(Customer(name, password, location));
        saveAll();
        cout << "Account created successfully!\n";
        pause();
    }

    void customerLogin() {
        clearScreen();
        cout << "=== Customer Login ===\n\n";
        string name = readLine("Enter name: ");
        string password = readLine("Enter password: ");
        Customer* c = findCustomer(name);
        if (!c || c->password != password) {
            cout << "Invalid credentials.\n";
            pause();
            return;
        }
        loggedInCustomer = c;
        cout << "Welcome, " << c->name << "!\n";
        pause();
        customerMenu();
    }

    void customerViewRestaurants() {
        clearScreen();
        cout << "=== Available Restaurants ===\n\n";
        if (restaurants.empty()) {
            cout << "No restaurants available.\n";
            pause();
            return;
        }
        for (size_t i = 0; i < restaurants.size(); i++)
            cout << "  [" << (i + 1) << "] " << restaurants[i].name
            << " @ " << restaurants[i].location << endl;
        pause();
    }

    void customerViewMenu() {
        clearScreen();
        cout << "=== View Restaurant Menu ===\n\n";
        if (restaurants.empty()) {
            cout << "No restaurants available.\n";
            pause();
            return;
        }
        for (size_t i = 0; i < restaurants.size(); i++)
            cout << "  [" << (i + 1) << "] " << restaurants[i].name << endl;
        int choice = readInt("\nSelect restaurant: ");
        if (choice < 1 || choice >(int)restaurants.size()) {
            cout << "Invalid selection.\n";
            pause();
            return;
        }
        Restaurant& r = restaurants[choice - 1];
        cout << "\n--- Menu: " << r.name << " ---\n\n";
        r.menu.displayInOrder();
        pause();
    }

    void customerSearchItem() {
        clearScreen();
        cout << "=== Search Food Item ===\n\n";
        if (restaurants.empty()) {
            cout << "No restaurants available.\n";
            pause();
            return;
        }
        string query = readLine("Enter item name to search: ");
        bool found = false;
        for (size_t i = 0; i < restaurants.size(); i++) {
            const MenuItem* item = restaurants[i].menu.search(query);
            if (item) {
                if (!found) {
                    cout << "\nResults:\n";
                    cout << left << setw(20) << "Restaurant"
                        << setw(22) << "Item"
                        << setw(10) << "Price"
                        << setw(12) << "Prep" << endl;
                    cout << string(64, '-') << endl;
                    found = true;
                }
                cout << left << setw(20) << restaurants[i].name
                    << setw(22) << item->name
                    << setw(10) << fixed << setprecision(2) << item->price
                    << setw(12) << item->prepTime << " min" << endl;
            }
        }
        if (!found) cout << "Item not found in any restaurant.\n";
        pause();
    }

    void customerPlaceOrder() {
        clearScreen();
        cout << "=== Place Order ===\n\n";
        if (restaurants.empty()) {
            cout << "No restaurants available.\n";
            pause();
            return;
        }
        for (size_t i = 0; i < restaurants.size(); i++)
            cout << "  [" << (i + 1) << "] " << restaurants[i].name << endl;
        int choice = readInt("\nSelect restaurant: ");
        if (choice < 1 || choice >(int)restaurants.size()) {
            cout << "Invalid selection.\n";
            pause();
            return;
        }
        Restaurant& r = restaurants[choice - 1];
        cout << "\n--- Menu: " << r.name << " ---\n\n";
        r.menu.displayInOrder();

        vector<string> selectedItems;
        double totalPrice = 0;
        int totalPrep = 0;

        while (true) {
            string itemName = readLine("\nEnter item name (or 'done' to finish): ");
            if (itemName == "done" || itemName == "DONE") break;
            MenuItem* item = r.menu.search(itemName);
            if (!item) {
                cout << "Item not found. Try again.\n";
                continue;
            }
            selectedItems.push_back(item->name);
            totalPrice += item->price;
            totalPrep += item->prepTime;
            item->salesCount++;
            cout << "Added: " << item->name << " ($" << fixed << setprecision(2)
                << item->price << ")\n";
        }

        if (selectedItems.empty()) {
            cout << "No items selected. Order cancelled.\n";
            pause();
            return;
        }

        Order order;
        order.orderId = nextOrderId++;
        order.customerName = loggedInCustomer->name;
        order.customerLocation = loggedInCustomer->location;
        order.restaurantName = r.name;
        order.restaurantLocation = r.location;
        order.itemNames = selectedItems;
        order.totalPrice = totalPrice;
        order.totalPrepTime = totalPrep;
        order.status = "pending";

        r.pendingOrders.enqueue(order);
        r.totalOrdersPlaced++;
        saveAll();

        cout << "\nOrder #" << order.orderId << " placed successfully!\n";
        cout << "Total: $" << fixed << setprecision(2) << totalPrice
            << " | Estimated prep: " << totalPrep << " min\n";
        pause();
    }

    void customerCancelOrder() {
        clearScreen();
        cout << "=== Cancel Pending Order ===\n\n";
        vector<Order> myOrders;
        for (size_t i = 0; i < restaurants.size(); i++) {
            vector<Order> all;
            restaurants[i].pendingOrders.toVector(all);
            for (size_t j = 0; j < all.size(); j++) {
                if (all[j].customerName == loggedInCustomer->name &&
                    all[j].status == "pending")
                    myOrders.push_back(all[j]);
            }
        }
        if (myOrders.empty()) {
            cout << "No pending orders to cancel.\n";
            pause();
            return;
        }
        for (size_t i = 0; i < myOrders.size(); i++)
            cout << "  [" << (i + 1) << "] Order #" << myOrders[i].orderId
            << " @ " << myOrders[i].restaurantName
            << " ($" << fixed << setprecision(2) << myOrders[i].totalPrice << ")\n";
        int choice = readInt("\nSelect order to cancel: ");
        if (choice < 1 || choice >(int)myOrders.size()) {
            cout << "Invalid selection.\n";
            pause();
            return;
        }
        int orderId = myOrders[choice - 1].orderId;
        string restName = myOrders[choice - 1].restaurantName;
        Restaurant* r = findRestaurant(restName);
        if (r && r->pendingOrders.removeById(orderId)) {
            saveAll();
            cout << "Order #" << orderId << " cancelled.\n";
        }
        else {
            cout << "Could not cancel order.\n";
        }
        pause();
    }

    void customerViewWaitingTime() {
        clearScreen();
        cout << "=== Order Waiting Time ===\n\n";
        bool found = false;
        for (size_t i = 0; i < restaurants.size(); i++) {
            vector<Order> all;
            restaurants[i].pendingOrders.toVector(all);
            for (size_t j = 0; j < all.size(); j++) {
                if (all[j].customerName == loggedInCustomer->name) {
                    if (!found) found = true;
                    int wait = restaurants[i].pendingOrders.waitingTimeBefore(all[j].orderId);
                    int myPrep = all[j].totalPrepTime;
                    cout << "Order #" << all[j].orderId << " @ " << all[j].restaurantName << ":\n";
                    cout << "  Queue wait: " << wait << " min\n";
                    cout << "  Your prep:  " << myPrep << " min\n";
                    cout << "  Total est.:  " << (wait + myPrep) << " min\n\n";
                }
            }
        }
        if (!found) cout << "No active orders found.\n";
        pause();
    }

    void customerViewHistory() {
        clearScreen();
        cout << "=== Order History ===\n\n";
        loggedInCustomer->orderHistory.displayAll();
        pause();
    }

    void customerMenu() {
        while (loggedInCustomer) {
            clearScreen();
            cout << "=== Customer Portal ===\n";
            cout << "Logged in as: " << loggedInCustomer->name
                << " @ " << loggedInCustomer->location << "\n\n";
            cout << "  1. View Restaurants\n";
            cout << "  2. View Menu\n";
            cout << "  3. Search Food Item\n";
            cout << "  4. Place Order\n";
            cout << "  5. Cancel Pending Order\n";
            cout << "  6. View Waiting Time\n";
            cout << "  7. View Order History\n";
            cout << "  0. Logout\n";
            int choice = readInt("\nChoice: ");
            switch (choice) {
            case 1: customerViewRestaurants(); break;
            case 2: customerViewMenu(); break;
            case 3: customerSearchItem(); break;
            case 4: customerPlaceOrder(); break;
            case 5: customerCancelOrder(); break;
            case 6: customerViewWaitingTime(); break;
            case 7: customerViewHistory(); break;
            case 0: loggedInCustomer = nullptr; break;
            default: cout << "Invalid choice.\n"; pause(); break;
            }
        }
    }

    // --- RESTAURANT OWNER MENUS ---

    void ownerSignUp() {
        clearScreen();
        cout << "=== Restaurant Owner Sign Up ===\n\n";
        string name = readLine("Enter owner name: ");
        if (findRestaurantByOwner(name)) {
            cout << "Owner already exists.\n";
            pause();
            return;
        }
        string password = readLine("Enter password: ");
        string location = readLine("Enter restaurant location: ");
        string restName = readLine("Enter restaurant name: ");
        if (findRestaurant(restName)) {
            cout << "Restaurant name already taken.\n";
            pause();
            return;
        }
        Restaurant r(name, password, restName, location);
        restaurants.push_back(r);
        saveAll();
        cout << "Restaurant registered successfully!\n";
        pause();
    }

    void ownerLogin() {
        clearScreen();
        cout << "=== Restaurant Owner Login ===\n\n";
        string name = readLine("Enter owner name: ");
        string password = readLine("Enter password: ");
        Restaurant* r = findRestaurantByOwner(name);
        if (!r || r->ownerPassword != password) {
            cout << "Invalid credentials.\n";
            pause();
            return;
        }
        loggedInRestaurant = r;
        cout << "Welcome, " << r->ownerName << "! Managing: " << r->name << "\n";
        pause();
        ownerMenu();
    }

    void ownerAddMenuItem() {
        clearScreen();
        cout << "=== Add Menu Item ===\n\n";
        string name = readLine("Item name: ");
        if (loggedInRestaurant->menu.search(name)) {
            cout << "Item already exists.\n";
            pause();
            return;
        }
        double price = readDouble("Price: $");
        int prep = readInt("Prep time (minutes): ");
        MenuItem item(name, price, prep, 0);
        loggedInRestaurant->menu.insert(item);
        saveAll();
        cout << "Item added.\n";
        pause();
    }

    void ownerUpdateMenuItem() {
        clearScreen();
        cout << "=== Update Menu Item ===\n\n";
        loggedInRestaurant->menu.displayInOrder();
        string name = readLine("\nEnter item name to update: ");
        MenuItem* item = loggedInRestaurant->menu.search(name);
        if (!item) {
            cout << "Item not found.\n";
            pause();
            return;
        }
        double price = readDouble("New price: $");
        int prep = readInt("New prep time (minutes): ");
        int sales = item->salesCount;
        loggedInRestaurant->menu.remove(name);
        loggedInRestaurant->menu.insert(MenuItem(name, price, prep, sales));
        saveAll();
        cout << "Item updated.\n";
        pause();
    }

    void ownerDeleteMenuItem() {
        clearScreen();
        cout << "=== Delete Menu Item ===\n\n";
        loggedInRestaurant->menu.displayInOrder();
        string name = readLine("\nEnter item name to delete: ");
        if (loggedInRestaurant->menu.remove(name)) {
            saveAll();
            cout << "Item deleted.\n";
        }
        else {
            cout << "Item not found.\n";
        }
        pause();
    }

    void ownerProcessOrders() {
        clearScreen();
        cout << "=== Process Pending Orders ===\n\n";
        if (riders.empty()) {
            cout << "No riders available. Cannot process orders.\n";
            pause();
            return;
        }
        if (loggedInRestaurant->pendingOrders.isEmpty()) {
            cout << "No pending orders.\n";
            pause();
            return;
        }
        loggedInRestaurant->pendingOrders.displayAll();
        int choice = readInt("\nProcess first order? (1=Yes, 0=No): ");
        if (choice != 1) return;

        Order o;
        if (!loggedInRestaurant->pendingOrders.dequeue(o)) {
            cout << "No orders to process.\n";
            pause();
            return;
        }
        o.status = "assigned";
        o.restaurantName = loggedInRestaurant->name;
        o.restaurantLocation = loggedInRestaurant->location;
        loggedInRestaurant->totalRevenue += o.totalPrice;

        Rider* rider = selectRiderForDelivery();
        rider->assignedDeliveries.enqueue(o);
        saveAll();

        cout << "\nOrder #" << o.orderId << " assigned to rider: " << rider->name << "\n";
        pause();
    }

    void ownerViewAnalytics() {
        clearScreen();
        cout << "=== Restaurant Analytics ===\n\n";
        cout << "Restaurant: " << loggedInRestaurant->name << "\n";
        cout << "Total Revenue:    $" << fixed << setprecision(2)
            << loggedInRestaurant->totalRevenue << "\n";
        cout << "Total Orders:     " << loggedInRestaurant->totalOrdersPlaced << "\n";
        cout << "Pending Orders:   " << loggedInRestaurant->pendingOrders.size() << "\n\n";

        vector<MenuItem> items;
        loggedInRestaurant->menu.collectAll(items);
        if (items.empty()) {
            cout << "No menu items for sales report.\n";
            pause();
            return;
        }

        MaxHeap heap;
        heap.buildFromItems(items);

        cout << "--- Most Sold Items (Max Heap Report) ---\n";
        cout << left << setw(22) << "Item" << setw(10) << "Sold" << endl;
        cout << string(32, '-') << endl;
        int reportCount = heap.size();
        for (int i = 0; i < reportCount; i++) {
            MenuItem top = heap.extractMax();
            cout << left << setw(22) << top.name << setw(10) << top.salesCount << endl;
        }
        pause();
    }

    void ownerMenu() {
        while (loggedInRestaurant) {
            clearScreen();
            cout << "=== Restaurant Owner Portal ===\n";
            cout << "Owner: " << loggedInRestaurant->ownerName
                << " | Restaurant: " << loggedInRestaurant->name
                << " @ " << loggedInRestaurant->location << "\n\n";
            cout << "  1. Add Menu Item\n";
            cout << "  2. Update Menu Item\n";
            cout << "  3. Delete Menu Item\n";
            cout << "  4. View Menu\n";
            cout << "  5. View Pending Orders\n";
            cout << "  6. Process Order (Assign to Rider)\n";
            cout << "  7. View Analytics\n";
            cout << "  0. Logout\n";
            int choice = readInt("\nChoice: ");
            switch (choice) {
            case 1: ownerAddMenuItem(); break;
            case 2: ownerUpdateMenuItem(); break;
            case 3: ownerDeleteMenuItem(); break;
            case 4:
                clearScreen();
                cout << "=== Menu ===\n\n";
                loggedInRestaurant->menu.displayInOrder();
                pause();
                break;
            case 5:
                clearScreen();
                cout << "=== Pending Orders ===\n\n";
                loggedInRestaurant->pendingOrders.displayAll();
                pause();
                break;
            case 6: ownerProcessOrders(); break;
            case 7: ownerViewAnalytics(); break;
            case 0: loggedInRestaurant = nullptr; break;
            default: cout << "Invalid choice.\n"; pause(); break;
            }
        }
    }

    // --- RIDER MENUS ---

    void riderSignUp() {
        clearScreen();
        cout << "=== Rider Sign Up ===\n\n";
        string name = readLine("Enter name: ");
        if (findRider(name)) {
            cout << "Rider already exists.\n";
            pause();
            return;
        }
        string password = readLine("Enter password: ");
        string location = readLine("Enter your location: ");
        riders.push_back(Rider(name, password, location));
        saveAll();
        cout << "Account created successfully!\n";
        pause();
    }

    void riderLogin() {
        clearScreen();
        cout << "=== Rider Login ===\n\n";
        string name = readLine("Enter name: ");
        string password = readLine("Enter password: ");
        Rider* r = findRider(name);
        if (!r || r->password != password) {
            cout << "Invalid credentials.\n";
            pause();
            return;
        }
        loggedInRider = r;
        cout << "Welcome, " << r->name << "!\n";
        pause();
        riderMenu();
    }

    void riderViewDeliveries() {
        clearScreen();
        cout << "=== Assigned Deliveries ===\n\n";
        loggedInRider->assignedDeliveries.displayAll();
        pause();
    }

    void riderDisplayDeliveryInfo() {
        clearScreen();
        cout << "=== Delivery Route Info ===\n\n";
        if (loggedInRider->assignedDeliveries.isEmpty()) {
            cout << "No assigned deliveries.\n";
            pause();
            return;
        }
        Order o;
        loggedInRider->assignedDeliveries.peek(o);
        cout << "Order #" << o.orderId << "\n\n";
        cout << "--- PICKUP ---\n";
        cout << "  Restaurant: " << o.restaurantName << "\n";
        cout << "  Location:   " << o.restaurantLocation << "\n\n";
        cout << "--- DROPOFF ---\n";
        cout << "  Customer:   " << o.customerName << "\n";
        cout << "  Location:   " << o.customerLocation << "\n\n";
        cout << "Items: ";
        for (size_t i = 0; i < o.itemNames.size(); i++) {
            if (i > 0) cout << ", ";
            cout << o.itemNames[i];
        }
        cout << "\nTotal: $" << fixed << setprecision(2) << o.totalPrice << "\n";
        pause();
    }

    void riderMarkDelivered() {
        clearScreen();
        cout << "=== Mark Order Delivered ===\n\n";
        if (loggedInRider->assignedDeliveries.isEmpty()) {
            cout << "No deliveries to complete.\n";
            pause();
            return;
        }
        loggedInRider->assignedDeliveries.displayAll();
        int choice = readInt("\nDeliver first order? (1=Yes, 0=No): ");
        if (choice != 1) return;

        Order o;
        if (!loggedInRider->assignedDeliveries.dequeue(o)) {
            cout << "No deliveries.\n";
            pause();
            return;
        }
        o.status = "delivered";

        Customer* c = findCustomer(o.customerName);
        if (c) c->orderHistory.push(o);

        loggedInRider->deliveryHistory.push(o);
        saveAll();

        cout << "\nOrder #" << o.orderId << " marked as delivered!\n";
        pause();
    }

    void riderViewHistory() {
        clearScreen();
        cout << "=== Delivery History ===\n\n";
        loggedInRider->deliveryHistory.displayAll();
        pause();
    }

    void riderMenu() {
        while (loggedInRider) {
            clearScreen();
            cout << "=== Rider Portal ===\n";
            cout << "Logged in as: " << loggedInRider->name
                << " @ " << loggedInRider->location << "\n\n";
            cout << "  1. View Assigned Deliveries\n";
            cout << "  2. Display Delivery Route Info\n";
            cout << "  3. Mark Order Delivered\n";
            cout << "  4. View Delivery History\n";
            cout << "  0. Logout\n";
            int choice = readInt("\nChoice: ");
            switch (choice) {
            case 1: riderViewDeliveries(); break;
            case 2: riderDisplayDeliveryInfo(); break;
            case 3: riderMarkDelivered(); break;
            case 4: riderViewHistory(); break;
            case 0: loggedInRider = nullptr; break;
            default: cout << "Invalid choice.\n"; pause(); break;
            }
        }
    }

public:
    Application()
        : loggedInCustomer(nullptr), loggedInRider(nullptr), loggedInRestaurant(nullptr) {
        loadAll();
    }

    ~Application() { saveAll(); }

    void run() {
        while (true) {
            clearScreen();
            cout << "============================================\n";
            cout << "     FOOD DELIVERY APPLICATION\n";
            cout << "============================================\n\n";
            cout << "  1. Customer\n";
            cout << "  2. Restaurant Owner\n";
            cout << "  3. Rider\n";
            cout << "  0. Exit\n";
            int role = readInt("\nSelect role: ");
            if (role == 0) {
                saveAll();
                cout << "Goodbye!\n";
                break;
            }
            clearScreen();
            cout << "  1. Sign Up\n";
            cout << "  2. Login\n";
            cout << "  0. Back\n";
            int action = readInt("Choice: ");
            switch (role) {
            case 1:
                if (action == 1) customerSignUp();
                else if (action == 2) customerLogin();
                break;
            case 2:
                if (action == 1) ownerSignUp();
                else if (action == 2) ownerLogin();
                break;
            case 3:
                if (action == 1) riderSignUp();
                else if (action == 2) riderLogin();
                break;
            }
        }
    }
};

// --- MAIN APPLICATION LOOP ---

int main() {
    Application app;
    app.run();
    return 0;
}
