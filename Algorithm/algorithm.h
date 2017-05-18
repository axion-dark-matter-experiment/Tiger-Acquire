#ifndef ALGORITHM_H
#define ALGORITHM_H

//C System-Headers
//
//C++ System headers
#include <string>
//Qt Headers
//
//OpenCV Headers
//
//Boost Headers
#include <boost/algorithm/string.hpp>//split(), is_any_of, trim
#include <boost/lexical_cast.hpp>
//Project specific headers
//

namespace etig {

void remove_newlines( std::string& str );

template < typename T >
T mean( const std::vector<T>& vect ) {
    uint size_T = static_cast<T>( vect.size() );
    T sum = static_cast<T>( 0 );

    std::for_each( vect.begin(), vect.end(), [&] (T x) {
        sum += x;
    });

    return sum / size_T;
}

template < typename T >
struct Rebin {

    Rebin() {}

    /*!
     * \brief operator ()
     * \param time_series
     * The raw power signal that needs to be re-binned
     *
     * \param f_res
     * Center frequency of the current mode ( in MHz )
     *
     * \param time_int
     * Total time that time_series was collected over ( in seconds )
     */
    void operator()( const std::vector< T >& time_series, T f_res, T time_int ) {

        rebinned_signal.clear();

        uint sig_size = time_series.size();
        T delta_f = static_cast<T>( 1 )/( time_int );

        T f_axion = f_res*1e6 / ( static_cast<T>( 1e6 ) * static_cast<T>( 2 ) );

        overbin_size = static_cast< uint >( std::floor( f_axion / delta_f ) );

        if( overbin_size == 0 ) {
            std::string err_str =
                "Frequency seperation between signal elements is larger than 1/2 expected Axion signal width\n";
            err_str += "Axion signal is ";
            err_str += boost::lexical_cast< std::string >( f_axion / delta_f );
            err_str += " times larger than frequency seperation.";
            throw std::invalid_argument( err_str );
        }

        uint tail = sig_size % overbin_size;

        uint rebin_size = ( tail == 0 )?( sig_size / overbin_size ):( ( sig_size - tail ) / overbin_size );

        rebinned_signal.reserve( rebin_size );

        for( uint i = 0 ; i < sig_size - tail ; i += overbin_size ) {

            std::vector<T> sub_vector( time_series.begin() + i, time_series.begin() + ( i + 1 ) );
            rebinned_signal.push_back( mean( sub_vector ) );
        }

        std::vector<T> remainder( time_series.end() - tail, time_series.end() );
        rebinned_signal.push_back( mean( remainder ) );

    }

    /*!
     * \brief operator ()
     * \param time_series
     * The raw power signal that needs to be re-binned
     *
     * \param overbin_size
     * Number of elements that should be averaged into a single bin.
     */
    void operator()( const std::vector< T >& time_series, uint overbin_size ) {

        rebinned_signal.clear();

        uint sig_size = time_series.size();

        if( overbin_size == 0 ) {
            std::string err_str =
                "Requested rebin size is greater than signal size.\n";
            err_str += "Signal contains ";
            err_str += boost::lexical_cast< std::string >( sig_size );
            err_str += " rebin size was ";
            err_str += boost::lexical_cast< std::string >( overbin_size );
            throw std::invalid_argument( err_str );
        }

        uint tail = sig_size % overbin_size;
        uint rebin_size = ( tail == 0 )?( sig_size / overbin_size ):( ( sig_size - tail ) / overbin_size );

        rebinned_signal.reserve( rebin_size );

        for( uint i = 0 ; i < sig_size - tail ; i += overbin_size ) {

            std::vector<T> sub_vector( time_series.begin() + i, time_series.begin() + ( i + 1 ) );
            rebinned_signal.push_back( mean( sub_vector ) );
        }

        std::vector<T> remainder( time_series.end() - tail, time_series.end() );
        rebinned_signal.push_back( mean( remainder ) );

    }

    std::vector< T > RebinnedSignal() {
        return rebinned_signal;
    }

    uint RebinSize() {
        return overbin_size;
    }

  private:
    std::vector<T> rebinned_signal;
    uint overbin_size;

};

}

#endif /* ALGORITHM_H */
