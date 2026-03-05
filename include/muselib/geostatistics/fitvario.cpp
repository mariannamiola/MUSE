#include "fitvario.h"

#include <limits.h>

#include "muselib/colors.h"
#include "muselib/geostatistics/utils.h"



// //////////////////////////////////////////
// //////////////////////////////////////////
// Get gamma from h and model parameters
// //////////////////////////////////////////
/// \brief get_gamma
/// \param h
/// \param r represents effective range (length at which spatial dependence is present)
/// \param c0 represents nugget
/// \param c represents sill
/// \param model_type
/// \return
///
double get_gamma (const double &h, const double &r, const double &c0, const double &c, variogram_type model_type)
{
    double gamma_h = 0.0;

    if(h < 0)
    {
        std::cout << FRED("ERROR: The variogram is a positive function. Negative or null lag cannot be accepeted.") << std::endl;
        exit(1);
    }

    switch (model_type)
    {
        case variogram_type::SPHERIC:
        {
            double a = r; //for spherical, range parameter a is equal to effective range r
            if(h <= a)
                gamma_h = c * ( (3 * h) / (2 * a) - 0.5 * pow(h / a, 3) ) + c0;
            else
                gamma_h = c + c0;

            break;
        }
        case variogram_type::GAUSSIAN:
        {
            double a = r/(sqrt(3)); //for gaussian, range parameter a is equal to 1/2 r
            //std::cout << "Effective range is: " << a << "; range parameter is assumed equal to " << a_par << std::endl;

            gamma_h = c * ( 1 - exp(-1 * pow((h / a), 2))) + c0;
            break;
        }
        case variogram_type::EXPONENTIAL:
        {
            double a = r/3; //for exponential, range parameter a is equal to 1/3 r
            //std::cout << "Effective range is: " << a << "; range parameter is assumed equal to " << a_par << std::endl;

            gamma_h =  c * ( 1 - exp(-(h/a))) + c0;
            break;
        }
        case variogram_type::LINEAR:
        {
            gamma_h = c * h + c0;
            break;
        }
        case variogram_type::DEFAULT:
        {
            break;
        }
    }

    return gamma_h;
}

double get_gamma (const double &h, const double &a, const double &c0, const double &c, add_variogram_type model_type)
{
    double gamma_h = 0.0;

    if(h < 0)
    {
        std::cout << "ERROR: The variogram is a positive function. Negative or null lag cannot be accepeted." << std::endl;
        exit(1);
    }

    switch (model_type)
    {
        case add_variogram_type::NUGGET:
        {
            if(h == 0)
                gamma_h = c * 0 + c0;
            else
                gamma_h = c + c0;

            break;
        }
        case add_variogram_type::LINEAR_WITH_SILL:
        {
            if(h>= 0 && h <= a)
                gamma_h = c * (h/a) + c0;
            else if (h>a)
                gamma_h = c + c0;

            break;
        }
        case add_variogram_type::CIRCULAR:
        {
            if(h>= 0 && h <= a) //continuareeeeeeeeeeeee
                gamma_h = (2*h)/(M_PI * a);
            else if (h>a)
                gamma_h = c + c0;

            break;
        }

        case add_variogram_type::HOLE_EFFECT:
        {
            double fac = 1.0 - cos(M_PI * (h/a));
            gamma_h = c * fac;

            break;
        }

    }

    return gamma_h;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// helper for fitting weights
static double compute_weight(const exp_variog &ev, size_t idx, weightsType w_type)
{
    double h = ev.h.at(idx);
    double N = ev.N.at(idx);
    double gamma = ev.gamma.at(idx);

    // Soglie per evitare divisioni per zero / pesi esplosivi
    //constexpr double H_MIN     = 1.0;    // distanza minima "sicura" (stessa unità di h)
    //const double GAMMA_MIN = 1e-6;   // variogramma minimo "sicuro"
    const double THRESH = 1e-6;    // soglia per evitare divisioni per zero o pesi troppo grandi

    double w = 0.0;
    switch(w_type)
    {
        // -------------------------------------------------------------------
        // Cressie (1985) WLS standard: w_i = N_i / γ(h_i)²
        // Downweights i lag con variogramma alto (molto variabile)
        // -------------------------------------------------------------------
        case weightsType::CRESSIE:
        {
            const double gamma = std::max(gamma, THRESH);
            w = N / (gamma * gamma);
            break;
        }
        
        // -------------------------------------------------------------------
        // Cressie pesato per distanza: w_i = N_i / h_i²
        // Utile quando si vuole privilegiare i lag vicini.
        // -------------------------------------------------------------------
        case weightsType::CRESSIE_WEIGHTED:
        {
            w = N / (h * h + THRESH);
            break;
        }

        // -------------------------------------------------------------------
        // Variante lineare in h: w_i = N_i / h_i
        // Meno aggressiva di CRESSIE_WEIGHTED nel penalizzare i lag lontani.
        // -------------------------------------------------------------------
        case weightsType::CRESSIE_WEIGHTED_MODIFIED:
        {
            w = N / (h + THRESH);
            break;
        }
        default:
            w = 1.0; // peso uniforme
            break;
    }
    return w;
}




//FITTING - OMNIDIRECTIONAL

/// @brief fitting experimental variogram: solution with minimum error varying on range, nugget and model type
/// @param ev experimental variogram
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param nugget_precision nugget precision (number of nugget values to test between min and max nugget)
/// @param w_type weights type to use for fitting (Cressie, Cressie weighted, Cressie weighted modified)
/// @return 
variogram fit_variogram (const exp_variog &ev, const double &range_precision, const double &nugget_precision, weightsType w_type)
{
    variogram fitvario; //risultato finale

    std::vector<variogram_type> model_types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL}; //vettore dei variogramma modello

    std::vector<MUSE::VarioError> fitvario_on_model; //fit vario:

    double min_nugget = 0.0;
    double max_nugget = 1.0; //per variabili raw/indicatori: varianza!!

    for(size_t m=0; m<model_types.size(); m++)
    {
        std::vector<MUSE::VarioError> fitvario_on_nug; //fit vario on nugget -> variogramma modello al variare del nugget
        fitvario_on_nug.clear();

        //ciclo sui valori di nugget
        for(double nugget = min_nugget; nugget < max_nugget; nugget = nugget + max_nugget/nugget_precision)
        {
            MUSE::VarioError fitvario_on_range; //fit vario on range -> variogramma modello al variare dei range

            double c0 = nugget;
            double c = 1.0 - c0;

            double max_range = ev.h.at(ev.h.size()-1);
            double min_range = ev.h.at(0);

            double best_mse_range = DBL_MAX;
            double best_r = 0.0;
            std::string best_type;

            // Inizializzazione di out a 1 (asintoto variogramma: max valore)
            vector<double> out (ev.gamma.size(), 1.0);

            //ciclo sui range
            for(double r = min_range; r<max_range; r = r + max_range/range_precision)
            {
                for(size_t i=0; i<ev.h.size(); i++)
                    out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_types.at(m));

                double mse = 0.0;
                for(unsigned int i=0; i<out.size(); i++)
                {
                    if (ev.h.at(i) > 0)
                    {
                        double weight_coefficient = compute_weight(ev, i, w_type);
                        mse += pow((out.at(i) - ev.gamma.at(i)),2) * weight_coefficient;
                    }
                }

                if(mse < best_mse_range)
                {
                    best_mse_range = mse;
                    best_r = r;
                    convert_to_str(best_type, model_types.at(m));
                }
            }

            // Soluzione a minimo errore (variando sul range) per un valore di fissato di nugget e per un modello fissato
            fitvario_on_range.vario.type = best_type;
            fitvario_on_range.vario.sill = c+c0;
            fitvario_on_range.vario.range = best_r;
            fitvario_on_range.vario.nugget = c0;
            fitvario_on_range.mse = best_mse_range;

            fitvario_on_nug.push_back(fitvario_on_range);
        }

        // Trova la soluzione a minimo errore variando sul nugget
        int min_index;
        double min_mse = DBL_MAX;
        for(size_t i=0; i< fitvario_on_nug.size(); i++)
        {
            if(fitvario_on_nug.at(i).mse < min_mse)
            {
                min_mse = fitvario_on_nug.at(i).mse;
                min_index = i;
            }
        }

        fitvario_on_model.push_back(fitvario_on_nug.at(min_index));
    }

    int min_index_model;
    double min_mse_model = DBL_MAX;
    for(size_t i=0; i< fitvario_on_model.size(); i++)
    {
        if(fitvario_on_model.at(i).mse < min_mse_model)
        {
            if(fitvario_on_model.at(i).vario.type.compare("Gau") == 0)
            {
                std::cout << "### Variogram model is Gaussian. Check on nugget value ..." << std::endl;
                if(fitvario_on_model.at(i).vario.nugget > 0.05 * fitvario_on_model.at(i).vario.sill)
                {
                    std::cout << "WARNING: Nugget is major than 5% of total sill" << std::endl;
                    std::cout << "Model: " << fitvario_on_model.at(i).vario.type << " | Nugget: " << fitvario_on_model.at(i).vario.nugget << " | Sill: " << fitvario_on_model.at(i).vario.sill << " | 5% sill: " << 0.05*fitvario_on_model.at(i).vario.sill << std::endl;
                    continue;
                }
            }
            else
            {
                min_mse_model = fitvario_on_model.at(i).mse;
                min_index_model = i;
            }
        }
    }

    fitvario.range = fitvario_on_model.at(min_index_model).vario.range;
    fitvario.nugget = fitvario_on_model.at(min_index_model).vario.nugget;
    fitvario.sill = fitvario_on_model.at(min_index_model).vario.sill;
    fitvario.type = fitvario_on_model.at(min_index_model).vario.type;


    cout<<endl;
    cout<<"####################################"<<endl;
    cout<<"###### FITTED VARIOGRAM MODEL ######"<<endl;
    cout<<"####################################"<<endl;
    cout<<"Model Type: "<<fitvario.type<<endl;
    cout<<"Sill: "<<fitvario.sill<<endl;
    cout<<"Nugget: "<<fitvario.nugget<<endl;
    cout<<"Range: "<<fitvario.range<<endl;
    cout<<"####################################"<<endl;
    cout<<"####################################"<<endl;
    cout<<endl;

    return fitvario;
}


