//Header for this file
#include "flatfilesaver.h"
//C System-Headers
//
//C++ System headers
#include <QDateTime>
#include <iostream>
#include <fstream>
//Qt Headers
//
//OpenCV Headers
//
//Boost Headers
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
//Project specific headers
//

FlatFileSaver::FlatFileSaver( std::string save_file_path ) {
    root_dir = save_file_path + GetFolderName();
    MakeEmptyDataFolder( root_dir );
}

void FlatFileSaver::Save( const std::vector< data_triple<double> >& data_values, std::string header ) {

    std::string save_file = GenerateSaveFileName( counter );

    std::ofstream config_file;
    config_file.precision( 10 );

    config_file.open ( save_file, std::ofstream::out );

    config_file << header << std::endl;
    config_file << "@" << std::endl;

    for( const auto& data_elem : data_values ) {
        config_file << data_elem << std::endl;
    }

    config_file.close();

    counter ++;
}

//Protected functions
std::string FlatFileSaver::GenerateSaveFileName( uint index ) {

    std::string save_file = root_dir;
    save_file += "/";
    save_file += "POWER_SPECTRUM";
    save_file += boost::lexical_cast<std::string>( index );
    save_file += ".csv";

    return save_file;

}

//Private functions
void FlatFileSaver::MakeEmptyDataFolder( std::string data_folder ) {

    const char* path = data_folder.c_str();

    boost::filesystem::path dir(path);

    if( boost::filesystem::create_directory(dir) ) {

        std::cout << "Directory Created: " << data_folder << std::endl;

    }

}

std::string FlatFileSaver::GetFolderName() {

    QDateTime time_stamp = QDateTime::currentDateTime();
    QString format_str = "yyyy:MM:dd_HH:mm:ss";

    QString time_str = time_stamp.toString( format_str );

    return time_str.toUtf8().constData();

}
