#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

class Property {
private:
    string owner;
    string date;
    int price;
    
public:
    Property(string o, string d, int p) : owner(o), date(d), price(p) {}
    
    string getOwner() const { return owner; }
    string getDate() const { return date; }
    int getPrice() const { return price; }
};

class PropertyManager {
private:
    vector<Property> properties;
    
public:
    PropertyManager() {
        properties.push_back(Property("Иванов Иван", "2023.01.15", 4500000));
        properties.push_back(Property("Петров Петр", "2023.02.20", 7800000));
        properties.push_back(Property("Сидорова Анна", "2023.03.10", 3200000));
    }
    
    void addProperty(const string& owner, const string& date, int price) {
        properties.push_back(Property(owner, date, price));
        cout << "\nОбъект добавлен\n";
    }
    
    void showAll() const {
        if (properties.empty()) {
            cout << "\nСписок пуст\n";
            return;
        }
        
        cout << "\nВСЕ ОБЪЕКТЫ (" << properties.size() << ")" << endl;
        cout << "----------------------------------------" << endl;
        
        for (const auto& prop : properties) {
            cout << prop.getOwner() << " | " 
                 << prop.getDate() << " | " 
                 << prop.getPrice() << " руб." << endl;
        }
        cout << "----------------------------------------" << endl;
    }
    
    long long getTotalValue() const {
        long long total = 0;
        for (const auto& prop : properties) {
            total += prop.getPrice();
        }
        return total;
    }
};


int f(vector<Property> properties)
{
    int count = 0;
    for (int i = 0; i < properties.size(); i++)
    {
        if (properties[i].getPrice() > 5000)
        {
            count += 1;
        }
    }
    return count;
}

int main() {
    PropertyManager manager;
    string command, owner, date;
    int price;
    
    cout << "ПРОГРАММА УЧЕТА НЕДВИЖИМОСТИ" << endl;
    cout << "=============================" << endl;
    cout << "Команды:" << endl;
    cout << "  add \"Имя Фамилия\" ГГГГ.ММ.ДД Цена - добавить объект" << endl;
    cout << "  list - показать все" << endl;
    cout << "  total - общая стоимость" << endl;
    cout << "  exit - выход" << endl;
    cout << "=============================" << endl;
    
    manager.showAll();
    
    while (true) {
        cout << "\n> ";
        cin >> command;
        
        if (command == "exit") {
            cout << "Программа завершена" << endl;
            break;
        }
        
        if (command == "list") {
            manager.showAll();
            continue;
        }
        
        if (command == "total") {
            cout << "Общая стоимость: " << manager.getTotalValue() << " руб." << endl;
            continue;
        }
        
        if (command == "add") {
            cin >> ws;
            
            // Проверяем, что первым символом идет кавычка
            if (cin.peek() == '"') {
                cin.ignore(); // пропускаем открывающую кавычку
                getline(cin, owner, '"'); // читаем до закрывающей кавычки
                cin >> date >> price;
                
                manager.addProperty(owner, date, price);
                manager.showAll();
            } else {
                cout << "Ошибка: имя должно быть в кавычках" << endl;
                cin.ignore(10000, '\n');
            }
        } else {
            cout << "Неизвестная команда" << endl;
            cin.ignore(10000, '\n');
        }
    }
    return 0;
}
