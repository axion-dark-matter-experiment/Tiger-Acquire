//Header for this file
#include "programframe.h"
//C System-Headers
//
//C++ System headers
//
//Boost Headers
#include <boost/lexical_cast.hpp>//lexical cast

namespace etig {

ProgramFrame::ProgramFrame(QObject *parent) : ProgramCore( parent ) {

}

void ProgramFrame::Prequel() {
    PrequelReflection();
    mxg_n5183b->SetPower( signal_generator_power_dBm );
}

void ProgramFrame::SetBackground() {

    MoveToBGSubtractionLength();

    std::vector< double > background = hp8757_c->TakeDataMultiple();
    data_list blank_data = power_to_data_list( background, na_min_freq, na_max_freq );

    mode_tracker.SetBackground( blank_data );

    MoveToStartLength();

}

double ProgramFrame::FindMinimaPeak(data_list formatted_points ) {
    return mode_tracker.GetPeaksBiLat( formatted_points, 1 );
}

std::string ProgramFrame::BuildHeader() {

    std::string header = "";

    header += "length_of_tune;" + boost::lexical_cast<std::string>( length_of_tune ) + "\n";
    header += "revs_per_iterations;" + boost::lexical_cast<std::string>( revs_per_iterations ) + "\n";
    header += "start_length;" + boost::lexical_cast<std::string>( start_length ) + "\n";
    header += "nwa_span_MHz;" + boost::lexical_cast<std::string>( nwa_span_MHz ) + "\n";
    header += "nwa_points;" + boost::lexical_cast<std::string>( nwa_points ) + "\n";
    header += "nwa_power_dBm;" + boost::lexical_cast<std::string>( nwa_power_dBm ) + "\n";
    header += "freq_window_MHz;" + boost::lexical_cast<std::string>( freq_window_MHz ) + "\n";
    header += "digitizer_rate_MHz;" + boost::lexical_cast<std::string>( digitizer_rate_MHz ) + "\n";
    header += "num_averages;" + boost::lexical_cast<std::string>( num_averages ) + "\n";

    return header;
}

//cavity_length = self.ardu_comm.get_cavity_length()
//trans_window_str = self.convertor.power_list_to_str(power_list, mode_of_desire, self.freq_window, cavity_length)

//return self.m_track.GetMaxPeak(trans_window_str[:-1])

double ProgramFrame::DeriveCavityLength() {

    return start_length + ( iteration*length_of_tune /number_of_iterations );
}

double ProgramFrame::RecenterPeak( std::vector< data_triple<double> > to_check ) {

    try {
        return mode_tracker.GetMaxPeak( to_check );
    } catch (const daq_failure& e) {
        throw daq_failure( "Could not find a local maxima in provided data set." );
    }

}

void ProgramFrame::ShiftFrequencyWindow( double center_frequency ) {

    double nyquist_frequency = digitizer_rate_MHz/2.0;
    double shift_frequency = center_frequency - nyquist_frequency/2.0;

    mxg_n5183b->SetFrequency( shift_frequency );
}

double ProgramFrame::CheckPeak( double possible_mode_position ) {

    if( possible_mode_position <= 0.0 ) {
        throw daq_failure( "Mode position to less than or equal to 0 MHz" );
    }

    // Since we identified the position of our mode using reflection measurements
    // we need to switch to transmission to find the 'real' position of the mode
    xdl_35_5tp->SwitchToTransmission();
    hp8757_c->SetFrequencyWindow( possible_mode_position, nwa_span_MHz );

    std::vector< double > initial_window = hp8757_c->TakeDataSingle();

    emit UpdateNA( initial_window, nwa_span_MHz );
//    na_view->UpdateSignal( initial_window, nwa_span_MHz );

    double min_freq_initial = possible_mode_position - nwa_span_MHz/2.0;
    double max_freq_initial = possible_mode_position + nwa_span_MHz/2.0;

    double new_mode_position = 0;

    try {
        new_mode_position = RecenterPeak( power_to_data_list( initial_window, min_freq_initial, max_freq_initial ) );
    } catch (const daq_failure& e) {
        throw daq_failure( "Could not find a local maxima in provided data set." );
    }

    hp8757_c->SetFrequencyWindow( new_mode_position, nwa_span_MHz );

    std::vector< double > final_window = hp8757_c->TakeDataSingle();

//    double min_freq_final = new_mode_position - nwa_span_MHz/2;
//    double max_freq_final = new_mode_position + nwa_span_MHz/2;

//    data_list mode_window = power_to_data_list( final_window, min_freq_final, max_freq_final );

//    na_view->UpdateSignal( final_window, single_scan_window );

    emit UpdateNA( final_window, nwa_span_MHz );

    //Return to reflection measurements
    xdl_35_5tp->SwitchToReflection();

    return new_mode_position;

}

template < typename T >
std::vector< T > ProgramFrame::data_list_to_power( std::vector< data_triple <T> > data ) {

    std::vector< T > power_list;
    power_list.reserve( data.size() );

    for( const auto& triple : data ) {
        power_list.push_back( triple.power_dBm );
    }

    return power_list;
}

std::vector< data_triple<double> > ProgramFrame::power_to_data_list ( std::vector< float > power_list,
                                                                      float min_freq,
                                                                      float max_freq ) {

    uint number_of_points = power_list.size();
//    float max_freq = center_frequency + nwa_span_MHz / 2.0f;
//    float min_freq = center_frequency - nwa_span_MHz / 2.0f;

    float cavity_length = static_cast<float>( arduino->GetCavityLength() );

    std::vector< data_triple< double > > processed;
    processed.reserve( number_of_points );

    for( uint i = 0; i < number_of_points ; i++ ) {

        double i_f = static_cast<double>(i);
        double num_points_f = static_cast<double>(number_of_points);

        double min_freq_d = static_cast< double >( min_freq );
        double max_freq_d = static_cast< double >( max_freq );
        double frequency = ( i_f/num_points_f )*( max_freq_d - min_freq_d ) + min_freq_d;
        double power = static_cast<double>( power_list.at(i) );

        processed.push_back( data_triple< double >( cavity_length, std::round( frequency ), power ) );

    }

    return processed;

}

std::vector< data_triple<double> > ProgramFrame::power_to_data_list ( std::vector< double > power_list,
                                                                      double min_freq,
                                                                      double max_freq ) {

    uint number_of_points = power_list.size();
//    double max_freq = center_frequency + nwa_span_MHz / 2.0;
//    double min_freq = center_frequency - nwa_span_MHz / 2.0;

    double cavity_length = arduino->GetCavityLength();

    std::vector< data_triple<double> > processed;
    processed.reserve( number_of_points );

    for( uint i = 0; i < number_of_points ; i++ ) {

        double i_f = static_cast<double>(i);
        double num_points_f = static_cast<double>(number_of_points);

        double frequency = ( i_f/num_points_f )*( max_freq - min_freq ) + min_freq;
        double power = power_list.at(i);

        processed.push_back( data_triple< double >( cavity_length, std::round( frequency ), power ) );

    }

    return processed;

}

}
