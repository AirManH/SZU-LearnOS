#ifndef SZU_LEARNOS_UTILS_HPP
#define SZU_LEARNOS_UTILS_HPP

#include <QList>
#include <QObject>

namespace los
{

QList<qint64> range(qint64 begin, qint64 end);

QList<qint64> range(qint64 end);

class Loggable
{

public:
    ~Loggable()
    {};
public:
    void debug(const QString &s);
    void info(const QString &s);
signals:
    virtual void to_debug(QString s) = 0;
    virtual void to_info(QString s) = 0;
};

}

#endif //SZU_LEARNOS_UTILS_HPP
