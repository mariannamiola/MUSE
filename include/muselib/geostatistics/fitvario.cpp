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
            double gamma_tmp = std::max(gamma, THRESH);
            w = N / (gamma_tmp * gamma_tmp);
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
        // nugget == max_nugget è degenere (partial sill = 0) quindi viene escluso nel loop (nugget < max_nugget)
        for(double nugget = min_nugget; nugget < max_nugget; nugget = nugget + max_nugget/nugget_precision)
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
            for(double r = min_range; r<=max_range; r = r + max_range/range_precision)
            {
                for(size_t i=0; i<ev.h.size(); i++)
                    out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_types.at(m));

                double mse = 0.0;
                for(unsigned int i=0; i<out.size(); i++)
                {
                    if (ev.h.at(i) > 0 && ev.gamma.at(i) <= 1.0) // sill=1 per normal score
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
        int min_index = 0;
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
        for(double r = min_range; r<=max_range; r = r + max_range/range_precision)
        {
            for(size_t i=0; i<ev.h.size(); i++)
                out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

            double mse = 0.0;
            for(unsigned int i=0; i<out.size(); i++)
            {
                if (ev.h.at(i) > 0 && ev.gamma.at(i) <= 1.0) // sill=1 per normal score
                {
                    double weight_coefficient = compute_weight(ev, i, w_type);
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
    int min_index = 0;
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
        for(double r = min_range; r<=max_range; r = r + max_range/range_precision)
        {
            for(size_t i=0; i<ev.h.size(); i++)
                out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_types.at(m));

            double mse = 0.0;
            for(unsigned int i=0; i<out.size(); i++)
            {
                if (ev.h.at(i) > 0 && ev.gamma.at(i) <= 1.0) // sill=1 per normal score
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

        fitvario_on_model.push_back(fitvario_on_range);
    }


    int min_index_model = 0;
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
    for(double r = min_range; r<=max_range; r = r + max_range/range_precision)
    {
        for(size_t i=0; i<ev.h.size(); i++)
            out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

        double mse = 0.0;
        for(unsigned int i=0; i<out.size(); i++)
        {
            if (ev.h.at(i) > 0 && ev.gamma.at(i) <= 1.0) // sill=1 per normal score
            {
                double weight_coefficient = compute_weight(ev, i, w_type);
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
        for(double r = min_range; r<=max_range; r = r + max_range/range_precision)
        {
            for(size_t i=0; i<ev.h.size(); i++)
                out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

            double mse = 0.0;
            for(unsigned int i=0; i<out.size(); i++)
            {
                if (ev.h.at(i) > 0 && ev.gamma.at(i) <= 1.0) // sill=1 per normal score
                {
                    double weight_coefficient = compute_weight(ev, i, w_type);
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

    int min_index = 0;
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
    std::string best_type = "";

    // Inizializzazione di out a 1 (asintoto variogramma: max valore)
    vector<double> out (ev.gamma.size(), 1.0);

    //ciclo sui range
    for(double r = min_range; r<=max_range; r = r + max_range/range_precision)
    {
        for(size_t i=0; i<ev.h.size(); i++)
            out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

        double mse = 0.0;
        for(unsigned int i=0; i<out.size(); i++)
        {
            if (ev.h.at(i) > 0 && ev.gamma.at(i) <= 1.0) // sill=1 per normal score
            {
                double weight_coefficient = compute_weight(ev, i, w_type);
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
vector<MUSE::VarioError> fit_dir_variogram (const std::vector<exp_variog> &dev, const double &degree_step, const double &degree_tolerance, const double &range_precision, const double &nugget_precision, variogram_type &model_type, weightsType w_type)
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
        res_k.at(k) = fit_variogram_mse (dev.at(k), range_precision, nugget_precision, model_type, false, w_type);
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
vector<MUSE::VarioError> fit_dir_variogram (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, const double &nugget_precision, variogram_type &model_type, weightsType w_type)
{
    // Funzione che richiama il fit_variogram (a modello fissato) e cicla per ogni direzione: funzione di supporto sulle direzioni

    vector<MUSE::VarioError> res_k (dev.size()); //VarioError per ogni direzione

    int n_directions = seq.size();

    // Ciclo sulle direzioni e faccio il fitting a modello fissato + mse in output
    for(uint k = 0; k< n_directions; k++)
    {
        //TO DO: AGGIUNGERE CHECK SUL NUMERO DI COPPIE
        res_k.at(k) = fit_variogram_mse (dev.at(k), range_precision, nugget_precision, model_type, false, w_type);
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
vector<MUSE::VarioError> fit_dir_variogram_1par (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, const double &nugget, variogram_type &model_type, bool is_print, weightsType w_type)
{
    // Funzione che richiama il fit_variogram (a modello fissato) e cicla per ogni direzione: funzione di supporto sulle direzioni

    vector<MUSE::VarioError> res_k (dev.size()); //VarioError per ogni direzione

    int n_directions = seq.size();

    // Ciclo sulle direzioni e faccio il fitting a modello fissato + mse in output
    for(uint k = 0; k< n_directions; k++)
    {
        //TO DO: AGGIUNGERE CHECK SUL NUMERO DI COPPIE
        res_k.at(k) = fit_variogram_mse_1par (dev.at(k), range_precision, nugget, model_type, is_print, w_type);
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
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev, const double & degree_step, const double & degree_tolerance, const double &range_precision, variogram_type &model_type, double &nugget, weightsType w_type)
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
        res_k.at(k) = fit_variogram (dev.at(k), range_precision, model_type, nugget, false, w_type);
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
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev, const std::vector<double> &seq, const double &range_precision, variogram_type &model_type, const double &nugget, bool is_print, weightsType w_type)
{
    vector<variogram> res_k (dev.size());

    for(uint k = 0; k< seq.size(); k++)
        res_k.at(k) = fit_variogram (dev.at(k), range_precision, model_type, nugget, is_print, w_type);

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
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev,const double & degree_step, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::vector<double> &weigth, bool is_print, weightsType w_type)
{
    // 0) Definisco il vettore dei modelli
    std::vector<variogram_type> types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL};
    std::vector<std::vector<MUSE::VarioError>> dir_vario (types.size());

    // 1) Ciclo sui modelli e calcolo il fit (a modello fissato) dei variogrammi direzionali
    for(size_t i=0; i<types.size(); i++)
        dir_vario.at(i) = fit_dir_variogram (dev, degree_step, degree_tolerance, range_precision, nugget_precision, types.at(i), w_type);

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
    vector<variogram> vv = fit_dir_variogram (dev, degree_step, degree_tolerance, range_precision, types.at(min_index), avg_nugget, w_type);

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
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::vector<double> &weigth, bool is_print, weightsType w_type)
{
    // 0) Definisco il vettore dei modelli
    std::vector<variogram_type> types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL};
    std::vector<std::vector<MUSE::VarioError>> dir_vario (types.size());


    // 1) Ciclo sui modelli e calcolo il fit (a modello fissato) dei variogrammi direzionali
    for(size_t i=0; i<types.size(); i++)
        dir_vario.at(i) = fit_dir_variogram (dev, directions, range_precision, nugget_precision, types.at(i), w_type);


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
    vector<variogram> vv = fit_dir_variogram (dev, directions, range_precision, types.at(min_index), avg_nugget, false, w_type);

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
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget, bool is_print, weightsType w_type)
{
    // 0) Definisco il vettore dei modelli
    std::vector<variogram_type> types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL};
    std::vector<std::vector<MUSE::VarioError>> dir_vario (types.size());

    // 1) Ciclo sui modelli e calcolo il fit (a modello fissato) dei variogrammi direzionali
    for(size_t i=0; i<types.size(); i++)
        dir_vario.at(i) = fit_dir_variogram_1par (dev, directions, range_precision, nugget, types.at(i), false, w_type);


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
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev, const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, variogram_type &type, const std::vector<double> &weigth, bool is_print, weightsType w_type)
{
    // 0) Calcolo fit a modello fissato dei variogrammi direzionali
    std::vector<MUSE::VarioError> dir_vario = fit_dir_variogram (dev, directions, range_precision, nugget_precision, type, w_type);

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
    vector<variogram> vv = fit_dir_variogram (dev, directions, range_precision, type, avg_nugget, false, w_type);

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
vector<variogram> fit_dir_variogram (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_precision, const double &nugget_precision, const std::string &type, const std::vector<double> &weight, bool is_print, weightsType w_type)
{
    if(type.compare("AUTO") != 0)
    {
        std::cout << "Fitting of directional variogram is set using fixed model type: " << type << std::endl;
        variogram_type model_type;
        convert_from_str(type, model_type);

        return fit_dir_variogram (dev, directions, degree_tolerance, range_precision, nugget_precision, model_type, weight, is_print, w_type);
    }
    else
    {
        std::cout << "Fitting of directional variograms is set on: " << type << std::endl;
        return fit_dir_variogram (dev, directions, degree_tolerance, range_precision, nugget_precision, weight, is_print, w_type);
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


///
/// \brief variogram_fit_wmse computes the weighted mean squared error of a fitted variogram
/// model against an experimental variogram, using the same point-validity rule and the same
/// weights of the fitting loops. The score is normalized by the sum of the weights, so it is
/// comparable across experimental variograms with different lags and different pair counts
/// (used to rank candidate fits computed on different analysis planes).
///
double variogram_fit_wmse (const exp_variog &ev, const variogram &v, weightsType w_type)
{
    // Convert the stored model type string back to the enum used by get_gamma
    variogram_type model_type;
    convert_from_str(v.type, model_type);

    // get_gamma composes gamma = c*f(h) + c0: c is the partial sill
    double c0 = v.nugget;
    double c = v.sill - v.nugget;

    double sse = 0.0;
    double wsum = 0.0;
    for(size_t i=0; i<ev.h.size(); i++)
    {
        // Same validity rule of the fitting loops (sill = 1 for normal score values)
        if(ev.h.at(i) > 0 && ev.gamma.at(i) <= 1.0)
        {
            double out = get_gamma(ev.h.at(i), v.range, c0, c, model_type);
            double w = compute_weight(ev, i, w_type);

            sse += pow((out - ev.gamma.at(i)), 2) * w;
            wsum += w;
        }
    }

    // Weighted mean: insensitive to the absolute magnitude of the weights
    return (wsum > 0.0) ? sse/wsum : DBL_MAX;
}


///
/// \brief fit_anisotropy_ellipsoid fits a 3D anisotropy ellipsoid, centered at the origin, on the
/// 3D points obtained by projecting the fitted directional ranges along their direction unit vectors.
/// The ellipsoid quadric form is p^T A p = 1 with A symmetric positive definite: the 6 unique
/// coefficients of A are estimated by linear least squares, then the eigen-decomposition of A
/// provides the semi-axes (1/sqrt(eigenvalue)) and the principal directions (eigenvectors).
/// The rotation angles are extracted following the GSLIB-like setrot() convention, so that the
/// result is directly usable by the covariance/Variogram machinery (see MUSE::EllipsoidParameter).
/// \param x is the vector of x coordinates of the range points (world coordinates)
/// \param y is the vector of y coordinates of the range points (world coordinates)
/// \param z is the vector of z coordinates of the range points (world coordinates)
/// \param ellipsoid_par is the output structure with semi-axes, angles and principal directions
///
void fit_anisotropy_ellipsoid (const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z, MUSE::EllipsoidParameter &ellipsoid_par)
{
    size_t n = x.size();
    ellipsoid_par.is_valid = false;

    // At least 6 points are required to estimate the 6 unique coefficients of the quadric
    if(n < 6 || y.size() != n || z.size() != n)
    {
        std::cerr << FRED("ERROR: fit_anisotropy_ellipsoid requires at least 6 consistent (x,y,z) points.") << std::endl;
        return;
    }

    // 1) Build the linear system D*s = 1 where s = (A11, A22, A33, A12, A13, A23)
    Eigen::MatrixXd D (n, 6);
    Eigen::VectorXd ones = Eigen::VectorXd::Ones(n);
    for(size_t i=0; i<n; i++)
    {
        D(i,0) = x[i]*x[i];
        D(i,1) = y[i]*y[i];
        D(i,2) = z[i]*z[i];
        D(i,3) = 2.0*x[i]*y[i];
        D(i,4) = 2.0*x[i]*z[i];
        D(i,5) = 2.0*y[i]*z[i];
    }

    // 2) Solve in the least squares sense (the system is overdetermined and the input is noisy)
    Eigen::VectorXd s = D.colPivHouseholderQr().solve(ones);

    // 3) Recompose the symmetric quadric matrix A from the 6 estimated coefficients
    Eigen::Matrix3d A;
    A << s(0), s(3), s(4),
         s(3), s(1), s(5),
         s(4), s(5), s(2);

    // 4) Mean absolute residual of the quadric equation (p^T A p = 1) as fit quality measure
    Eigen::VectorXd res = D*s - ones;
    ellipsoid_par.fit_residual = res.cwiseAbs().mean();

    // 5) Eigen-decomposition of A: eigenvalues in ascending order, orthonormal eigenvectors
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eig (A);
    Eigen::Vector3d lambda = eig.eigenvalues();

    // The quadric represents an ellipsoid only if A is positive definite
    if(lambda(0) <= 0.0)
    {
        std::cerr << FRED("ERROR: fitted quadric is not an ellipsoid (matrix is not positive definite).") << std::endl;
        std::cerr << FRED("Check the spatial coverage of the directional ranges used for the fitting.") << std::endl;
        return;
    }

    // The smallest eigenvalue corresponds to the major (longest) semi-axis
    Eigen::Vector3d e_major = eig.eigenvectors().col(0);
    double s_major = 1.0/std::sqrt(lambda(0));

    // 6) Assign the two secondary axes: the most vertical eigenvector fills the z slot,
    // the other one fills the min slot (consistent with range_min/range_z of variogram_methods)
    Eigen::Vector3d e_a = eig.eigenvectors().col(1);
    Eigen::Vector3d e_b = eig.eigenvectors().col(2);
    double s_a = 1.0/std::sqrt(lambda(1));
    double s_b = 1.0/std::sqrt(lambda(2));

    Eigen::Vector3d e_min, e_z;
    double s_min = 0.0, s_z = 0.0;
    if(std::fabs(e_a(2)) >= std::fabs(e_b(2)))
    {
        e_z = e_a;   s_z = s_a;   //e_a is the most vertical secondary axis
        e_min = e_b; s_min = s_b; //e_b is the most horizontal secondary axis
    }
    else
    {
        e_z = e_b;   s_z = s_b;
        e_min = e_a; s_min = s_a;
    }

    // 7) Axes are bidirectional: flip the major axis to keep its azimuth in [0, 180)
    const double rad2deg = 180.0/M_PI;
    double az = std::atan2(e_major(0), e_major(1)) * rad2deg;
    if(az < 0.0)
    {
        e_major = -e_major;
        az += 180.0;
    }

    // Near-vertical major axis: the azimuth is undefined, fix it to 0 by convention
    if(std::fabs(e_major(2)) > 0.999999)
        az = 0.0;

    // Roll (ang2 of setrot) is the inclination of the major axis above the horizontal plane
    double roll = std::asin(std::max(-1.0, std::min(1.0, e_major(2)))) * rad2deg;

    // 8) Build the right-handed frame (rows of the setrot matrix) to extract pitch (ang3):
    // the third row is the cross product of the major and secondary axes
    Eigen::Vector3d row2 = e_major.cross(e_min);
    double pitch = std::atan2(e_min(2), row2(2)) * rad2deg;

    // Keep pitch in (-90, 90]: flipping the secondary axis rotates pitch by 180 degree
    if(pitch > 90.0)
    {
        e_min = -e_min;
        pitch -= 180.0;
    }
    else if(pitch <= -90.0)
    {
        e_min = -e_min;
        pitch += 180.0;
    }

    // Recompute the third row after the possible secondary axis flip
    row2 = e_major.cross(e_min);

    // 9) Store all the estimated parameters in the output structure
    ellipsoid_par.max_semiaxis = s_major;
    ellipsoid_par.min_semiaxis = s_min;
    ellipsoid_par.z_semiaxis   = s_z;

    ellipsoid_par.azimuth = az;
    ellipsoid_par.roll    = roll;
    ellipsoid_par.pitch   = pitch;

    ellipsoid_par.max_axis_dir = {e_major(0), e_major(1), e_major(2)};
    ellipsoid_par.min_axis_dir = {e_min(0), e_min(1), e_min(2)};
    ellipsoid_par.z_axis_dir   = {row2(0), row2(1), row2(2)};
    ellipsoid_par.is_valid = true;

    // 10) Print a human readable summary of the fitted ellipsoid
    std::cout << std::endl;
    std::cout << "#############################################" << std::endl;
    std::cout << "#### ANISOTROPY ELLIPSOID PARAMETERS (*) ####" << std::endl;
    std::cout << "Max semi-axis (range_max): " << ellipsoid_par.max_semiaxis << std::endl;
    std::cout << "Min semi-axis (range_min): " << ellipsoid_par.min_semiaxis << std::endl;
    std::cout << "Z   semi-axis (range_z):   " << ellipsoid_par.z_semiaxis << std::endl;
    std::cout << "Azimuth (degree from North, clockwise): " << ellipsoid_par.azimuth << std::endl;
    std::cout << "Roll (degree, major axis dip):          " << ellipsoid_par.roll << std::endl;
    std::cout << "Pitch (degree, around major axis):      " << ellipsoid_par.pitch << std::endl;
    std::cout << "Mean quadric residual: " << ellipsoid_par.fit_residual << std::endl;
    std::cout << "#############################################" << std::endl;
    std::cout << "(*) Angles follow the setrot() convention of geostatslib (azimuth=ang1, roll=ang2, pitch=ang3)." << std::endl;
    std::cout << FGRN("Fit anisotropy ellipsoid ... COMPLETED.") << std::endl;
    std::cout << std::endl;
}















