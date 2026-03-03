#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

using namespace std;

// Класс для хранения информации о недвижимости
class RealEstate {
private:
    string owner;           // владелец (строка)
    string registrationDate; // дата постановки на учет (в формате гггг.мм.дд)
    int estimatedValue;      // ориентировочная стоимость (целое)
    string color; //цвет
public:
    // Конструктор
    RealEstate(const string& ownerName, const string& date, int value) 
        : owner(ownerName), registrationDate(date), estimatedValue(value) {}

    // Метод для вывода информации
    void printInfo() const {
        cout << "\n=== Информация об объекте недвижимости ===\n";
        cout << "Владелец: " << owner << endl;
        cout << "Дата постановки на учет: " << registrationDate << endl;
        cout << "Ориентировочная стоимость: " << estimatedValue << " руб." << endl;
        cout << "=========================================\n";
    }

    // Геттеры (при необходимости)
    string getOwner() const { return owner; }
    string getDate() const { return registrationDate; }
    int getValue() const { return estimatedValue; }
};

// Функция для разбора строки и создания объекта недвижимости
RealEstate parseRealEstate(const string& input) {
    stringstream ss(input);
    string token;
    vector<string> tokens;

    // Разбиваем строку на токены, учитывая кавычки для строковых свойств
    string currentToken;
    bool inQuotes = false;

    for (char c : input) {
        if (c == '"') {
            inQuotes = !inQuotes;
            if (!inQuotes) {
                // Закрывающая кавычка - добавляем токен
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            continue;
        }

        if (c == ' ' && !inQuotes) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else {
            currentToken += c;
        }
    }

    // Добавляем последний токен, если он есть
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }

    // Первый токен - тип объекта (пропускаем, т.к. мы знаем что это недвижимость)
    // Остальные токены: владелец, дата, стоимость
    
    // Предполагаем, что строка имеет формат: "Недвижимость" Иванов 2023.10.15 5000000
    // или Недвижимость "Иванов Петр" 2023.10.15 5000000
    
    string owner;
    string date;
    int value;

    // Находим владельца (может быть в кавычках или без)
    int index = 1; // Пропускаем первый токен "Недвижимость"
    
    // Проверяем, является ли токен датой (содержит точки)
    if (tokens[index].find('.') != string::npos && tokens[index].size() == 10) {
        // Формат: Недвижимость Иванов 2023.10.15 5000000
        owner = tokens[index];
        index++;
    } else {
        // Формат: Недвижимость "Иванов Петр" 2023.10.15 5000000
        owner = tokens[index];
        index++;
    }
    
    // Получаем дату
    date = tokens[index];
    index++;
    
    // Получаем стоимость
    value = stoi(tokens[index]);

    return RealEstate(owner, date, value);
}

int main() {
    // Примеры корректных входных строк
    vector<string> testInputs = {
        "Недвижимость Иванов 2023.10.15 5000000",
        "Недвижимость \"Петров Сидор\" 2022.05.20 7500000",
        "Недвижимость Смирнова 2024.01.10 3200000",
        "Недвижимость \"Иванова Мария Петровна\" 2023.12.01 8900000"
    };

    cout << "Программа для создания объектов недвижимости из текстового описания\n";
    cout << "================================================================\n";

    // Обрабатываем каждый тестовый пример
    for (size_t i = 0; i < testInputs.size(); i++) {
        cout << "\nИсходная строка: " << testInputs[i];
        
        try {
            RealEstate property = parseRealEstate(testInputs[i]);
            property.printInfo();
        } catch (const exception& e) {
            cout << "\nОшибка при обработке: " << e.what() << endl;
        }
    }

    // Интерактивный режим
    cout << "\n================================================================\n";
    cout << "Введите описание недвижимости (или 'exit' для выхода):\n";
    cout << "Формат: Недвижимость [\"Владелец\"] ГГГГ.ММ.ДД Стоимость\n";
    
    string userInput;
    while (getline(cin, userInput)) {
        if (userInput == "exit") break;
        
        if (!userInput.empty()) {
            try {
                RealEstate property = parseRealEstate(userInput);
                property.printInfo();
            } catch (const exception& e) {
                cout << "Ошибка при обработке: " << e.what() << endl;
            }
        }
        
        cout << "\nВведите следующее описание: ";
    }

    return 0;
}