/// @brief fitting experimental variogram: solution with minimum error varying on range and nugget - model type fixed
/// @param ev experimental variogram
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param nugget_precision nugget precision (number of nugget values to test between min and max nugget)
/// @param model_type model type to use for fitting (Spherical, Gaussian, Exponential, Linear)
/// @param w_type weights type to use for fitting (Cressie, Cressie weighted, Cressie weighted modified)
/// @return 
variogram fit_variogram (const exp_variog &ev, const double &range_precision, const double &nugget_precision, variogram_type model_type, weightsType w_type)
{
    variogram fitvario; //risultato finale

    std::vector<MUSE::VarioError> fitvario_on_nug; //fit vario on nugget -> variogramma modello al variare del nugget
    fitvario_on_nug.clear();

    double min_nugget = 0.0;
    double max_nugget = 1.0; //per variabili raw/indicatori: varianza!!

    //ciclo sui valori di nugget
    for(double nugget = min_nugget; nugget < max_nugget; nugget = nugget + max_nugget/nugget_precision)
    {
        MUSE::VarioError fitvario_on_range; //fit vario on range -> variogramma modello al variare dei range

        double c0 = nugget;
        double c = 1.0 - c0;

        double max_range = ev.h.at(ev.h.size()-1);
        double min_range = ev.h.at(0);

        double best_mse_range = DBL_MAX;
        double best_r = 0.0;
        std::string best_type ="";

        // Inizializzazione di out a 1 (asintoto variogramma: max valore)
        vector<double> out (ev.gamma.size(), 1.0);

        //ciclo sui range
        for(double r = min_range; r<max_range; r = r + max_range/range_precision)
        {
            for(size_t i=0; i<ev.h.size(); i++)
                out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

            double mse = 0.0;
            for(unsigned int i=0; i<out.size(); i++)
            {
                if (ev.h.at(i) > 0)
                {
                    double weight_coefficient = 0.0;
                        switch (w_type)
                        {
                        case weightsType::CRESSIE:
                            weight_coefficient = ev.N.at(i);
                            break;
                        case weightsType::CRESSIE_WEIGHTED:
                            weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),2));
                            break;
                        case weightsType::CRESSIE_WEIGHTED_MODIFIED:
                            weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),1));
                            break;
                        default:
                            weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),1)); //equal to CRESSIE_WEIGHTED_MODIFIED
                            break;
                        }
                        mse += pow((out.at(i) - ev.gamma.at(i)),2) * weight_coefficient;
                }
            }

            if(mse < best_mse_range)
            {
                best_mse_range = mse;
                best_r = r;
                convert_to_str(best_type, model_type);
            }
        }

        // Soluzione a minimo errore (variando sul range) per un valore di fissato di nugget e per un modello fissato
        fitvario_on_range.vario.type = best_type;
        fitvario_on_range.vario.sill = c+c0;
        fitvario_on_range.vario.range = best_r;
        fitvario_on_range.vario.nugget = c0;
        fitvario_on_range.mse = best_mse_range;

        fitvario_on_nug.push_back(fitvario_on_range);
    }

    // Trova la soluzione a minimo errore variando sul nugget
    int min_index;
    double min_mse = DBL_MAX;
    for(size_t i=0; i< fitvario_on_nug.size(); i++)
    {
        if(fitvario_on_nug.at(i).mse < min_mse)
        {
            min_mse = fitvario_on_nug.at(i).mse;
            min_index = i;
        }
    }

    fitvario = fitvario_on_nug.at(min_index).vario;

    cout<<endl;
    cout<<"####################################"<<endl;
    cout<<"###### FITTED VARIOGRAM MODEL ######"<<endl;
    cout<<"####################################"<<endl;
    cout<<"Model Type: "<<fitvario.type<<endl;
    cout<<"Sill: "<<fitvario.sill<<endl;
    cout<<"Nugget: "<<fitvario.nugget<<endl;
    cout<<"Range: "<<fitvario.range<<endl;
    cout<<"####################################"<<endl;
    cout<<"####################################"<<endl;
    cout<<endl;

    return fitvario;
}



/// @brief fitting experimental variogram: solution with minimum error varying on range and model type - nugget fixed
/// @param ev experimental variogram
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param nugget nugget value to fix for fitting
/// @param w_type weights type to use for fitting (Cressie, Cressie weighted, Cressie weighted modified)
/// @return 
variogram fit_variogram_1par (const exp_variog &ev, const double &range_precision, const double &nugget, weightsType w_type)
{
    variogram fitvario; //risultato finale

    std::vector<variogram_type> model_types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL}; //vettore dei variogramma modello

    std::vector<MUSE::VarioError> fitvario_on_model; //fit vario:

    for(size_t m=0; m<model_types.size(); m++)
    {
        MUSE::VarioError fitvario_on_range; //fit vario on range -> variogramma modello al variare dei range

        double c0 = nugget;
        double c = 1.0 - c0;

        double max_range = ev.h.at(ev.h.size()-1);
        double min_range = ev.h.at(0);

        double best_mse_range = DBL_MAX;
        double best_r = 0.0;
        std::string best_type = "";

        // Inizializzazione di out a 1 (asintoto variogramma: max valore)
        vector<double> out (ev.gamma.size(), 1.0);

        //ciclo sui range
        for(double r = min_range; r<max_range; r = r + max_range/range_precision)
        {
            for(size_t i=0; i<ev.h.size(); i++)
                out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_types.at(m));

            double mse = 0.0;
            for(unsigned int i=0; i<out.size(); i++)
            {
                if (ev.h.at(i) > 0)
                {
                    double weight_coefficient = 0.0;
                    switch (w_type)
                    {
                    case weightsType::CRESSIE:
                        weight_coefficient = ev.N.at(i);
                        break;
                    case weightsType::CRESSIE_WEIGHTED:
                        weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),2));
                        break;
                    case weightsType::CRESSIE_WEIGHTED_MODIFIED:
                        weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),1));
                        break;
                    default:
                        weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),1)); //equal to CRESSIE_WEIGHTED_MODIFIED
                        break;
                    }
                    mse += pow((out.at(i) - ev.gamma.at(i)),2) * weight_coefficient;
                }
            }

            if(mse < best_mse_range)
            {
                best_mse_range = mse;
                best_r = r;
                convert_to_str(best_type, model_types.at(m));
            }
        }

        // Soluzione a minimo errore (variando sul range) per un valore di fissato di nugget e per un modello fissato
        fitvario_on_range.vario.type = best_type;
        fitvario_on_range.vario.sill = c+c0;
        fitvario_on_range.vario.range = best_r;
        fitvario_on_range.vario.nugget = c0;
        fitvario_on_range.mse = best_mse_range;

        fitvario_on_model.push_back(fitvario_on_range);
    }


    int min_index_model;
    double min_mse_model = DBL_MAX;
    for(size_t i=0; i< fitvario_on_model.size(); i++)
    {
        if(fitvario_on_model.at(i).mse < min_mse_model)
        {
            if(fitvario_on_model.at(i).vario.type.compare("Gau") == 0)
            {
                std::cout << "### Variogram model is Gaussian. Check on nugget value ..." << std::endl;
                if(fitvario_on_model.at(i).vario.nugget > 0.05 * fitvario_on_model.at(i).vario.sill)
                {
                    std::cout << "WARNING: Nugget is major than 5% of total sill" << std::endl;
                    std::cout << "Model: " << fitvario_on_model.at(i).vario.type << " | Nugget: " << fitvario_on_model.at(i).vario.nugget << " | Sill: " << fitvario_on_model.at(i).vario.sill << " | 5% sill: " << 0.05*fitvario_on_model.at(i).vario.sill << std::endl;
                    continue;
                }
            }
            else
            {
                min_mse_model = fitvario_on_model.at(i).mse;
                min_index_model = i;
            }
        }
    }

    fitvario.range = fitvario_on_model.at(min_index_model).vario.range;
    fitvario.nugget = fitvario_on_model.at(min_index_model).vario.nugget;
    fitvario.sill = fitvario_on_model.at(min_index_model).vario.sill;
    fitvario.type = fitvario_on_model.at(min_index_model).vario.type;


    cout<<endl;
    cout<<"####################################"<<endl;
    cout<<"###### FITTED VARIOGRAM MODEL ######"<<endl;
    cout<<"####################################"<<endl;
    cout<<"Model Type: "<<fitvario.type<<endl;
    cout<<"Sill: "<<fitvario.sill<<endl;
    cout<<"Nugget: "<<fitvario.nugget<<endl;
    cout<<"Range: "<<fitvario.range<<endl;
    cout<<"####################################"<<endl;
    cout<<"####################################"<<endl;
    cout<<endl;

    return fitvario;
}


