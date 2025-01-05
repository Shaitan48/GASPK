#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QStringListModel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    QListView *getOperationListView();

public slots:
    void updateOperationList(const QStringList &operations);

private:
    QListView *operationListView;
};

#endif // MAINWINDOW_H