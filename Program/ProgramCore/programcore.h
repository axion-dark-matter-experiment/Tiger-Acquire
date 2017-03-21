#ifndef PROGRAMCORE_H
#define PROGRAMCORE_H


// C System-Headers
//
// C++ System headers
#include <memory>
// Qt Headers
//
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

class ProgramCore {

  public:
    ProgramCore();

    void RetractCavity();
    void RapidTraverse();
    void PrequelTransmission();
    void PrequelReflection();
    void NextIteration();

  protected:

    const std::string save_file_path = "/home/admx/Electric_Tiger_Data/";
    const double length_of_tune = 3.0;
    const double revs_per_iterations = 2.5;
    const double start_length = 6.0;
    const double nwa_span_MHz = 400.0;
    const uint nwa_points = 401;
    const double nwa_power_dBm = -15.0;
    const double signal_generator_power_dBm = 15.0; //Recommended power for Polyphase P/N IRM3570B
    const double freq_window_MHz = 100.0;
    const double digitizer_rate_MHz = 0.5;
    const uint num_averages = 10000;

    const TCPSocketParam psu_switch = TCPSocketParam( "Switch", "10.95.100.174", 9221 );
    const TCPSocketParam network_analyzer = TCPSocketParam( "NetworkAnalyzer", "10.95.100.176", 1234 );
    const TCPSocketParam stepper = TCPSocketParam( "Stepper", "10.95.100.177", 7776 );
    const TCPSocketParam signal_generator = TCPSocketParam( "SignalGenerator", "10.95.100.175", 5025 );
    const TCPSocketParam arduino_info = TCPSocketParam( "Arduino", "10.95.100.173", 23 );


    std::shared_ptr<ATS9462Engine> ats9462;
    std::shared_ptr<Arduino> arduino;
    std::shared_ptr<NetworkAnalyzer> hp8757_c;
    std::shared_ptr<SignalGenerator> mxg_n5183b;
    std::shared_ptr<StepperMotor> stm23_ee;
    std::shared_ptr<Switch> xdl_35_5tp;

    MainWindow* window;
    SpectrumAnalyzer* spec_analyzer;

    InstrumentView* na_view;

    double number_of_iterations = 0.0;
    uint iteration = 0;

  private:
    void MoveToStartLength();
    void InitializeSocketObjects();
    void InitializeDigitizer();

    int DeriveNumberofIterations();

};

}

#endif // PROGRAMCORE_H