/// @brief fitting experimental variogram: solution with minimum error varying on range - nugget and model type fixed
/// @param ev experimental variogram
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param model_type model type to use for fitting (Spherical, Gaussian, Exponential, Linear)
/// @param nugget nugget value to fix for fitting
/// @param is_print boolean to print or not the fitted variogram parameters
/// @param w_type weights type to use for fitting (Cressie, Cressie weighted, Cressie weighted modified)
/// @return 
variogram fit_variogram (const exp_variog &ev, const double &range_precision, variogram_type model_type, const double &nugget, bool is_print, weightsType w_type)
{
    variogram fitvario; //risultato finale

    double c0 = nugget;
    double c = 1.0 - c0;

    double max_range = ev.h.at(ev.h.size()-1);
    double min_range = ev.h.at(0);

    double best_mse_range = DBL_MAX;
    double best_r = 0.0;
    std::string best_type = "";

    // Inizializzazione di out a 1 (asintoto variogramma: max valore)
    vector<double> out (ev.gamma.size(), 1.0);

    //ciclo sui range
    for(double r = min_range; r<max_range; r = r + max_range/range_precision)
    {
        for(size_t i=0; i<ev.h.size(); i++)
            out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

        double mse = 0.0;
        for(unsigned int i=0; i<out.size(); i++)
        {
            if (ev.h.at(i) > 0)
            {
                double weight_coefficient = 0.0;
                switch (w_type)
                {
                case weightsType::CRESSIE:
                    weight_coefficient = ev.N.at(i);
                    break;
                case weightsType::CRESSIE_WEIGHTED:
                    weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),2));
                    break;
                case weightsType::CRESSIE_WEIGHTED_MODIFIED:
                    weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),1));
                    break;
                default:
                    weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),1)); //equal to CRESSIE_WEIGHTED_MODIFIED
                    break;
                }
                mse += pow((out.at(i) - ev.gamma.at(i)),2) * weight_coefficient;
            }
        }

        if(mse < best_mse_range)
        {
            best_mse_range = mse;
            best_r = r;
            convert_to_str(best_type, model_type);
        }
    }

    // Soluzione per un valore di nugget
    fitvario.type = best_type;
    fitvario.sill = c+c0;
    fitvario.range = best_r;
    fitvario.nugget = c0;


    if(is_print == true)
    {
        cout<<endl;
        cout<<"####################################"<<endl;
        cout<<"###### FITTED VARIOGRAM MODEL ######"<<endl;
        cout<<"####################################"<<endl;
        cout<<"Model Type: "<<fitvario.type<<endl;
        cout<<"Sill: "<<fitvario.sill<<endl;
        cout<<"Nugget: "<<fitvario.nugget<<endl;
        cout<<"Range: "<<fitvario.range<<endl;
        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return fitvario;
}


/// @brief fitting experimental variogram: solution with minimum error varying on range and nugget - model type fixed - output with error (mse)
/// @param ev experimental variogram
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param nugget_precision nugget precision (number of nugget values to test between min and max nugget)
/// @param model_type model type to use for fitting (Spherical, Gaussian, Exponential, Linear)
/// @param is_print boolean to print or not the fitted variogram parameters
/// @param w_type weights type to use for fitting (Cressie, Cressie weighted, Cressie weighted modified)
/// @return 
MUSE::VarioError fit_variogram_mse (const exp_variog &ev, const double &range_precision, const double &nugget_precision, variogram_type model_type, bool is_print, weightsType w_type)
{
    MUSE::VarioError fitvario; //risultato finale

    double min_nugget = 0.0;
    double max_nugget = 1.0;

    std::vector<MUSE::VarioError> fitvario_on_nug; //fit vario on nugget -> variogramma modello al variare del nugget
    fitvario_on_nug.clear();

    //ciclo sui valori di nugget
    for(double nugget = min_nugget; nugget < max_nugget; nugget = nugget + max_nugget/nugget_precision)
    {
        MUSE::VarioError fitvario_on_range; //fit vario: f(r, fixed nugget)

        double c0 = nugget;
        double c = 1.0-c0;

        double max_range = ev.h.at(ev.h.size()-1);
        double min_range = ev.h.at(0);

        double best_mse_range = DBL_MAX;
        double best_r = 0.0;
        std::string best_type ="";

        // Inizializzazione di out a 1 (asintoto variogramma: max valore)
        vector<double> out (ev.gamma.size(), 1.0);

        //ciclo sui range
        for(double r = min_range; r<max_range; r = r + max_range/range_precision)
        {
            for(size_t i=0; i<ev.h.size(); i++)
                out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

            double mse = 0.0;

            for(unsigned int i=0; i<out.size(); i++)
            {
                if (ev.h.at(i) > 0)
                {
                    double weight_coefficient = 0.0;
                    switch (w_type)
                    {
                    case weightsType::CRESSIE:
                        weight_coefficient = ev.N.at(i);
                        break;
                    case weightsType::CRESSIE_WEIGHTED:
                        weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),2));
                        break;
                    case weightsType::CRESSIE_WEIGHTED_MODIFIED:
                        weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),1));
                        break;
                    default:
                        weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),1)); //equal to CRESSIE_WEIGHTED_MODIFIED
                        break;
                    }
                    mse += pow((out.at(i) - ev.gamma.at(i)),2) * weight_coefficient;
                }
            }

            if(mse < best_mse_range)
            {
                best_mse_range = mse;
                best_r = r;
                convert_to_str(best_type, model_type);
            }
        }

        // Soluzione per un valore di nugget
        fitvario_on_range.vario.type = best_type;
        fitvario_on_range.vario.sill = c+c0;
        fitvario_on_range.vario.range = best_r;
        fitvario_on_range.vario.nugget = c0;
        fitvario_on_range.mse = best_mse_range;

        fitvario_on_nug.push_back(fitvario_on_range);
    }

    int min_index;
    double min_mse = DBL_MAX;
    for(size_t i=0; i< fitvario_on_nug.size(); i++)
    {
        if(fitvario_on_nug.at(i).mse < min_mse)
        {
            min_mse = fitvario_on_nug.at(i).mse;
            min_index = i;
        }
    }

    fitvario.vario.range = fitvario_on_nug.at(min_index).vario.range;
    fitvario.vario.nugget = fitvario_on_nug.at(min_index).vario.nugget;
    fitvario.vario.sill = fitvario_on_nug.at(min_index).vario.sill;
    fitvario.vario.type = fitvario_on_nug.at(min_index).vario.type;
    fitvario.mse = fitvario_on_nug.at(min_index).mse;

    if(is_print == true)
    {
        cout<<endl;
        cout<<"####################################"<<endl;
        cout<<"###### FITTED VARIOGRAM MODEL ######"<<endl;
        cout<<"####################################"<<endl;
        cout<<"Model Type: "<< fitvario.vario.type<<endl;
        cout<<"Sill: "<< fitvario.vario.sill<<endl;
        cout<<"Nugget: "<< fitvario.vario.nugget<<endl;
        cout<<"Range: "<< fitvario.vario.range<<endl;
        cout<<"Mean Squared Error: "<<fitvario.mse<<endl;
        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return fitvario;
}


