#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Eigen/Core>
#include <QMainWindow>
#include "gurobi_c++.h"
#include <fstream>
#include <cstring>
#include <QFileDialog>
#include <QFileInfo>
#include <iostream>     // std::cout, std::ios

//green == 0
// red == 1
using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Eigen::MatrixXi adjacency;
    Eigen::VectorXi values;
    Eigen::VectorXi constants;
    /* The graph has points with two properties: value and constant (all binary)
    *  Each edge has a weigh (binary)
    *  The format use to represent that is a adjacency matrix. When a edge exists between the nodes i and j the matrix value
    *  represent the weight. In two arrays we store the values and the constant
    *  Each line of the file is in the format :
    *  [ i, j, (edge weight), (value node i), (value node j) (constant node i) (constant node j) ]
    */
    void chargeGraph();
    void export_as_dot(Eigen::MatrixXi &graph,Eigen::VectorXi &gvalues,QString name);
    void process();

signals:
    /// signal for setting the statusbar message
     void setStatusBar(QString message);
private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
