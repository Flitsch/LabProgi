#include "realestate.h"

RealEstate::RealEstate() : owner_(""), date_(QDate::currentDate()), price_(0) {}

RealEstate::RealEstate(const QString& owner, const QDate& date, int price)
    : owner_(owner), date_(date), price_(price) {}

QString RealEstate::getOwner() const {
    return owner_;
}

QDate RealEstate::getDate() const {
    return date_;
}

int RealEstate::getPrice() const {
    return price_;
}

void RealEstate::setOwner(const QString& owner) {
    owner_ = owner;
}

void RealEstate::setDate(const QDate& date) {
    date_ = date;
}

void RealEstate::setPrice(int price) {
    price_ = price;
}