/// @brief fitting experimental variogram: solution with minimum error varying on range - nugget and model type fixed - output with error (mse)
/// @param ev experimental variogram
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param nugget nugget value to fix for fitting
/// @param model_type model type to use for fitting (Spherical, Gaussian, Exponential, Linear)
/// @param is_print boolean to print or not the fitted variogram parameters
/// @param w_type weights type to use for fitting (Cressie, Cressie weighted, Cressie weighted modified)
/// @return 
MUSE::VarioError fit_variogram_mse_1par (const exp_variog &ev, const double &range_precision, const double &nugget, variogram_type model_type, bool is_print, weightsType w_type)
{
    MUSE::VarioError fitvario; //risultato finale

    //ciclo sui valori di nugget
    //MUSE::VarioError fitvario_on_range; //fit vario: f(r, fixed nugget)

    double c0 = nugget;
    double c = 1.0 - c0;

    double max_range = ev.h.at(ev.h.size()-1);
    double min_range = ev.h.at(0);

    double best_mse_range = DBL_MAX;
    double best_r = 0.0;
    std::string best_type;

    // Inizializzazione di out a 1 (asintoto variogramma: max valore)
    vector<double> out (ev.gamma.size(), 1.0);

    //ciclo sui range
    for(double r = min_range; r<max_range; r = r + max_range/range_precision)
    {
        for(size_t i=0; i<ev.h.size(); i++)
            out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

        double mse = 0.0;
        for(unsigned int i=0; i<out.size(); i++)
        {
            if (ev.h.at(i) > 0)
            {
                double weight_coefficient = 0.0;
                switch (w_type)
                {
                case weightsType::CRESSIE:
                    weight_coefficient = ev.N.at(i);
                    break;
                case weightsType::CRESSIE_WEIGHTED:
                    weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),2));
                    break;
                case weightsType::CRESSIE_WEIGHTED_MODIFIED:
                    weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),1));
                    break;
                default:
                    weight_coefficient = (ev.N.at(i) / pow((ev.h.at(i)),1)); //equal to CRESSIE_WEIGHTED_MODIFIED
                    break;
                }
                mse += pow((out.at(i) - ev.gamma.at(i)),2) * weight_coefficient;
            }
        }

        if(mse < best_mse_range)
        {
            best_mse_range = mse;
            best_r = r;
            convert_to_str(best_type, model_type);
        }
    }

    // Soluzione per un valore di nugget
    fitvario.vario.type = best_type;
    fitvario.vario.sill = c+c0;
    fitvario.vario.range = best_r;
    fitvario.vario.nugget = c0;
    fitvario.mse = best_mse_range;

    if(is_print == true)
    {
        cout<<endl;
        cout<<"####################################"<<endl;
        cout<<"###### FITTED VARIOGRAM MODEL ######"<<endl;
        cout<<"####################################"<<endl;
        cout<<"Model Type: "<< fitvario.vario.type<<endl;
        cout<<"Sill: "<< fitvario.vario.sill<<endl;
        cout<<"Nugget: "<< fitvario.vario.nugget<<endl;
        cout<<"Range: "<< fitvario.vario.range<<endl;
        cout<<"Mean Squared Error: "<<fitvario.mse<<endl;
        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return fitvario;
}


/// @brief loop on directions for fitting variograms with minimum error varying on range and nugget - model type fixed - output with error (mse) for each direction
/// @param dev vector of experimental variogram for each direction
/// @param degree_step step in degree to define the directions (e.g., 22.5° for 8 directions)
/// @param degree_tolerance tolerance in degree to assign pairs to a direction (e.g., 22.5°/2 for 8 directions)
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param nugget_precision nugget precision (number of nugget values to test between min and max nugget)
/// @param model_type model type to use for fitting (Spherical, Gaussian, Exponential, Linear)
/// @return 
vector<MUSE::VarioError> fit_dir_variogram (const std::vector<exp_variog> &dev, const double &degree_step, const double &degree_tolerance, const double &range_precision, const double &nugget_precision, variogram_type &model_type)
{
    // Funzione che richiama il fit_variogram (a modello fissato) e cicla per ogni direzione: funzione di supporto sulle direzioni

    vector<MUSE::VarioError> res_k (dev.size()); //VarioError per ogni direzione

    vector<double> seq;
    double n_directions = 180/degree_step;
    seq.resize(n_directions);

    seq[0]=0;
    for(uint i=1; i<seq.size();i++)
        seq[i] = seq[i-1] + degree_step;

    // Ciclo sulle direzioni e faccio il fitting a modello fissato + mse in output
    for(uint k = 0; k< n_directions; k++)
    {
        //TO DO: AGGIUNGERE CHECK SUL NUMERO DI COPPIE
        res_k.at(k) = fit_variogram_mse (dev.at(k), range_precision, nugget_precision, model_type);
    }

    return res_k;
}

/// @brief loop on directions for fitting variograms with minimum error varying on range and nugget - model type fixed - output with error (mse) for each direction
/// @param dev vector of experimental variogram for each direction
/// @param seq directions sequence (e.g., 0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5 for 8 directions)
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param nugget_precision nugget precision (number of nugget values to test between min and max nugget)
/// @param model_type model type to use for fitting (Spherical, Gaussian, Exponential, Linear)
/// @return 
vector<MUSE::VarioError> fit_dir_variogram (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, const double &nugget_precision, variogram_type &model_type)
{
    // Funzione che richiama il fit_variogram (a modello fissato) e cicla per ogni direzione: funzione di supporto sulle direzioni

    vector<MUSE::VarioError> res_k (dev.size()); //VarioError per ogni direzione

    int n_directions = seq.size();

    // Ciclo sulle direzioni e faccio il fitting a modello fissato + mse in output
    for(uint k = 0; k< n_directions; k++)
    {
        //TO DO: AGGIUNGERE CHECK SUL NUMERO DI COPPIE
        res_k.at(k) = fit_variogram_mse (dev.at(k), range_precision, nugget_precision, model_type);
    }

    return res_k;
}

/// @brief loop on directions for fitting variograms with minimum error varying on range - nugget and model type fixed - output with error (mse) for each direction
/// @param dev vector of experimental variogram for each direction
/// @param seq directions sequence (e.g., 0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5 for 8 directions)
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param nugget nugget value to fix for fitting
/// @param model_type model type to use for fitting (Spherical, Gaussian, Exponential, Linear)
/// @param is_print boolean to print or not the fitted variogram parameters for each direction
/// @return 
vector<MUSE::VarioError> fit_dir_variogram_1par (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, const double &nugget, variogram_type &model_type, bool is_print)
{
    // Funzione che richiama il fit_variogram (a modello fissato) e cicla per ogni direzione: funzione di supporto sulle direzioni

    vector<MUSE::VarioError> res_k (dev.size()); //VarioError per ogni direzione

    int n_directions = seq.size();

    // Ciclo sulle direzioni e faccio il fitting a modello fissato + mse in output
    for(uint k = 0; k< n_directions; k++)
    {
        //TO DO: AGGIUNGERE CHECK SUL NUMERO DI COPPIE
        res_k.at(k) = fit_variogram_mse_1par (dev.at(k), range_precision, nugget, model_type, is_print);
    }

    return res_k;
}


/// @brief loop on directions for fitting variograms with minimum error varying on range - nugget and model type fixed - with automatic selection of directions
/// @param dev vector of experimental variogram for each direction
/// @param degree_step step between directions (in degrees)
/// @param degree_tolerance tolerance for direction matching (in degrees)
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param model_type model type to use for fitting (Spherical, Gaussian, Exponential, Linear)
/// @param nugget nugget value to fix for fitting
/// @return vector of fitted variograms for each direction
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev, const double & degree_step, const double & degree_tolerance, const double &range_precision, variogram_type &model_type, double &nugget)
{
    vector<variogram> res_k (dev.size());

    vector<double> seq;
    double n_directions = 180/degree_step;
    seq.resize(n_directions);

    seq[0]=0;
    for(uint i=1; i<seq.size();i++)
        seq[i] = seq[i-1] + degree_step;

    for(uint k = 0; k< n_directions; k++)
    {
        //TO DO: AGGIUNGERE CHECK SUL NUMERO DI COPPIE
        res_k.at(k) = fit_variogram (dev.at(k), range_precision, model_type, nugget);
    }

    return res_k;
}


