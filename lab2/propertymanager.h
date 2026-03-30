#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H

#include <vector>
#include "realestate.h"

class PropertyManager {
public:
    PropertyManager();

    bool loadFromFile(const QString& filename);
    bool saveToFile(const QString& filename) const;

    void addProperty(const RealEstate& property);
    void removeProperty(int index);

    const std::vector<RealEstate>& getAllProperties() const;
    int getCount() const;
    RealEstate getProperty(int index) const;
    int countExpensive(int minPrice) const;
    long long getTotalValue() const;

private:
    std::vector<RealEstate> properties_;
};

#endif // PROPERTYMANAGER_H