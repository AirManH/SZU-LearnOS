#include <read_write_manager.hpp>
#include <QDebug>

int main()
{
    los::ReadWriteManager rwm("./a.txt");
    rwm.add_writers(1);
    rwm.add_readers(1);
    return 0;
}