/// \brief loop on directions for fitting variograms with minimum error varying on range - nugget and model type fixed - with discrete directions
/// \param dev vector of experimental variogram for each direction
/// \param seq directions sequence (e.g., 0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5 for 8 directions)
/// \param range_precision range precision (number of range values to test between min and max range)
/// \param model_type model type to use for fitting (Spherical, Gaussian, Exponential, Linear)
/// \param nugget nugget value to fix for fitting
/// \param is_print boolean to print or not the fitted variogram parameters for each direction
/// \return vector of fitted variograms for each direction
///
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, variogram_type &model_type, const double &nugget, bool is_print)
{
    vector<variogram> res_k (dev.size());

    for(uint k = 0; k< seq.size(); k++)
        res_k.at(k) = fit_variogram (dev.at(k), range_precision, model_type, nugget, is_print);

    return res_k;
}


/// @brief fitting variogram for each direction with minimum error varying on range, nugget and model type - with automatic selection of directions - with selection of best model based on MSE on all directions and weighted average of nugget values for best model
/// @param dev directional experimental variogram (vector of experimental variogram for each direction)
/// @param degree_step degree step to define the directions (e.g., 22.5° for 8 directions)
/// @param degree_tolerance tolerance in degree to assign pairs to a direction (e.g., 22.5°/2 for 8 directions)
/// @param range_precision range precision (number of range values to test between min and max range)
/// @param nugget_precision nugget precision (number of nugget values to test between min and max nugget)
/// @param weigth weights for each direction on nugget averaging
/// @param is_print boolean to print or not the fitted variogram parameters for each direction
/// @return vector of fitted variograms for each direction
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev,const double & degree_step, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::vector<double> &weigth, bool is_print)
{
    // 0) Definisco il vettore dei modelli
    std::vector<variogram_type> types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL};
    std::vector<std::vector<MUSE::VarioError>> dir_vario (types.size());

    // 1) Ciclo sui modelli e calcolo il fit (a modello fissato) dei variogrammi direzionali
    for(size_t i=0; i<types.size(); i++)
        dir_vario.at(i) = fit_dir_variogram (dev, degree_step, degree_tolerance, range_precision, nugget_precision, types.at(i));

    // 2) Inizializza l'errore somma per ogni modello
    std::vector<double> sum_mse (types.size(), 0.0);
    for(size_t i=0; i<types.size(); i++)
    {
        for(size_t j=0; j<dir_vario.at(i).size(); j++)
            sum_mse.at(i) += dir_vario.at(i).at(j).mse; //sommo l'errore su tutte le direzioni per ogni tipo
    }

    // 3) Trova minimo errore e l'indice corrispondente
    double min_mse = DBL_MAX;
    int min_index = INT_MAX;
    for(size_t i=0; i< sum_mse.size(); i++)
    {
        if(sum_mse.at(i) < min_mse)
        {
            min_mse = sum_mse.at(i);
            min_index = i;
        }
    }

    std::string str_model;
    convert_to_str(str_model, types.at(min_index));
    std::cout << "Fit directional variogram model with minimum MSE on all directions: "<< str_model << "; Mean Squared Error: "<< sum_mse.at(min_index) << std::endl;

    // Media del nugget per il modello ad errore minimo (min_index)
    double avg_nugget = 0.0;
    double sum_weights = 0.0;
    for(size_t i=0; i<dir_vario.at(min_index).size(); i++)
    {
        std::cout << "### DIR: " << i << "; nugget: " << dir_vario.at(min_index).at(i).vario.nugget << "; assigned weight: " << weigth.at(i) << std::endl;
        avg_nugget += dir_vario.at(min_index).at(i).vario.nugget * weigth.at(i);
        sum_weights += weigth.at(i);
    }
    avg_nugget = avg_nugget/sum_weights; //(180/degree_step);

    //MEDIA PESATA SUL NUGGET IN BASE AGLI ZERI!!


    if(types.at(min_index) == variogram_type::GAUSSIAN)
    {
        std::cout << "### Variogram model is Gaussian. Check on nugget value ..." << std::endl;
        if(avg_nugget > 0.05 * 1.0) //sill=1.0
        {
            std::cout << "WARNING: Nugget is major than 5% of total sill" << std::endl;
            std::cout << "Model: " << str_model << " | Nugget: " << avg_nugget << " | Sill: " << 1.0 << " | 5% sill: " << 0.05*1.0 << std::endl;

            double min_mse_tmp = DBL_MAX;
            int min_index_tmp = INT_MAX;
            for(size_t i=0; i< sum_mse.size(); i++)
            {
                if((sum_mse.at(i) < min_mse_tmp) && (i != min_index))
                {
                    min_mse_tmp = sum_mse.at(i);
                    min_index_tmp = i;
                }
            }
            min_index = min_index_tmp;
            min_mse = min_mse_tmp;

            //ricalcolo
            sum_weights = 0.0;
            avg_nugget = 0.0;
            for(size_t i=0; i<dir_vario.at(min_index).size(); i++)
            {
                std::cout << "### (UPDATE) Nugget: " << dir_vario.at(min_index).at(i).vario.nugget << "; assigned weight: " << weigth.at(i) << std::endl;
                avg_nugget += dir_vario.at(min_index).at(i).vario.nugget * weigth.at(i);
                sum_weights += weigth.at(i);
            }
            avg_nugget = avg_nugget/sum_weights; //(180/degree_step);
        }
    }



    // 5) Ricalcolo il fitting a modello e nugget (medio sulle direzioni per il modello prescelto) fissato
    vector<variogram> vv = fit_dir_variogram (dev, degree_step, degree_tolerance, range_precision, types.at(min_index), avg_nugget);

    if(is_print == true)
    {
        int dir = 0;
        for(const variogram &v:vv)
        {
            cout<<endl;
            cout<<"################################################"<<endl;
            cout<<"###### FITTED DIRECTIONAL VARIOGRAM MODEL ######"<<endl;
            cout<<"###### Direction "<< dir << ": "<<dir * degree_step<<" +/- "<<degree_tolerance<<" ################"<<endl;
            cout<<"################################################"<<endl;
            cout<<"Model Type: "<<v.type<<endl;
            cout<<"Sill: "<<v.sill<<endl;
            cout<<"Nugget: "<<v.nugget<<endl;
            cout<<"Range: "<<v.range<<endl;
            cout<<"################################################"<<endl;
            cout<<"################################################"<<endl;
            cout<<endl;

            dir++;
        }
    }

    return vv; //variogramma sperimentale per ogni direzione

}



