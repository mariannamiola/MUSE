#include "fitindvario.h"

//INDICATOR FITTING - OMNIDIRECTIONAL
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
/// \brief fit_ind_variogram compute the automatic fitting of omnidirectional variogram for indicators
/// \param ev is the omnidirectional experimental variogram
/// \param range_step is the factor to step range interval (from range max to range min) for the loop
/// \param nugget_step is the factor to step nugget interval (from nugget max to nugget min) for the loop
/// \param variance is the variance of categorical variables
/// \return fitting of indicator variogram
///
variogram fit_ind_variogram (const exp_variog &ev, const double &range_step, const double &nugget_step, const double &variance)
{
    //The value that the semivariogram model attains at the range (the value on the y-axis) is called the sill. The partial sill is the sill minus the nugget.

    variogram fitvario; //risultato finale
    double best_mse_model = DBL_MAX;

    std::vector<variogram_type> model_types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL}; //vettore dei variogramma modello

    for(size_t m=0; m<model_types.size(); m++)
    {
        variogram fitvario_on_nug;
        double best_mse_nug = DBL_MAX;

        double min_nugget = 0.0;
        double max_nugget = variance; //per variabili raw/indicatori: varianza!!

        //ciclo sui valori di nugget
        for(double nugget = min_nugget; nugget < max_nugget; nugget = nugget + max_nugget/nugget_step)
        {
            //fit vario on range -> variogramma modello al variare dei range
            variogram fitvario_on_range;

            double c0 = nugget;
            double c = variance-c0;

            double max_range = ev.h.at(ev.h.size()-1);
            double min_range = ev.h.at(0);

            double best_mse_range = DBL_MAX;
            double best_r = 0;
            //std::string best_type;

            // Inizializzazione di out a variance (asintoto variogramma: max valore)
            vector<double> out (ev.gamma.size(), variance);

            double mse = 0.0;

            //ciclo sui range
            for(double r = min_range; r<max_range; r = r + max_range/range_step)
            {
                for(size_t i=0; i<ev.h.size(); i++)
                    out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_types.at(m));

                double last_mse_j = mse;
                mse = 0.0;
                int count = 0;

                int countGT1 = 0;

                for(unsigned int i=0; i<out.size(); i++)
                {
                    if(ev.gamma.at(i) <= variance && countGT1<3)
                    {
                        count++;
                        mse += pow((out.at(i) - ev.gamma.at(i)),2) * (ev.N.at(i) / pow((ev.h.at(i)),1));
                    }
                    else
                        countGT1++;
                }

                if(mse < best_mse_range)
                {
                    best_mse_range = mse;
                    best_r = r;    
                }
                else
                    mse = last_mse_j;
            }

            // Soluzione a minimo errore (variando sul range) per un valore di fissato di nugget e per un modello fissato
            convert_to_str(fitvario_on_range.type, model_types.at(m));
            fitvario_on_range.sill = c+c0;
            fitvario_on_range.range = best_r;
            fitvario_on_range.nugget = c0;

            if(best_mse_range < best_mse_nug)
            {
                best_mse_nug = best_mse_range;
                fitvario_on_nug = fitvario_on_range;
            }
        }

        // if(best_mse_nug < best_mse_model)
        // {
        //     best_mse_model = best_mse_nug;
        //     fitvario = fitvario_on_nug;
        // }

        if(best_mse_nug < best_mse_model)
        {
            if(fitvario_on_nug.type.compare("Gau") == 0)
            {
                std::cout << "### Variogram model is Gaussian. Check on nugget value ..." << std::endl;
                if(fitvario_on_nug.nugget > 0.05 * fitvario_on_nug.sill)
                {
                    std::cout << "WARNING: Nugget is major than 5% of total sill" << std::endl;
                    std::cout << "Model: " << fitvario_on_nug.type << " | Nugget: " << fitvario_on_nug.nugget << " | Sill: " << fitvario_on_nug.sill << " | 5% sill: " << 0.05*fitvario_on_nug.sill << std::endl;
                    continue;
                }
            }
            else
            {
                best_mse_model = best_mse_nug;
                fitvario = fitvario_on_nug;
            }
        }
    }


    cout<<endl;
    cout<<"####################################"<<endl;
    cout<<"###### FITTED VARIOGRAM MODEL ######"<<endl;
    cout<<"####################################"<<endl;
    cout<<"Model Type: "<<fitvario.type<<endl;
    cout<<"Sill: "<<fitvario.sill<<endl;
    cout<<"Partial Sill: "<<fitvario.sill - fitvario.nugget <<endl;
    cout<<"Nugget: "<<fitvario.nugget<<endl;
    cout<<"Range: "<<fitvario.range<<endl;
    cout<<"####################################"<<endl;
    cout<<"####################################"<<endl;
    cout<<endl;

    return fitvario;
}

