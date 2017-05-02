//Header for this file
#include "program.h"
//C System-Headers
#include <cstdlib> // std::atexit
//C++ System headers
#include <algorithm>
#include <future>
//Qt Headers
//
//OpenCV Headers
//
//Boost Headers
//
//Project specific headers

namespace etig {
//Public functions

Program::Program(QObject *parent) : ProgramFrame( parent ) {

//    connect( this, &Program::UpdateNA, na_view, &InstrumentView::UpdateSignal );
//    connect( this, &Program::UpdateSpec, spec_analyzer, &SpectrumAnalyzer::UpdateSignal );

//    if ( std::atexit(Program::PanicCleanUp&) != 0 ) {
//        std::cout << "Failed to register CleanUp" << std::endl;
//    }

}

Program::~Program() {
}

double Program::FindModeReflection() {

    auto network_analyzer_scan = hp8757_c->TakeDataMultiple();

//    na_view->UpdateSignal( network_analyzer_scan, nwa_span_MHz*4.0 );
    emit UpdateNA( network_analyzer_scan, nwa_span_MHz*4.0 );

    auto formatted_na_scan = power_to_data_list( network_analyzer_scan, na_min_freq, na_max_freq );

    try {
        return FindMinimaPeak( formatted_na_scan );
    } catch (const mode_track_failure& e) {
        throw( daq_failure( "Mode not found in power spectrum." ) );
    }

}

double Program::FindModeTransmission( double mode_frequency ) {

    try {
        return CheckPeak( mode_frequency );
    } catch (const mode_track_failure& e) {
        throw( daq_failure( "Search mode not found in power spectrum." ) );
    }
}

std::vector< data_triple<double> > Program::TakeData( double mode_frequency ) {

    ShiftFrequencyWindow( mode_frequency );
    emit LOFrequency( mode_frequency );

    ats9462->Start();

    while( !ats9462->Finished() ) {

        std::vector< float > volts_data = ats9462->PullVoltageDataTail( 1024 );
        emit UpdateSpec( volts_data, static_cast<uint>( 2e6 ) );

        sleep(5);
    }

    std::vector< float > signal;

    try {
        signal = ats9462->FinishedSignal();
    } catch ( const std::ios_base::failure& e ) {
        throw( daq_failure( "Signal could not be read from digitizer." ) );
    }

    float nyquist_over_2 = static_cast< float >( digitizer_rate_MHz )/4.0f;
    float min_freq = mode_frequency - nyquist_over_2;
    float max_freq = mode_frequency + nyquist_over_2;

    return power_to_data_list( signal, min_freq, max_freq );

}

void Program::SavePowerSpectrum( const data_list& scan ) {

    std::string header = BuildHeader();
    data_saver.Save( scan, header );
}

void Program::PanicCleanUp() {
    MoveToStartLength();
}

void Program::Run() {

    Prequel();
    SetBackground();

    for( uint i = 0; i <= number_of_iterations ; i ++ ) {

        emit Iteration( i );

        double mode_of_desire = 0.0;

        try {
            mode_of_desire = FindModeReflection();
        } catch (const daq_failure& e) {

            std::cout << "Could not locate mode in reflection measurements"
                      << ", caught error message:"
                      << e.what()
                      << " advancing to next search region."
                      << std::endl;

            NextIteration();
            continue;
        }

        try {
            mode_of_desire = FindModeTransmission( mode_of_desire );
        } catch (const daq_failure& e) {

            std::cout << "Could not locate mode in transmission measurements"
                      << ", caught error message:"
                      << e.what()
                      << " advancing to next search region."
                      << std::endl;

            NextIteration();
            continue;
        }

        auto current_scan = TakeData( mode_of_desire );
        SavePowerSpectrum( current_scan );

        NextIteration();

    }
}

void Program::Stop() {
    //Usually digitizer class should handle
    // things upon deletion, but it seems that Qt
    // is interfering with this.
    ats9462->AbortCapture();
}

//Private function
double Program::DeriveLengthFromStart() {

    double cavity_length = arduino->GetCavityLength();
    return start_length - cavity_length;

}

}