/// \brief fit_dir_variogram ranging on models and nugget - with directions and final average nugget
/// \param dev vector of experimental variogram for each direction
/// \param directions vector of directions (in degree) for each variogram in dev
/// \param degree_tolerance tolerance for the direction (in degree)
/// \param range_precision range precision for the fitting
/// \param nugget_precision nugget precision for the fitting
/// \param weigth weight assigned to each direction for the average nugget calculation
/// \param is_print boolean to print or not the fitted variogram for each direction
/// \return
///
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::vector<double> &weigth, bool is_print)
{
    // 0) Definisco il vettore dei modelli
    std::vector<variogram_type> types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL};
    std::vector<std::vector<MUSE::VarioError>> dir_vario (types.size());


    // 1) Ciclo sui modelli e calcolo il fit (a modello fissato) dei variogrammi direzionali
    for(size_t i=0; i<types.size(); i++)
        dir_vario.at(i) = fit_dir_variogram (dev, directions, range_precision, nugget_precision, types.at(i));


    // 2) Inizializza l'errore somma per ogni modello
    std::vector<double> sum_mse (types.size(), 0.0);
    for(size_t i=0; i<types.size(); i++)
    {
        for(size_t j=0; j<dir_vario.at(i).size(); j++)
            sum_mse.at(i) += dir_vario.at(i).at(j).mse; //sommo l'errore su tutte le direzioni per ogni tipo
    }

    // 3) Trova minimo errore e l'indice corrispondente
    double min_mse = DBL_MAX;
    int min_index = INT_MAX;
    for(size_t i=0; i< sum_mse.size(); i++)
    {
        if(sum_mse.at(i) < min_mse)
        {
            min_mse = sum_mse.at(i);
            min_index = i;
        }
    }

    std::string str_model;
    convert_to_str(str_model, types.at(min_index));
    std::cout << "Fit directional variogram model with minimum MSE on all directions: "<< str_model << "; Mean Squared Error: "<< sum_mse.at(min_index) << std::endl;


    // Media del nugget per il modello ad errore minimo (min_index)
    double avg_nugget = 0.0;
    double sum_weights = 0.0;
    for(size_t i=0; i<dir_vario.at(min_index).size(); i++)
    {
        std::cout << FMAG("### DIR: ") << i << FMAG("; nugget: ") << dir_vario.at(min_index).at(i).vario.nugget << FMAG("; assigned weight: ") << weigth.at(i) << std::endl;
        avg_nugget += dir_vario.at(min_index).at(i).vario.nugget * weigth.at(i);
        sum_weights += weigth.at(i);
    }
    avg_nugget = avg_nugget/sum_weights; //(directions.size());
    std::cout << std::endl;


    if(types.at(min_index) == variogram_type::GAUSSIAN)
    {
        std::cout << "### Variogram model is Gaussian! Check on nugget value ..." << std::endl;
        if(avg_nugget > 0.05 * 1.0) //sill=1.0
        {
            std::cout << "WARNING: Nugget is major than 5% of total sill" << std::endl;
            std::cout << "Model: " << str_model << " | Nugget: " << avg_nugget << " | Sill: " << 1.0 << " | 5% sill: " << 0.05*1.0 << std::endl;

            double min_mse_tmp = DBL_MAX;
            int min_index_tmp = INT_MAX;
            for(size_t i=0; i< sum_mse.size(); i++)
            {
                if((sum_mse.at(i) < min_mse_tmp) && (i != min_index))
                {
                    min_mse_tmp = sum_mse.at(i);
                    min_index_tmp = i;
                }
            }
            min_index = min_index_tmp;
            min_mse = min_mse_tmp;

            //ricalcolo avg_nugget per il nuovo modello successivo al gaussian
            sum_weights = 0.0;
            avg_nugget = 0.0;
            for(size_t i=0; i<dir_vario.at(min_index).size(); i++)
            {
                std::cout << FMAG("### (UPDATE) Nugget: ") << dir_vario.at(min_index).at(i).vario.nugget << FMAG("; assigned weight: ") << weigth.at(i) << std::endl;
                avg_nugget += dir_vario.at(min_index).at(i).vario.nugget * weigth.at(i);
                sum_weights += weigth.at(i);
            }
            avg_nugget = avg_nugget/sum_weights; //(directions.size());
        }
    }

    // 5) Ricalcolo il fitting a modello e nugget (medio sulle direzioni per il modello prescelto) fissato
    vector<variogram> vv = fit_dir_variogram (dev, directions, range_precision, types.at(min_index), avg_nugget);

    if(is_print == true)
    {
        int dir = 0;
        for(const variogram &v:vv)
        {
            cout<<endl;
            cout<<"################################################"<<endl;
            cout<<"###### FITTED DIRECTIONAL VARIOGRAM MODEL ######"<<endl;
            cout<<"###### Direction "<< dir << ": "<<directions.at(dir)<<" +/- "<<degree_tolerance<<" ################"<<endl;
            cout<<"################################################"<<endl;
            cout<<"Model Type: "<<v.type<<endl;
            cout<<"Sill: "<<v.sill<<endl;
            cout<<"Nugget: "<<v.nugget<<endl;
            cout<<"Range: "<<v.range<<endl;
            cout<<"################################################"<<endl;
            cout<<"################################################"<<endl;
            cout<<endl;

            dir++;
        }
    }

    return vv; //variogramma sperimentale per ogni direzione

}


/// @brief Fit directional variogram with ranging on models and fixed nugget
/// @param dev vector of directional experimental variograms
/// @param directions vector of directions (in degree) for the directional variogram
/// @param degree_tolerance degree tolerance for the directional variogram (e.g., +/- 22.5 for 8 directions)
/// @param range_precision range precision for the variogram fitting (e.g., 100 for 100 steps between min and max range)
/// @param nugget nugget value for the variogram fitting (e.g., 0.2)
/// @param is_print boolean flag to print the fitted variogram parameters for each direction
/// @return vector of fitted variogram models for each direction
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget, bool is_print)
{
    // 0) Definisco il vettore dei modelli
    std::vector<variogram_type> types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL};
    std::vector<std::vector<MUSE::VarioError>> dir_vario (types.size());

    // 1) Ciclo sui modelli e calcolo il fit (a modello fissato) dei variogrammi direzionali
    for(size_t i=0; i<types.size(); i++)
        dir_vario.at(i) = fit_dir_variogram_1par (dev, directions, range_precision, nugget, types.at(i));


    // 2) Inizializza l'errore somma per ogni modello
    std::vector<double> sum_mse (types.size(), 0.0);
    for(size_t i=0; i<types.size(); i++)
    {
        for(size_t j=0; j<dir_vario.at(i).size(); j++)
            sum_mse.at(i) += dir_vario.at(i).at(j).mse; //sommo l'errore su tutte le direzioni per ogni tipo
    }

    // 3) Trova minimo errore e l'indice corrispondente
    double min_mse = DBL_MAX;
    int min_index = INT_MAX;
    for(size_t i=0; i< sum_mse.size(); i++)
    {
        if(sum_mse.at(i) < min_mse)
        {
            min_mse = sum_mse.at(i);
            min_index = i;
        }
    }

    std::string str_model;
    convert_to_str(str_model, types.at(min_index));
    std::cout << "Fit directional variogram model with minimum MSE on all directions: "<< str_model << "; Mean Squared Error: "<< sum_mse.at(min_index) << std::endl;


//    // Media del nugget per il modello ad errore minimo (min_index)
//    double avg_nugget = 0.0;
//    for(size_t i=0; i<dir_vario.at(min_index).size(); i++)
//    {
//        std::cout << FMAG("NUGGET = ") << dir_vario.at(min_index).at(i).vario.nugget << FMAG("; PESO = ") << weigth.at(i) << std::endl;
//        avg_nugget += dir_vario.at(min_index).at(i).vario.nugget * weigth.at(i);

//    }

//    avg_nugget = avg_nugget/(directions.size());


    // 5) Ricalcolo il fitting a modello e nugget (medio sulle direzioni per il modello prescelto) fissato
    //vector<variogram> vv = fit_dir_variogram (dev, directions, range_precision, types.at(min_index), avg_nugget);

    if(types.at(min_index) == variogram_type::GAUSSIAN)
    {
        std::cout << "### Variogram model is Gaussian. Check on nugget value ..." << std::endl;
        if(nugget > 0.05 * 1.0) //sill=1.0
        {
            std::cout << "WARNING: Nugget is major than 5% of total sill" << std::endl;
            std::cout << "Model: " << str_model << " | Nugget: " << nugget << " | Sill: " << 1.0 << " | 5% sill: " << 0.05*1.0 << std::endl;

            double min_mse_tmp = DBL_MAX;
            int min_index_tmp = INT_MAX;
            for(size_t i=0; i< sum_mse.size(); i++)
            {
                if((sum_mse.at(i) < min_mse_tmp) && (i != min_index))
                {
                    min_mse_tmp = sum_mse.at(i);
                    min_index_tmp = i;
                }
            }
            min_index = min_index_tmp;
            min_mse = min_mse_tmp;
        }
    }

    vector<variogram> vv;
    for(uint dir=0; dir< directions.size(); dir++)
        vv.push_back(dir_vario.at(min_index).at(dir).vario);

    if(is_print == true)
    {
        int dir = 0;
        for(const variogram &v:vv)
        {
            cout<<endl;
            cout<<"################################################"<<endl;
            cout<<"###### FITTED DIRECTIONAL VARIOGRAM MODEL ######"<<endl;
            cout<<"###### Direction "<< dir << ": "<<directions.at(dir)<<" +/- "<<degree_tolerance<<" ################"<<endl;
            cout<<"################################################"<<endl;
            cout<<"Model Type: "<<v.type<<endl;
            cout<<"Sill: "<<v.sill<<endl;
            cout<<"Nugget: "<<v.nugget<<endl;
            cout<<"Range: "<<v.range<<endl;
            cout<<"################################################"<<endl;
            cout<<"################################################"<<endl;
            cout<<endl;

            dir++;
        }
    }

    return vv; //variogramma sperimentale per ogni direzione

}




