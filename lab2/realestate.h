#ifndef REALESTATE_H
#define REALESTATE_H

#include <QString>
#include <QDate>

class RealEstate {
public:
    RealEstate();
    RealEstate(const QString& owner, const QDate& date, int price);

    QString getOwner() const;
    QDate getDate() const;
    int getPrice() const;

    void setOwner(const QString& owner);
    void setDate(const QDate& date);
    void setPrice(int price);

private:
    QString owner_;
    QDate date_;
    int price_;
};

#endif // REALESTATE_H