variogram fit_ind_variogram_1par (const exp_variog &ev, const double &range_step, const double &nugget, const double &variance)
{
    //The value that the semivariogram model attains at the range (the value on the y-axis) is called the sill. The partial sill is the sill minus the nugget.

    variogram fitvario; //risultato finale
    double best_mse_model = DBL_MAX;

    std::vector<variogram_type> model_types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL}; //vettore dei variogramma modello

    for(size_t m=0; m<model_types.size(); m++)
    {
        //fit vario on range -> variogramma modello al variare dei range
        variogram fitvario_on_range;

        double c0 = nugget;
        double c = variance-c0;

        double max_range = ev.h.at(ev.h.size()-1);
        double min_range = ev.h.at(0);

        double best_mse_range = DBL_MAX;
        double best_r = 0;
        //std::string best_type;

        // Inizializzazione di out a variance (asintoto variogramma: max valore)
        vector<double> out (ev.gamma.size(), variance);

        double mse = 0.0;

        //ciclo sui range
        for(double r = min_range; r<max_range; r = r + max_range/range_step)
        {
            for(size_t i=0; i<ev.h.size(); i++)
                out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_types.at(m));

            double last_mse_j = mse;
            mse = 0.0;
            int count = 0;

            int countGT1 = 0;

            for(unsigned int i=0; i<out.size(); i++)
            {
                if(ev.gamma.at(i) <= variance && countGT1<3)
                {
                    count++;
                    mse += pow((out.at(i) - ev.gamma.at(i)),2) * (ev.N.at(i) / pow((ev.h.at(i)),1));
                }
                else
                    countGT1++;
            }

            if(mse < best_mse_range)
            {
                best_mse_range = mse;
                best_r = r;
            }
            else
                mse = last_mse_j;
        }

        // Soluzione a minimo errore (variando sul range) per un valore di fissato di nugget e per un modello fissato
        convert_to_str(fitvario_on_range.type, model_types.at(m));
        fitvario_on_range.sill = c+c0;
        fitvario_on_range.range = best_r;
        fitvario_on_range.nugget = c0;

        // if(best_mse_range < best_mse_model)
        // {
        //     best_mse_model = best_mse_range;
        //     fitvario = fitvario_on_range;
        // }

        if(best_mse_range < best_mse_model)
        {
            if(fitvario_on_range.type.compare("Gau") == 0)
            {
                std::cout << "### Variogram model is Gaussian. Check on nugget value ..." << std::endl;
                if(fitvario_on_range.nugget > 0.05 * fitvario_on_range.sill)
                {
                    std::cout << "WARNING: Nugget is major than 5% of total sill" << std::endl;
                    std::cout << "Model: " << fitvario_on_range.type << " | Nugget: " << fitvario_on_range.nugget << " | Sill: " << fitvario_on_range.sill << " | 5% sill: " << 0.05*fitvario_on_range.sill << std::endl;
                    continue;
                }
            }
            else
            {
                best_mse_model = best_mse_range;
                fitvario = fitvario_on_range;
            }
        }
    }


    cout<<endl;
    cout<<"####################################"<<endl;
    cout<<"###### FITTED VARIOGRAM MODEL ######"<<endl;
    cout<<"####################################"<<endl;
    cout<<"Model Type: "<<fitvario.type<<endl;
    cout<<"Sill: "<<fitvario.sill<<endl;
    cout<<"Partial Sill: "<<fitvario.sill - fitvario.nugget <<endl;
    cout<<"Nugget: "<<fitvario.nugget<<endl;
    cout<<"Range: "<<fitvario.range<<endl;
    cout<<"####################################"<<endl;
    cout<<"####################################"<<endl;
    cout<<endl;

    return fitvario;
}