///
/// \brief fit_dir_variogram computes directional variogram fitting, with model type fixed
/// \param dev is the vector of directional experimental variogram
/// \param directions is the vector of the directions
/// \param degree_tolerance is the degree tolerance
/// \param range_precision
/// \param nugget_precision
/// \param type is the model type
/// \param weigth is the vector of weigths related to the number of zeros, near to the origin axis (limit is set on 4*firstlagspac)
/// \param is_print
/// \return fitted variogram vector
///
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, variogram_type &type, const std::vector<double> &weigth, bool is_print)
{
    // 0) Calcolo fit a modello fissato dei variogrammi direzionali
    std::vector<MUSE::VarioError> dir_vario = fit_dir_variogram (dev, directions, range_precision, nugget_precision, type);

    // 1) Inizializza l'errore somma per ogni modello
    double sum_mse = 0.0;
    for(size_t j=0; j<dir_vario.size(); j++)
        sum_mse += dir_vario.at(j).mse; //sommo l'errore su tutte le direzioni per ogni tipo

    std::cout << "Fit directional variogram model with minimum MSE on all directions: "<< type << "; Mean Squared Error: "<< sum_mse << std::endl;



    // 2) Media del nugget per il modello ad errore minimo (min_index)
    double avg_nugget = 0.0;
    for(size_t i=0; i<dir_vario.size(); i++)
    {
        std::cout << FMAG("### Nugget: ") << dir_vario.at(i).vario.nugget << FMAG("; assigned weight: ") << weigth.at(i) << std::endl;
        avg_nugget += dir_vario.at(i).vario.nugget * weigth.at(i);
    }
    avg_nugget = avg_nugget/(directions.size());




    // 5) Ricalcolo il fitting a modello e nugget (medio sulle direzioni per il modello prescelto) fissato
    vector<variogram> vv = fit_dir_variogram (dev, directions, range_precision, type, avg_nugget);

    if(is_print == true)
    {
        int dir = 0;
        for(const variogram &v:vv)
        {
            cout<<endl;
            cout<<"################################################"<<endl;
            cout<<"###### FITTED DIRECTIONAL VARIOGRAM MODEL ######"<<endl;
            cout<<"###### Direction "<< dir << ": "<<directions.at(dir)<<" +/- "<<degree_tolerance<<" ################"<<endl;
            cout<<"################################################"<<endl;
            cout<<"Model Type: "<<v.type<<endl;
            cout<<"Sill: "<<v.sill<<endl;
            cout<<"Nugget: "<<v.nugget<<endl;
            cout<<"Range: "<<v.range<<endl;
            cout<<"################################################"<<endl;
            cout<<"################################################"<<endl;
            cout<<endl;

            dir++;
        }
    }

    return vv; //variogramma sperimentale per ogni direzione

}


/// @brief fitting variogram for each direction with minimum error varying on range, nugget and model type - with selection of best model based on MSE on all directions and weighted average of nugget values for best model - with discrete directions
/// @param dev 
/// @param directions 
/// @param degree_tolerance 
/// @param range_precision 
/// @param nugget_precision 
/// @param type 
/// @param weight 
/// @param is_print 
/// @return 
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::string &type, const std::vector<double> &weight, bool is_print)
{
    if(type.compare("AUTO") != 0)
    {
        std::cout << "Fitting of directional variogram is set using fixed model type: " << type << std::endl;
        variogram_type model_type;
        convert_from_str(type, model_type);

        return fit_dir_variogram (dev, directions, degree_tolerance, range_precision, nugget_precision, model_type, weight, is_print);
    }
    else
    {
        std::cout << "Fitting of directional variograms is set on: " << type << std::endl;
        return fit_dir_variogram (dev, directions, degree_tolerance, range_precision, nugget_precision, weight, is_print);
    }
}


///
/// \brief fit_anisotropy_ellipse fits the anisotropy ellipse parameters (center, angle, width, height) given the coordinates of the points on the ellipse
/// \param x is the vector of x coordinates of the points on the ellipse
/// \param y is the vector of y coordinates of the points on the ellipse
/// \param ellipse_par is the structure that contains the parameters of the ellipse (center_x, center_y, phi_rad, max_dir, min_dir, max_semiaxis, min_semiaxis)
///
void fit_anisotropy_ellipse (const std::vector<double> &x, const std::vector<double> &y, MUSE::EllipseParameter &ellipse_par)
{
    size_t n = x.size();
    std::vector<std::vector<double>> A;
    A.resize(n, std::vector<double>(2, 0));

    for(size_t j =0; j<n; j++)
    {
        A[j][0] = x.at(j);
        A[j][1] = y.at(j);
    }

    //double center_x, center_y, phi, width, height;

    ellipse_fit my_ellipse;
    my_ellipse.set(A);
    my_ellipse.fit(ellipse_par.center_x, ellipse_par.center_y, ellipse_par.phi_rad, ellipse_par.max_semiaxis, ellipse_par.min_semiaxis);

    std::cout << std::endl;
    std::cout << "###########################################" << std::endl;
    std::cout << "###########################################" << std::endl;
    std::cout << "#### ANISOTROPY ELLIPSE PARAMETERS (*) #### "<< std::endl;
    cout<<"Center x: "<<ellipse_par.center_x<<endl;
    cout<<"Center y: "<<ellipse_par.center_y<<endl;

    cout<<"phi (rad): "<< ellipse_par.phi_rad <<endl;
    cout<<"width: "<< ellipse_par.max_semiaxis<<endl;
    cout<<"height: "<< ellipse_par.min_semiaxis<<endl;
    std::cout << "###########################################" << std::endl;
    std::cout << "###########################################" << std::endl;
    std::cout << std::endl;

    std::cout << "(*) phi is the angle (in radians) of counter-clockwise rotation of major semi-axis of ellipse to x-axis." << std::endl;
    std::cout << "(*) width is the major semi-axis of ellipse." << std::endl;
    std::cout << "(*) height is minor semi-axis of ellipse." << std::endl;
    std::cout << FGRN("Fit anisotropy ellipse ... COMPLETED.") << std::endl;
    std::cout << std::endl;
}






