#include <algorithm>
#include <cmath>
#include "modecharacterization.h"

// Taken from StackExchange
// http://codereview.stackexchange.com/questions/48470/std-lib-like-c-function-to-find-nearest-elements-in-a-container
template<typename Q, typename I, typename Distance>
void find_n_nearest(const Q& q, I first, I nth, I last, Distance dist)
{
    using T = decltype(*first);
    auto compare = [&q, &dist] (T i, T j) { return dist(i, q) < dist(j, q); };
    std::nth_element(first, nth, last, compare);
    std::sort(first, last, compare);
}

ModeTraits::ModeTraits( const std::vector< data_triple<double> >& data_values ) {

    auto compare = [] (data_triple<double> i, data_triple<double> j) { return ( i.power_dBm < j.power_dBm ); };
    auto max_power = std::max_element( data_values.begin(), data_values.end(), compare );
    data_triple<double> max_val = *max_power;
    f_0 = max_val.frequency_MHz;

    std::vector< data_triple<double> > left_side( data_values.begin(), max_power );
    std::vector< data_triple<double> > right_side( max_power, data_values.end() );

    std::cout << max_val << std::endl;

    data_triple<double> three_dB_drop ( max_val.cavity_length, max_val.frequency_MHz, max_val.power_dBm - 3.0 );

    auto distance = [] (data_triple<double> i, data_triple<double> j) { return std::abs( i.power_dBm - j.power_dBm ); };

    find_n_nearest( three_dB_drop , left_side.begin(), left_side.end(), left_side.end(), distance );
    auto three_dB_drop_left = *left_side.begin();
    std::cout << three_dB_drop_left << std::endl;

    find_n_nearest( three_dB_drop , right_side.begin(), right_side.end(), right_side.end(), distance );
    auto three_dB_drop_right = *right_side.begin();
    std::cout << three_dB_drop_right << std::endl;

    double hwhm = std::abs( three_dB_drop_right.frequency_MHz - three_dB_drop_left.frequency_MHz );

    Q_factor = f_0/(hwhm);
}

double ModeTraits::Q() {
    return Q_factor;
}

double ModeTraits::f0() {
    return f_0;
}
