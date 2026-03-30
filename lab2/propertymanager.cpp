#include "propertymanager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

PropertyManager::PropertyManager() {}

bool PropertyManager::loadFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    properties_.clear();
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        int firstQuote = line.indexOf('"');
        int secondQuote = line.indexOf('"', firstQuote + 1);

        if (firstQuote != -1 && secondQuote != -1) {
            QString owner = line.mid(firstQuote + 1, secondQuote - firstQuote - 1);
            QString remaining = line.mid(secondQuote + 1).trimmed();
            QStringList parts = remaining.split(' ', Qt::SkipEmptyParts);

            if (parts.size() >= 2) {
                QDate date = QDate::fromString(parts[0], "yyyy.MM.dd");
                int price = parts[1].toInt();
                if (date.isValid() && price > 0) {
                    properties_.push_back(RealEstate(owner, date, price));
                }
            }
        }
    }

    file.close();
    return true;
}

bool PropertyManager::saveToFile(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
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

void PropertyManager::addProperty(const RealEstate& property) {
    properties_.push_back(property);
}

void PropertyManager::removeProperty(int index) {
    if (index >= 0 && index < static_cast<int>(properties_.size())) {
        properties_.erase(properties_.begin() + index);
    }
}

const std::vector<RealEstate>& PropertyManager::getAllProperties() const {
    return properties_;
}

int PropertyManager::getCount() const {
    return properties_.size();
}

RealEstate PropertyManager::getProperty(int index) const {
    if (index >= 0 && index < static_cast<int>(properties_.size())) {
        return properties_[index];
    }
    return RealEstate();
}

int PropertyManager::countExpensive(int minPrice) const {
    int count = 0;
    for (const auto& prop : properties_) {
        if (prop.getPrice() > minPrice) {
            count++;
        }
    }
    return count;
}

long long PropertyManager::getTotalValue() const {
    long long total = 0;
    for (const auto& prop : properties_) {
        total += prop.getPrice();
    }
    return total;
}