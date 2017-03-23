#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//C System-Headers
//
//C++ System headers
//
//OpenCL Headers
//
//Boost Headers
//
//Qt Headers
#include <QMainWindow>
#include <QPalette>
#include <QHBoxLayout>
//Project specific headers
#include "Panels/SpectrumAnalyzer/spectrumanalyzer.h"
#include "Panels/InstrumentView/instrumentview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow( QWidget *parent = 0 );

    void SetSpectrumAnalyzerView( SpectrumAnalyzer* spec_analyzer );
    void SetNetworkAnalyzerView( InstrumentView* network_analyzer );
    ~MainWindow();

  private:

    Ui::MainWindow *ui;
    QHBoxLayout* front_panel_layout;
    QPalette dark_palette;
};

#endif // MAINWINDOW_H