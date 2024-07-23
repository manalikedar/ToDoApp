#include "ToDoApps.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    std::cout << "hello world\n";
    QApplication a(argc, argv);
    ToDoApps w;
    w.show();
    return a.exec();
}