/*// Da utilizzare nel caso servisse salvare ed esportare l'errore
MUSE::VarioError fit_variogram_mse(const exp_variog &ev, const double &range_precision, const variogram_type &model_type)
{
    MUSE::VarioError res;   // questa è la struttura che viene restituita da questa funzione
                     // è composta da 3 double: sill,range,nugget
                     //               1 string: modello

    double nugget = ev.gamma.at(0);
    double sill = 1 - nugget;     //oppure c=1 e quindi sill=1-nugget (da rivedere)
    double c=sill;
    double c0=nugget;

    double max_range=ev.h.at(ev.h.size()-1);
    double min_range=ev.h.at(1);
    double best_mse = DBL_MAX;
    double best_r = 0;

    vector<double> out(ev.gamma.size()-1);
    for(unsigned int i=0; i<out.size(); i++)
        out.at(i)=1;    // c corrisponde all'asintoto del variogramma, quindi inizializzo out con quello e poi eventualmente per i valori di h minori di range lo cambio

    //double mse = 0;
    res.mse = 0;

    switch (model_type)
    {
    case variogram_type::SPHERIC:
    {
        for(double r = min_range; r<max_range; r = r + max_range/range_precision)
        {
           for(unsigned int i=1; i<ev.gamma.size(); i++)
           {
               if(ev.h.at(i)<=r && ev.h.at(i)>0)
                   out.at(i-1) = c * ( (3 * ev.h.at(i)) / (2 * r) - 0.5 * pow(ev.h.at(i) / r , 3) ) + c0;     //out[h <= range] <- c * ((3 * h[h <= range]) / (2 * range) - 1 / 2 * (h[h <= range] / range)^3)
               else
                   out.at(i-1) =  c + c0;
           }
           double last_mse_j = res.mse;
           res.mse = 0;
           int count = 0;

           int countGT1 = 0;

           for(unsigned int i=0; i<out.size(); i++)
           {
               if(ev.gamma.at(i+1)<=1 && countGT1<3)
               {
                   count++;
                   res.mse += pow((out.at(i) - ev.gamma.at(i+1)),2) * (ev.N.at(i+1) / pow((ev.h.at(i+1)),1));
                   //cout<<" j "<<j<< " mse: "<<mse[j]<<" out: "<<out[i]<<" gamma: "<<ev.gamma[i+1]<<" n: "<<ev.N[i+1]<<" h: "<<ev.h[i+1]<<endl;
               }
               else
                   countGT1++;
           }

           //mse.at(j)=mse.at(j)/count;

           res.mse = res.mse;

           if(best_mse > res.mse)
           {
               best_mse=res.mse;
               //cout<<"SFERICO...BEST MSE for "<<j<<"..."<<best_mse<<"...con range = "<<r<<endl;
               best_r = r;
           }
           else
               res.mse = last_mse_j;
        }

        res.vario.type = "Sph";
        break;
    }
    case variogram_type::GAUSSIAN:
    {
        for(double r = min_range; r<max_range; r = r + max_range/range_precision)
        {
           for(unsigned int i=1; i<ev.gamma.size(); i++)
           {
               if(ev.h.at(i)<=r && ev.h.at(i)>0)
                   out.at(i-1) = c * ( 1 - exp(-1*(pow(ev.h.at(i),2)/pow(r,2)))) + c0;
               else
                   out.at(i-1) =  c +c0 ;
           }
           double last_mse_j = res.mse;
           res.mse = 0;
           int count = 0;

           int countGT1 = 0;

           for(unsigned int i=0; i<out.size(); i++)
           {
               // PESIAMO rispetto alla distanza il mse in modo che sia più importante fittare meglio
               // la funzione per coppie di punti più vicine rispetto a quelle più lontane
               // inoltre il peso dipende anche dal numero di coppie di punti che hanno contribuito al calcolo di gamma

               // https://stats.stackexchange.com/questions/99944/weights-in-the-fit-variogram-method-of-gstat
               // dove PEBESMA dice:
               // "The intuition behind it is that estimates with more point pairs (= more data) get more weight,
               // and that estimates at smaller lags get more weight (focus on behavior near origin)."
               // ed anche:
               // "The default method uses weights Nh/h2 with Nh the number of point pairs and h the distance.
               // This criterion is not supported by theory, but by practice. "

               if(ev.gamma.at(i+1)<=1 && countGT1<3)
               {
                   count++;
                   res.mse += pow((out.at(i) - ev.gamma.at(i+1)),2) * (ev.N.at(i+1) / pow((ev.h.at(i+1)),1));
                   //cout<<" j "<<j<< " mse: "<<mse[j]<<" out: "<<out[i]<<" gamma: "<<ev.gamma[i+1]<<" n: "<<ev.N[i+1]<<" h: "<<ev.h[i+1]<<endl;
               }
               else
                   countGT1++;
           }

           res.mse=res.mse;


           if(best_mse > res.mse)
           {
               best_mse=res.mse;
               best_r = r;
           }
           else
               res.mse = last_mse_j;
        }

        res.vario.type = "Gau";
        break;
    }
    case variogram_type::EXPONENTIAL:
    {
        for(double r = min_range; r<max_range; r = r + max_range/range_precision)
        {
            for(unsigned int i=1; i<ev.gamma.size(); i++)
           {
               if(ev.h.at(i)<=r && ev.h.at(i)>0)
                   out.at(i-1) =  c * ( 1 - exp(-(ev.h.at(i)/r))) + c0;
               else
                   out.at(i-1) =  c+c0;
           }
           double last_mse_j = res.mse;
           res.mse = 0;
           int count = 0;

           int countGT1 = 0;

           for(unsigned int i=0; i<out.size(); i++)
           {
               // PESIAMO rispetto alla distanza il mse in modo che sia più importante fittare meglio
               // la funzione per coppie di punti più vicine rispetto a quelle più lontane
               // inoltre il peso dipende anche dal numero di coppie di punti che hanno contribuito al calcolo di gamma

               // https://stats.stackexchange.com/questions/99944/weights-in-the-fit-variogram-method-of-gstat
               // dove PEBESMA dice:
               // "The intuition behind it is that estimates with more point pairs (= more data) get more weight,
               // and that estimates at smaller lags get more weight (focus on behavior near origin)."
               // ed anche:
               // "The default method uses weights Nh/h2 with Nh the number of point pairs and h the distance.
               // This criterion is not supported by theory, but by practice. "

               if(ev.gamma.at(i+1)<=1 && countGT1<3)
               {
                   count++;
                   res.mse += pow((out.at(i) - ev.gamma.at(i+1)),2) * (ev.N.at(i+1) / pow((ev.h.at(i+1)),1));
                   //cout<<" j "<<j<< " mse: "<<mse[j]<<" out: "<<out[i]<<" gamma: "<<ev.gamma[i+1]<<" n: "<<ev.N[i+1]<<" h: "<<ev.h[i+1]<<endl;
               }
               else
                   countGT1++;
           }

           //mse.at(j)=mse.at(j)/count;
           //mse.at(j)=mse.at(j);

           //cout<<"prima dell'if..."<<best_mse<<"..."<<mse.at(j)<<"..."<<count<<endl;
           if(best_mse > res.mse)
           {
               best_mse=res.mse;
               //cout<<"ESPONENZIALE...BEST MSE for "<<j<<"..."<<best_mse<<"... con range = "<<r<<endl;
               best_r = r;
           }
           else
               res.mse = last_mse_j;
        }

        res.vario.type = "Exp";
        break;
    }
    case variogram_type::LINEAR:
    {
        // modello lineare
        break;
    }
    case variogram_type::DEFAULT:
    {
        // modello default
        break;
    }
    }

    res.vario.sill = sill;
    res.vario.range = best_r;
    res.vario.nugget = nugget;

    cout<<endl;
    cout<<"####################################"<<endl;
    cout<<"###### FITTED VARIOGRAM MODEL ######"<<endl;
    cout<<"####################################"<<endl;
    cout<<"Model Type: "<<res.vario.type<<endl;
    cout<<"Sill: "<<res.vario.sill<<endl;
    cout<<"Nugget: "<<res.vario.nugget<<endl;
    cout<<"Range: "<<res.vario.range<<endl;
    cout<<"Mean Squared Error: "<<res.mse<<endl;
    cout<<"####################################"<<endl;
    cout<<"####################################"<<endl;
    cout<<endl;

    return res;
}*/

/*vector<MUSE::VarioError> fit_directional_variogram_mse(const dir_exp_variog &dev, const double & degree_step, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, variogram_type &model_type)
{
    vector<MUSE::VarioError> res_k (dev.N.size());

    // per ogni variogramma sperimentale direzionale bisogna fittare una funzione

    uint n_directions = dev.N.size();

    uint is_n_direction=0;    // parametro da modificare nel caso si voglia usare n_directions (=1) oppure degree_step e degree_tolerance (=0)
    vector<double> seq;

    if(is_n_direction==1)
    {
        seq.resize(n_directions+1);
        seq[0]=0;
        seq[n_directions]=180;
         for (uint i=1;i<seq.size()-1;i++)
         {
             seq[i] = seq[i-1] + 180 / n_directions ;
         }
    }

    else
    {
        n_directions = 180/degree_step;
        seq.resize(n_directions+1);

        seq[0]=0;
        for(uint i=1; i<seq.size();i++)
        {
            seq[i]= seq[i-1]+degree_step;
        }
    }

    for(uint k = 0; k< n_directions; k++)
    {
        if(dev.N.at(k).at(0)== 9999)
        {
            cout<<"NO FITTING is available for direction..."<<k<<endl;

            res_k.at(k).vario.sill = 9999;
            res_k.at(k).vario.range = 9999;
            res_k.at(k).vario.nugget = 9999;
            res_k.at(k).vario.type = "none";
            //cout<<"Variogram of direction "<<k<<": SILL..."<<res_k.at(k).sill<< " RANGE..."<<res_k.at(k).range<<" NUGGET..."<<res_k.at(k).nugget<<" MODEL..."<<res_k.at(k).type<<endl;
        }

        else
        {
            exp_variog ev_k;
            ev_k.N = dev.N.at(k);
            ev_k.h = dev.h.at(k);
            ev_k.gamma = dev.gamma.at(k);

            cout<<endl;
            if(is_n_direction==1)
                cout<<"########### Direction: "<<seq[k]<<" - "<<seq[k+1]<<" ##########"<<endl;
            else
                cout<<"###### Direction: "<<seq[k]<<" +/- "<<degree_tolerance<<" ######"<<endl;

            for(uint l=0; l<ev_k.N.size();l++)
                cout<<" N : "<<ev_k.N.at(l)<<"  h : "<<ev_k.h.at(l)<<"  gam :  "<<ev_k.gamma.at(l)<<endl;

            //res_k.at(k) = fit_variogram_mse(ev_k, range_precision, model_type);
            res_k.at(k) = fit_variogram_mse_rev01 (ev_k, range_precision, nugget_precision, model_type);

            cout<<"Variogram of direction "<<k<<": SILL..."<<res_k.at(k).vario.sill<< "RANGE..."<<res_k.at(k).vario.range<<"NUGGET..."<<res_k.at(k).vario.nugget<<"MODEL..."<<res_k.at(k).vario.type<<endl;
            cout<<"####################################"<<endl;
            cout<<endl;
        }
    }

    return res_k;
}*/






















