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
#include "stepper_motor.h"

StepperMotor::StepperMotor(std::string ip_addr,
                           uint port_number,
                           QObject *parent) : AbstractIntermittenSocket( ip_addr, port_number, parent ) {}


void StepperMotor::TuneToLength(double desired_length, double current_length ) {

    double delta_l = desired_length - current_length;
    int delta_steps = static_cast<int>( std::round( 16.0*200*delta_l ) );

    std::string command = "FL";
    command += boost::lexical_cast<std::string>( delta_steps );

    socket->OpenConnection();
    SetUpStepperMotor( 5.0 );

    socket->SendScl( command );

    double delay_time = std::abs( delta_l );
    sleep( 2.0*delay_time );

    socket->CloseConnection();
}

void StepperMotor::TuneCavity( double length_of_tune ) {

    int revs = static_cast<int>( length_of_tune*16.0 );

    int number_steps = revs*200;

    std::string command = "FL";
    command += boost::lexical_cast<std::string>( number_steps );

    socket->OpenConnection();
    SetUpStepperMotor( 5.0 );

    socket->SendScl( command );

    double delay_time = std::abs( length_of_tune );
    sleep( 2.0*delay_time );

    socket->CloseConnection();
}

void StepperMotor::PanicResetCavity( uint iteration, double revs_per_iter ) {

    int revs = -1*iteration*static_cast<int>(revs_per_iter);
    int number_steps = revs*200;

    std::string command = "FL";
    command += boost::lexical_cast<std::string>( number_steps );

    socket->OpenConnection();
    SetUpStepperMotor( 5.0 );

    socket->SendScl( command );

    socket->CloseConnection();
}

void StepperMotor::TuningLoop( double len_of_tune, double revs, uint iters ) {

    int iteration_steps = static_cast<int>( revs*200 );

    std::string command = "FL";
    command += boost::lexical_cast<std::string>( iteration_steps );

    socket->OpenConnection();

    socket->SendScl( command );

    socket->CloseConnection();
    sleep( 2.0*revs );
}

//Private Functions
void StepperMotor::SetUpStepperMotor( double traverse_speed ) {

    std::string trav_speed_str = boost::lexical_cast<std::string>( traverse_speed );

    //Set resolution to 200 steps/rev
    socket->SendScl( "MR0" );
    //Set acceleration
    socket->SendScl( "AC" + trav_speed_str );
    //Set deceleration
    socket->SendScl( "DE" + trav_speed_str );
    //Set velocity
    socket->SendScl( "VE" + trav_speed_str );

}