variogram fit_ind_variogram_1par (const exp_variog &ev, const double &range_step, const double &nugget_step, const double &variance, variogram_type &model_type)
{
    variogram fitvario; //risultato finale
    double best_mse_nug = DBL_MAX;

    double min_nugget = 0.0;
    double max_nugget = variance; //per variabili raw/indicatori: varianza!!

    //ciclo sui valori di nugget
    for(double nugget = min_nugget; nugget < max_nugget; nugget = nugget + max_nugget/nugget_step)
    {
        variogram fitvario_on_range;

        double c0 = nugget;
        double c = variance-c0;

        double max_range = ev.h.at(ev.h.size()-1);
        double min_range = ev.h.at(0);

        double best_mse_range = DBL_MAX;
        double best_r = 0;
        std::string best_type;

        // Inizializzazione di out a 1 (asintoto variogramma: max valore)
        vector<double> out (ev.gamma.size(), variance);

        double mse = 0.0;

        //ciclo sui range
        for(double r = min_range; r<max_range; r = r + max_range/range_step)
        {
            for(size_t i=0; i<ev.h.size(); i++)
                out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

            double last_mse_j = mse;
            mse = 0.0;
            int count = 0;

            int countGT1 = 0;

            for(unsigned int i=0; i<out.size(); i++)
            {
                if(ev.gamma.at(i)<= variance && countGT1<3)
                {
                    count++;
                    mse += pow((out.at(i) - ev.gamma.at(i)),2) * (ev.N.at(i) / pow((ev.h.at(i)),1));
                }
                else
                    countGT1++;
            }

            //mse = mse;

            if(mse < best_mse_range)
            {
                best_mse_range = mse;
                best_r = r;
            }
            else
                mse = last_mse_j;
        }

        // Soluzione a minimo errore (variando sul range) per un valore di fissato di nugget e per un modello fissato
        convert_to_str(fitvario_on_range.type, model_type);
        fitvario_on_range.sill = c+c0;
        fitvario_on_range.range = best_r;
        fitvario_on_range.nugget = c0;

        if(best_mse_range < best_mse_nug)
        {
            best_mse_nug = best_mse_range;
            fitvario = fitvario_on_range;
        }
    }

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

variogram fit_ind_variogram_2par (const exp_variog &ev, const double &range_step, const double &nugget, const double &variance, variogram_type &model_type, bool is_print)
{
    variogram fitvario; //risultato finale

    double c0 = nugget;
    double c = variance-c0;

    double max_range = ev.h.at(ev.h.size()-1);
    double min_range = ev.h.at(0);

    double best_mse_range = DBL_MAX;
    double best_r = 0;
    std::string best_type;

    // Inizializzazione di out a 1 (asintoto variogramma: max valore)
    vector<double> out (ev.gamma.size(), variance);

    double mse = 0.0;

    //ciclo sui range
    for(double r = min_range; r<max_range; r = r + max_range/range_step)
    {
        for(size_t i=0; i<ev.h.size(); i++)
            out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

        double last_mse_j = mse;
        mse = 0;
        int count = 0;

        int countGT1 = 0;

        for(unsigned int i=0; i<out.size(); i++)
        {
            if(ev.gamma.at(i)<= variance && countGT1<3)
            {
                count++;
                mse += pow((out.at(i) - ev.gamma.at(i)),2) * (ev.N.at(i) / pow((ev.h.at(i)),1));
            }
            else
                countGT1++;
        }

        mse = mse;

        if(mse < best_mse_range)
        {
            best_mse_range = mse;
            best_r = r;
            convert_to_str(best_type, model_type);
        }
        else
            mse = last_mse_j;
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
        cout<<"Partial Sill: "<<fitvario.sill - fitvario.nugget<<endl;
        cout<<"Nugget: "<<fitvario.nugget<<endl;
        cout<<"Range: "<<fitvario.range<<endl;
        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return fitvario;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


MUSE::VarioError fit_ind_variogram_1par_mse (const exp_variog &ev, const double &range_step, const double &nugget_step, const double &variance, variogram_type &model_type, bool is_print)
{
    MUSE::VarioError fitvario; //risultato finale

    double min_nugget = 0;
    double max_nugget = variance;

    std::vector<MUSE::VarioError> fitvario_on_nug; //fit vario on nugget -> variogramma modello al variare del nugget
    fitvario_on_nug.clear();

    //ciclo sui valori di nugget
    for(double nugget = min_nugget; nugget < max_nugget; nugget = nugget + max_nugget/nugget_step)
    {
        MUSE::VarioError fitvario_on_range; //fit vario: f(r, fixed nugget)

        double c0 = nugget;
        double c = variance-c0;

        double max_range = ev.h.at(ev.h.size()-1);
        double min_range = ev.h.at(0);

        double best_mse_range = DBL_MAX;
        double best_r = 0;
        std::string best_type;

        // Inizializzazione di out a 1 (asintoto variogramma: max valore)
        vector<double> out (ev.gamma.size(), variance);

        double mse = 0.0;

        //ciclo sui range
        for(double r = min_range; r<max_range; r = r + max_range/range_step)
        {
            for(size_t i=0; i<ev.h.size(); i++)
                out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

            double last_mse_j = mse;
            mse = 0;
            int count = 0;

            int countGT1 = 0;

            for(unsigned int i=0; i<out.size(); i++)
            {
                if(ev.gamma.at(i)<=variance && countGT1<3)
                {
                    count++;
                    mse += pow((out.at(i) - ev.gamma.at(i)),2) * (ev.N.at(i) / pow((ev.h.at(i)),1));
                }
                else
                    countGT1++;
            }

            //mse = mse;

            if(mse < best_mse_range)
            {
                best_mse_range = mse;
                best_r = r;
                convert_to_str(best_type, model_type);
            }
            else
                mse = last_mse_j;
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
        cout<<"Partial Sill: "<< fitvario.vario.sill - fitvario.vario.nugget <<endl;
        cout<<"Nugget: "<< fitvario.vario.nugget<<endl;
        cout<<"Range: "<< fitvario.vario.range<<endl;
        cout<<"Mean Squared Error: "<<fitvario.mse<<endl;
        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return fitvario;
}


MUSE::VarioError fit_ind_variogram_2par_mse (const exp_variog &ev, const double &range_step, const double &nugget, const double &variance, variogram_type &model_type, bool is_print)
{
    MUSE::VarioError fitvario; //risultato finale

    double c0 = nugget;
    double c = variance-c0;

    double max_range = ev.h.at(ev.h.size()-1);
    double min_range = ev.h.at(0);

    double best_mse_range = DBL_MAX;
    double best_r = 0;
    std::string best_type;

    // Inizializzazione di out a 1 (asintoto variogramma: max valore)
    vector<double> out (ev.gamma.size(), variance);

    double mse = 0.0;

    //ciclo sui range
    for(double r = min_range; r<max_range; r = r + max_range/range_step)
    {
        for(size_t i=0; i<ev.h.size(); i++)
            out.at(i) = get_gamma (ev.h.at(i), r, c0, c, model_type);

        double last_mse_j = mse;
        mse = 0;
        int count = 0;

        int countGT1 = 0;

        for(unsigned int i=0; i<out.size(); i++)
        {
            if(ev.gamma.at(i)<= variance && countGT1<3)
            {
                count++;
                mse += pow((out.at(i) - ev.gamma.at(i)),2) * (ev.N.at(i) / pow((ev.h.at(i)),1));
            }
            else
                countGT1++;
        }

        //mse = mse;

        if(mse < best_mse_range)
        {
            best_mse_range = mse;
            best_r = r;
            convert_to_str(best_type, model_type);
        }
        else
            mse = last_mse_j;
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
        cout<<"Partial Sill: "<< fitvario.vario.sill - fitvario.vario.nugget <<endl;
        cout<<"Nugget: "<< fitvario.vario.nugget<<endl;
        cout<<"Range: "<< fitvario.vario.range<<endl;
        cout<<"Mean Squared Error: "<<fitvario.mse<<endl;
        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return fitvario;
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::





//INDICATOR FITTING - DIRECTIONAL
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

vector<variogram> fit_ind_dir_variogram (const std::vector<exp_variog> &dev,const std::vector<double> &directions, const double & degree_tolerance, const double &range_step, const double &nugget_step, const double &variance, const std::vector<double> &weigth, bool is_print)
{
    // 0) Definisco il vettore dei modelli
    std::vector<variogram_type> types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL};
    std::vector<std::vector<MUSE::VarioError>> dir_vario (types.size());

    // 1) Ciclo sui modelli e calcolo il fit (a modello fissato) dei variogrammi direzionali
    for(size_t i=0; i<types.size(); i++)
        dir_vario.at(i) = fit_ind_dir_variogram_1par_mse (dev, directions, range_step, nugget_step, variance, types.at(i));

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


    // 4) Media del nugget per il modello ad errore minimo (min_index)
    double avg_nugget = 0.0;
    for(size_t i=0; i<dir_vario.at(min_index).size(); i++)
    {
        std::cout << FMAG("### Nugget: ") << dir_vario.at(min_index).at(i).vario.nugget << FMAG("; assigned weight: ") << weigth.at(i) << std::endl;
        avg_nugget += dir_vario.at(min_index).at(i).vario.nugget * weigth.at(i);
    }
    avg_nugget = avg_nugget/(directions.size());


    if(types.at(min_index) == variogram_type::GAUSSIAN)
    {
        std::cout << "### Variogram model is Gaussian. Check on nugget value ..." << std::endl;
        if(avg_nugget > 0.05 * variance)
        {
            std::cout << "WARNING: Nugget is major than 5% of total sill" << std::endl;
            std::cout << "Model: " << str_model << " | Nugget: " << avg_nugget << " | Sill: " << variance << " | 5% sill: " << 0.05*variance << std::endl;

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
        avg_nugget = 0.0;
        for(size_t i=0; i<dir_vario.at(min_index).size(); i++)
        {
            std::cout << FMAG("### Nugget: ") << dir_vario.at(min_index).at(i).vario.nugget << FMAG("; assigned weight: ") << weigth.at(i) << std::endl;
            avg_nugget += dir_vario.at(min_index).at(i).vario.nugget * weigth.at(i);
        }
        avg_nugget = avg_nugget/(directions.size());
    }



    // 5) Ricalcolo il fitting a modello e nugget (medio sulle direzioni per il modello prescelto) fissato
    vector<variogram> vv = fit_ind_dir_variogram_2par (dev, directions, range_step, avg_nugget, variance, types.at(min_index));

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


vector<variogram> fit_ind_dir_variogram_1par (const std::vector<exp_variog> &dev, const std::vector<double> &directions, const double & degree_tolerance, const double &range_step, const double &nugget, const double &variance, bool is_print)
{
    // 0) Definisco il vettore dei modelli
    std::vector<variogram_type> types = {variogram_type::SPHERIC, variogram_type::GAUSSIAN, variogram_type::EXPONENTIAL};
    std::vector<std::vector<MUSE::VarioError>> dir_vario (types.size());

    // 1) Ciclo sui modelli e calcolo il fit (a modello fissato) dei variogrammi direzionali
    for(size_t i=0; i<types.size(); i++)
        dir_vario.at(i) = fit_ind_dir_variogram_2par_mse (dev, directions, range_step, nugget, variance, types.at(i));


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

    if(types.at(min_index) == variogram_type::GAUSSIAN)
    {
        std::cout << "### Variogram model is Gaussian. Check on nugget value ..." << std::endl;
        if(nugget > 0.05 * variance)
        {
            std::cout << "WARNING: Nugget is major than 5% of total sill" << std::endl;
            std::cout << "Model: " << str_model << " | Nugget: " << nugget << " | Sill: " << variance << " | 5% sill: " << 0.05*variance << std::endl;

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


vector<variogram> fit_ind_dir_variogram_1par (const std::vector<exp_variog> &dev, const std::vector<double> &directions, const double & degree_tolerance, const double &range_step, const double &nugget_step, const double &variance, variogram_type &model_type, const std::vector<double> &weigth, bool is_print)
{
    // 0) Calcolo fit a modello fissato dei variogrammi direzionali
    std::vector<MUSE::VarioError> dir_vario = fit_ind_dir_variogram_1par_mse (dev, directions, range_step, nugget_step, variance, model_type);

    // 1) Inizializza l'errore somma per ogni modello
    double sum_mse = 0.0;
    for(size_t j=0; j<dir_vario.size(); j++)
        sum_mse += dir_vario.at(j).mse; //sommo l'errore su tutte le direzioni per ogni tipo

    std::cout << "Fit directional variogram model with minimum MSE on all directions: "<< model_type << "; Mean Squared Error: "<< sum_mse << std::endl;



    // 2) Media del nugget per il modello ad errore minimo (min_index)
    double avg_nugget = 0.0;
    for(size_t i=0; i<dir_vario.size(); i++)
    {
        std::cout << FMAG("### Nugget: ") << dir_vario.at(i).vario.nugget << FMAG("; weight: ") << weigth.at(i) << std::endl;
        avg_nugget += dir_vario.at(i).vario.nugget * weigth.at(i);
    }
    avg_nugget = avg_nugget/(directions.size());


    // 5) Ricalcolo il fitting a modello e nugget (medio sulle direzioni per il modello prescelto) fissato
    vector<variogram> vv = fit_ind_dir_variogram_2par (dev, directions, range_step, avg_nugget, variance, model_type);

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



vector<variogram> fit_ind_dir_variogram_2par (const std::vector<exp_variog> &dev, const std::vector<double> &directions, const double &range_step, const double &nugget, const double &variance, variogram_type &model_type, bool is_print)
{
    vector<variogram> res_k (dev.size());

    for(uint k = 0; k< directions.size(); k++)
        res_k.at(k) = fit_ind_variogram_2par (dev.at(k), range_step, nugget, variance, model_type, is_print);

    return res_k;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


vector<MUSE::VarioError> fit_ind_dir_variogram_1par_mse (const std::vector<exp_variog> &dev, const std::vector<double> &directions, const double &range_step, const double &nugget_step, const double &variance, variogram_type &model_type)
{
    vector<MUSE::VarioError> res_k (dev.size()); //VarioError per ogni direzione

    // Ciclo sulle direzioni e faccio il fitting a modello fissato + mse in output
    for(size_t k = 0; k < directions.size(); k++)
        res_k.at(k) = fit_ind_variogram_1par_mse (dev.at(k), range_step, nugget_step, variance, model_type);

    return res_k;
}

vector<MUSE::VarioError> fit_ind_dir_variogram_2par_mse (const std::vector<exp_variog> &dev, const std::vector<double> &directions, const double &range_step, const double &nugget, const double &variance, variogram_type &model_type, bool is_print)
{
    vector<MUSE::VarioError> res_k (dev.size());

    for(uint k = 0; k< directions.size(); k++)
        res_k.at(k) = fit_ind_variogram_2par_mse (dev.at(k), range_step, nugget, variance, model_type, is_print);

    return res_k;
}
