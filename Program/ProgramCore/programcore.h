#ifndef PROGRAMCORE_H
#define PROGRAMCORE_H


// C System-Headers
//
// C++ System headers
#include <memory>
// Qt Headers
#include <QObject>
// ATS9462 Headers
#include "../ATS9462/Digitizer/ATS9462Engine/ats9462engine.h"
// JASPL Headers
//
// Project specific headers
#include "../../SocketCommunicators/socketcommunicators.h"
#include "../../Generics/generics.h"
#include "daq_failure.h"

#include "../../Windows/mainwindow.h"
#include "../../Panels/SpectrumAnalyzer/spectrumanalyzer.h"
#include "../../SocketCommunicators/NetworkAnalyzer/network_analyzer.h"
#include "../../Windows/mainwindow.h"
#include "../../Panels/InstrumentView/instrumentview.h"

namespace etig {

class ProgramCore: public QObject {

    Q_OBJECT

  public:
    ProgramCore( QObject* parent = 0 );

    void RetractCavity();
    void RapidTraverse();
    void PrequelTransmission();
    void PrequelReflection();
    void NextIteration();
    void Jitter();

  protected:

    void MoveToBGSubtractionLength();
    void MoveToStartLength();
    void MoveToEndLength();

    const std::string save_file_path = "/home/admx/Electric_Tiger_Data/";
    const double length_of_tune = 2.0;
    const double revs_per_iterations = 1;
    const double start_length = 8.0;
    const double background_scan_length = 5.5;
    const double nwa_span_MHz = 400.0;
    const uint nwa_points = 401;
    const double nwa_power_dBm = -15.0;
    const double signal_generator_power_dBm = 15.0; //Recommended power for Polyphase P/N IRM3570B
    const double freq_window_MHz = 100.0;
    const double digitizer_rate_MHz = 180.0;
    const double na_min_freq = 3000.0;
    const double na_max_freq = 4600.0;
    const uint num_averages = 10;
//    uint rebin_size = 0;
    const uint rebin_size = 2000;

    const TCPSocketParam psu_switch = TCPSocketParam( "Switch", "10.95.100.174", 9221 );
    const TCPSocketParam network_analyzer = TCPSocketParam( "NetworkAnalyzer", "10.95.100.176", 1234 );
    const TCPSocketParam stepper = TCPSocketParam( "Stepper", "10.95.100.177", 7776 );
    const TCPSocketParam signal_generator = TCPSocketParam( "SignalGenerator", "10.95.100.175", 5025 );
    const TCPSocketParam arduino_info = TCPSocketParam( "Arduino", "10.95.100.173", 23 );

    std::shared_ptr<ATS9462Engine> ats9462;

    Arduino* arduino;
    NetworkAnalyzer* hp8757_c;
    SignalGenerator* mxg_n5183b;
    StepperMotor* stm23_ee;
    Switch* xdl_35_5tp;

    double number_of_iterations = 0.0;
    uint iteration = 0;

  private:
    void InitializeSocketObjects();
    void InitializeDigitizer();

    int DeriveNumberofIterations();

};

}

#endif // PROGRAMCORE_H
