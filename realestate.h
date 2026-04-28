#ifndef REALESTATE_H
#define REALESTATE_H

#include <QString>
#include <QDate>
#include <stdexcept>

class RealEstateException : public std::runtime_error {
public:
    explicit RealEstateException(const QString& msg)
        : std::runtime_error(msg.toStdString()), message(msg) {}

    QString getMessage() const { return message; }

private:
    QString message;
};

class RealEstate {
public:
    RealEstate();
    RealEstate(const QString& owner, const QDate& date, int price);

    // Геттеры
    QString getOwner() const;
    QDate getDate() const;
    int getPrice() const;

    // Сеттеры с валидацией
    void setOwner(const QString& owner);
    void setDate(const QDate& date);
    void setPrice(int price);

    // Валидация
    bool isValid() const;

private:
    QString owner_;
    QDate date_;
    int price_;
};

#endif // REALESTATE_H