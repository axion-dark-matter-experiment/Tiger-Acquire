#ifndef TEST_MODECHARACTERIZATION_H
#define TEST_MODECHARACTERIZATION_H


//C System-Headers
//
//C++ System headers
#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>
//Qt Headers
//
//OpenCV Headers
//
//Boost Headers
//
//Project specific headers
#include "modecharacterization.h"

namespace etig {

namespace test {

double volts_sqr_to_dbm( double voltage ) {
    return 10.0 * std::log10( voltage / std::sqrt(0.05) );
}

struct VoltsSqrTodBm {

    void operator()(data_triple<double> &data) const {
        data.power_dBm = volts_sqr_to_dbm( data.power_dBm );
    }
};

inline double lorentzian( double f, double Q, double f_0 ) {

    double fwhm = f_0/Q;
    double gamma = ( f_0 - f )/( fwhm/2.0 );

    return 1.0/( 1.0 + gamma*gamma );
}


void TestModeCharacterization( double f_0, double Q, uint data_size ) {

    std::vector< data_triple<double> > test_data;
    test_data.reserve( data_size );

    std::cout << "Testing mode characretizatipn.\n"
              << "Test mode has center frequency of "
              << f_0
              << " and Q of "
              << Q
              << std::endl;

    for( uint i = 0; i < data_size; i++ ) {

        double cavity_length = 0.0;
        double frequency = static_cast<double>( i );
        double power_dBm = lorentzian( frequency, Q, f_0 );

        test_data.push_back( data_triple<double>( cavity_length, frequency, power_dBm ) );

    }

    std::for_each( test_data.begin(), test_data.end(), VoltsSqrTodBm() );

    ModeTraits characterization( test_data );

    double computed_Q = characterization.Q();
    double computed_f0 = characterization.f0();

    std::cout << "Computed mode parameters.\n"
              << "Center frequency of "
              << computed_f0
              << " and Q of "
              << computed_Q
              << std::endl;
}

}

}

#endif // TEST_MODECHARACTERIZATION_H
