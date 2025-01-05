#include "MainWindow.h"
#include <QVBoxLayout>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    operationListView = new QListView(this);
    setCentralWidget(operationListView);

}
MainWindow::~MainWindow(){}
QListView *MainWindow::getOperationListView()
{
    return operationListView;
}
void MainWindow::updateOperationList(const QStringList &operations)
{
  operationListView->setModel(new QStringListModel(operations, this));
}