#include "calc_window.h"
#include <QApplication>

/*
 * Function: Application entry point
 * Input: argc - argument count; argv - argument values
 * Output: none
 * Return: application exit code
 */
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    CalcWindow window;
    window.setWindowTitle("Calculator");
    window.resize(360, 540);
    window.show();

    return app.exec();
}
