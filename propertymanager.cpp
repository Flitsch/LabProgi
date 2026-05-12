#include "propertymanager.h"
#include "propertyparser.h"
#include <QFile>
#include <QTextStream>

PropertyManager::PropertyManager() {}

bool PropertyManager::loadFromFile(const QString& filename, QVector<ParseError>& errors)
{
    PropertyParser parser;
    QVector<RealEstate> loadedProperties = parser.parseFile(filename, errors);

    if (!loadedProperties.empty()) {
        properties_.clear();
        properties_.reserve(loadedProperties.size());
        for (const auto& prop : loadedProperties) {
            properties_.push_back(prop);
        }
        return true;
    }

    return false;
}

bool PropertyManager::saveToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        lastError_ = "Не удалось открыть файл для записи: " + filename;
        return false;
    }

    QTextStream out(&file);
    for (const auto& prop : properties_) {
        out << "Недвижимость \"" << prop.getOwner() << "\" "
            << prop.getDate().toString("yyyy.MM.dd") << " "
            << prop.getPrice() << "\n";
    }

    file.close();
    return true;
}

void PropertyManager::addProperty(const RealEstate& property)
{
    properties_.push_back(property);
}

void PropertyManager::removeProperty(int index)
{
    if (index >= 0 && index < static_cast<int>(properties_.size())) {
        properties_.erase(properties_.begin() + index);
    }
}

void PropertyManager::clearAll()
{
    properties_.clear();
}

const std::vector<RealEstate>& PropertyManager::getAllProperties() const
{
    return properties_;
}

int PropertyManager::getCount() const
{
    return properties_.size();
}

RealEstate PropertyManager::getProperty(int index) const
{
    if (index >= 0 && index < static_cast<int>(properties_.size())) {
        return properties_[index];
    }
    return RealEstate();
}

int PropertyManager::countExpensive(int minPrice) const
{
    int count = 0;
    for (const auto& prop : properties_) {
        if (prop.getPrice() > minPrice) {
            count++;
        }
    }
    return count;
}

long long PropertyManager::getTotalValue() const
{
    long long total = 0;
    for (const auto& prop : properties_) {
        total += prop.getPrice();
    }
    return total;
}