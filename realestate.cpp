#include "realestate.h"

RealEstate::RealEstate() : owner_(""), date_(QDate::currentDate()), price_(0) {}

RealEstate::RealEstate(const QString& owner, const QDate& date, int price)
{
    setOwner(owner);
    setDate(date);
    setPrice(price);
}

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
    if (owner.isEmpty()) {
        throw RealEstateException("Ошибка: владелец не может быть пустым");
    }
    owner_ = owner;
}

void RealEstate::setDate(const QDate& date) {
    if (!date.isValid()) {
        throw RealEstateException("Ошибка: неверная дата");
    }
    if (date > QDate::currentDate()) {
        throw RealEstateException("Ошибка: дата не может быть в будущем");
    }
    date_ = date;
}

void RealEstate::setPrice(int price) {
    if (price < 0) {
        throw RealEstateException("Ошибка: стоимость не может быть отрицательной");
    }
    price_ = price;
}

bool RealEstate::isValid() const {
    return !owner_.isEmpty() && date_.isValid() && price_ >= 0;
}