#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H

#include <vector>
#include <QString>
#include "realestate.h"

struct ParseError;

class PropertyManager {
public:
    PropertyManager();

    // Загрузка и сохранение
    bool loadFromFile(const QString& filename, QVector<ParseError>& errors);
    bool saveToFile(const QString& filename);

    // Управление данными
    void addProperty(const RealEstate& property);
    void removeProperty(int index);
    void clearAll();

    // Получение данных
    const std::vector<RealEstate>& getAllProperties() const;
    int getCount() const;
    RealEstate getProperty(int index) const;

    // Статистика
    int countExpensive(int minPrice) const;
    long long getTotalValue() const;

    // Получение последней ошибки
    QString getLastError() const { return lastError_; }

private:
    std::vector<RealEstate> properties_;
    QString lastError_;
};

#endif // PROPERTYMANAGER_H