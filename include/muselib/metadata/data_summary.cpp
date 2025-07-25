#include "data_summary.h"

#include <fstream>
#include <iostream>

namespace MUSE
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void DataSummary::setSummary(const MUSE::Data &data)
{
    this->n_samples = data.text_values.size();

    std::cout << std::endl;
    std::cout << "Name: " << data.getName() << "; Unit: " << data.getUnit() << "; Flag: " << data.getFlag() << "; Description: " << data.getDescription() << std::endl;
    std::cout << std::endl;

    std::cout << "Data summary ..." << std::endl;
    std::cout << "Number of samples: " << this->n_samples << std::endl;

    this->n_nd_values = count_ndvalues(data.text_values);
    this->n_na_values = count_navalues(data.text_values);
    this->n_allowed_symbols = count_allowedsymbol(data.text_values);
    this->n_positive_values = count_posvalues(data.text_values);
    this->n_negative_values = count_negvalues(data.text_values);
    this->n_empty = count_empty(data.text_values);


    std::cout << "Number of not available (NA) values: " << this->n_na_values << std::endl;
    std::cout << "Number of not detected (nd) values: " <<  this->n_nd_values << std::endl;
    std::cout << "Number of allowed symbols: " <<           this->n_allowed_symbols << std::endl;
    std::cout << "Number of positive values: " <<           this->n_positive_values << std::endl;
    std::cout << "Number of negative values: " <<           this->n_negative_values << std::endl;
    std::cout << "Number of empty cells: " <<               this->n_empty << std::endl;


    if(data.getFlag() == "A" || data.getFlag() == "R") //ammetto numeri negativi come VALIDI!
        this->n_valid_values = this->n_samples - this->n_na_values - this->n_nd_values - this->n_empty - this->n_allowed_symbols;

    else
        this->n_valid_values = this->n_samples - this->n_na_values - this->n_nd_values - this->n_empty - this->n_allowed_symbols - this->n_negative_values;

    std::cout << "Number of valid values: " << this->n_valid_values << std::endl;
    std::cout << "###################################" << std::endl;
}

bool DataSummary::read(const std::string filename)
{
    return readConfFileJSON(filename);
}

bool DataSummary::write(const std::string filename)
{
    return writeConfFileJSON(filename);
}

bool DataSummary::readConfFileJSON(const std::string filename)
{
#ifdef MUSE_USES_CEREAL
    std::ifstream ss (filename.c_str(), std::ifstream::in);

    if (!ss.is_open())
    {
        std::cerr << "[ERROR] Error in opening " << filename << "." << std::endl;
        std::cerr << "[ERROR] Configuration file cannot be read from disk." << std::endl;
        return false;
    }

    cereal::JSONInputArchive archive (ss);
    deserialize(archive);
    ss.close();

    return true;
#else
    std::cerr << "CEREAL Library is required to load an instrument from file." << std::endl;
    return false;
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


bool DataSummary::writeConfFileJSON (const std::string filename)
{
#ifdef MUSE_USES_CEREAL
    std::ofstream ss (filename.c_str(), std::ofstream::out);

    if (!ss.is_open())
    {
        std::cerr << "[ERROR] Error in opening " << filename << "." << std::endl;
        std::cerr << "[ERROR] Configuration file cannot be written on disk." << std::endl;
        return false;
    }

    {
        cereal::JSONOutputArchive archive (ss);
        serialize(archive);
    }
    ss.close();

    return true;
#else
    std::cerr << "CEREAL Library is required to save an instrument on file." << std::endl;
    return false;
#endif
}

}
