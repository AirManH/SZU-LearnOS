#include <szu-learnos-utils.hpp>
#include <exception>

namespace los
{

QList<qint64> range(qint64 begin, qint64 end)
{
    if (begin >= end) {
        throw std::invalid_argument("begin should less than end");
    }
    QList<qint64> a;
    while (begin < end) {
        a.append(begin);
        begin += 1;
    }
    return a;
}

QList<qint64> range(qint64 end)
{
    return range(0, end);
}

void Loggable::debug(const QString &s)
{
    emit to_debug(s);
}
void Loggable::info(const QString &s)
{
    emit to_info(s);
}

}
