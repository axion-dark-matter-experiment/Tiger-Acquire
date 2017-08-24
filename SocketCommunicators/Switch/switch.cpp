//C System-Headers
//
//C++ System headers
#include <sstream>
#include <string>
#include <stdexcept>
#include <iostream>
//Boost Headers
#include <boost/lexical_cast.hpp>
//Qt Headers
//
//Project specific headers
#include "switch.h"

Switch::Switch(std::string ip_addr,
               uint port_number,
               QObject *parent) : AbstractSocketCommunicator( ip_addr, port_number, parent ) {
    SetVoltages();
}

void Switch::SwitchToNetworkAnalyzer() {
    socket->Send( "OP1 0" );
}

void Switch::SwitchToDigitizer() {
    socket->Send( "OP1 1" );
}

void Switch::SwitchToTransmission() {
    socket->Send( "OP2 0" );
}

void Switch::SwitchToReflection() {
    socket->Send( "OP2 1" );
}

//Private Functions
void Switch::SetVoltages() {
    socket->Send( "V1 28" );
    socket->Send( "V2 28" );
}
