#include "vario.h"


#include "muselib/colors.h"
#include "muselib/geometry/tools.h"


double get_degrees (const double &rad)
{
    const double coeff = M_PI / 180;

    return rad / coeff;
}

double get_radians (const double &deg)
{
    const double coeff = M_PI / 180;

    return deg * coeff;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

double get_rangex (const double &h, const double &azimuth_deg) //degree (direction of variogram) = rotation angle starting from y axis
{
//    const double coeff = M_PI / 180;
//    double rad = azimuth_deg * coeff;

    double rad = get_radians(azimuth_deg);

    if(rad >= M_PI)
    {
        rad = rad - M_PI;
        return -(h * sin(rad));
    }
    else
        return h * sin(rad);
}

double get_rangey (const double &h, const double &azimuth_deg) //degree (direction of variogram) = rotation angle starting from y axis
{
//    const double coeff = M_PI / 180;
//    double rad = azimuth_deg * coeff;

    double rad = get_radians(azimuth_deg);

    if(rad >= M_PI)
    {
        rad = rad - M_PI;
        return -(h * cos(rad));
    }
    else
        return h * cos(rad);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


normalscore normal_score2(const std::vector<double> &variable)
{

//    double media = mean(variable);
//    double devstd = stdev(variable);

//    std::vector<double> pp;

//    for(unsigned int l=0; l<variable.size(); l++)
//    {
//        double z = (variable.at(l)-media)/devstd;
//        std::cout << "INDICE = " << l  << "; VAR = " << variable.at(l) << "; VAL = " << z << std::endl;
//        pp.push_back(z);
//    }

    std::cout << FMAG("#################################") << std::endl;
    std::cout << FMAG("IMPLEMENTAZIONE NORMAL SCORE 2") << std::endl;
    std::cout << FMAG("Stessa impostazione della funzione normalscore originaria della geostalib") << std::endl;
    std::cout << FMAG("MODIFICA: sul vettore output.nsco e di conseguenza su output.values, per assegnare nsco uguali a valori uguali della variabile di input") << std::endl;
    std::cout << FMAG("OSSERVAZIONE: lavoro su vettore completo di dati, ordinato e ripulito da duplicati") << std::endl;
    std::cout << FMAG("#################################") << std::endl;
    std::cout << std::endl;

    normalscore output;

    std::vector<double> ord_variable_uniq = variable;

    sort(ord_variable_uniq.begin(),ord_variable_uniq.end());
    ord_variable_uniq.erase( unique(ord_variable_uniq.begin(), ord_variable_uniq.end()), ord_variable_uniq.end() );
    double n = ord_variable_uniq.size();
    std::cout << "numero valori unici: " << n << std::endl;

    std::vector<double> ord_variable = variable;
    sort(ord_variable.begin(), ord_variable.end());

    std::vector<double> n_ord_var_vec;
    for(size_t i=0; i<ord_variable_uniq.size(); i++)
    {
        int n_ord_var = count(ord_variable.begin(), ord_variable.end(), ord_variable_uniq.at(i));
        n_ord_var_vec.push_back(n_ord_var);
    }

    std::vector<double> zi;

    //https://en.wikipedia.org/wiki/Normal_probability_plot
    double a;
    if(ord_variable_uniq.size()<=10)
        a=3.0/8;
    else
        a=1.0/2;

    vector<double> arg;

    for(unsigned int l=1; l<= ord_variable_uniq.size(); l++)
    {
         arg.push_back( (l-a)/(ord_variable_uniq.size()+(1-2*a)));
    }

    for (unsigned int i=0; i<arg.size();i++)
    {
        zi.push_back(NormalCDFInverse(arg[i]));
    }



    std::vector<double> norm_variable;
    for(size_t i=0; i<n_ord_var_vec.size(); i++)
    {
        for(size_t j=0; j<n_ord_var_vec.at(i); j++)
            norm_variable.push_back(zi.at(i));
    }

    //VARIABILE ORDINATA
    output.x = ord_variable;

    //Z VARIABILE ORDINATA
    output.nsco = norm_variable;

    //Z VARIABILE ORIGINALE (NON ORDINATA)
    vector<int> ordine=order(variable);
    output.values = variable;

    int j=0;
    for(int i : ordine)
    {
        output.values[j]=output.nsco[i];
        j+=1;
    }

    return output;
}


normalscore normal_score3(const std::vector<double> &input)
{
    std::cout << FMAG("#################################") << std::endl;
    std::cout << FMAG("IMPLEMENTAZIONE NORMAL SCORE 3") << std::endl;
    std::cout << FMAG("Stessa impostazione della funzione normalscore originaria della geostalib") << std::endl;
    std::cout << FMAG("MODIFICA: sul vettore output.nsco e di conseguenza su output.values, per assegnare nsco uguali a valori uguali della variabile di input") << std::endl;
    std::cout << FMAG("OSSERVAZIONE: lavoro su vettore completo di dati, ordinato") << std::endl;
    std::cout << FMAG("#################################") << std::endl;
    std::cout << std::endl;

    normalscore output;

    // 1) Definizione x = variabile di input ordinata
    output.x = input;
    sort(output.x.begin(),output.x.end());

    // 2) Calcolo di pp -> //https://en.wikipedia.org/wiki/Normal_probability_plot
    //La parte successiva è quella che corrisponde a ppoints
    double a;
    if(input.size()<=10)
        a=3.0/8;
    else
        a=1.0/2;

    vector<double> pp;

    for(unsigned int l=1; l<=input.size(); l++)
    {
        pp.push_back( (l-a)/(input.size()+(1-a)-a));
        //std::cout << "indice = " << l <<  "; pp = " << (l-a)/(input.size()+(1-a)-a) << std::endl;
    }

    // 3) Calcolo qnorm -> definizione di nsco = score ordinati
    // La parte successiva è quella che corrisponde alla qnorm,
    // ossia alla CDF normale inversa che, passata una probabilità,
    // restituisce il valore assunto dalla distribuzione.
    for (unsigned int i=0; i<pp.size();i++)
    {
        output.nsco.push_back(NormalCDFInverse(pp[i]));
        //std::cout << "nsco = " << NormalCDFInverse(pp[i]) << std::endl;
    }

    //Modifica del vettore degli nscore per duplicati
    for (size_t i=output.x.size()-1; i>0; i--)
    {
        if(output.x.at(i-1) == output.x.at(i))
            output.nsco.at(i-1) = output.nsco.at(i);
    }


    // 4) Definizione values = nsco non ordinati
    std::vector<int> ordine = order(input);
    output.values = input;

    int j=0;
    for(int i : ordine)
    {
        output.values[j]=output.nsco[i];
        j+=1;
    }

    return output;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void load_exp_variogram (const std::string filename, exp_variog variogram)
{
    std::ifstream file_in;
    file_in.open(filename, std::fstream::out);
    if(!file_in.is_open())
    {
        std::cerr << "\033[0;31mError in file opening: " << filename << "\033[0m" << std::endl;
        exit(1);
    }

    uint N;
    double h, gamma;
    while(file_in >> N >> h >> gamma)
    {
        variogram.N.push_back(N);
        variogram.h.push_back(h);
        variogram.gamma.push_back(gamma);

    }
    file_in.close();
    std::cout << "Loading file: " << filename << " ... COMPLETED." << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/// @brief exp_variogram: OMNIDIRECTIONAL 1D EXPERIMENTAL VARIOGRAM - VARIABLE LAG SPACING
/// @param values (normal score) sampled values
/// @param coord_z z coordinates of the sampled points
/// @param n_points number of points of the experimental variogram
/// @param limit_dist limit distance for the experimental variogram
/// @param tol_factor factor for the lag tolerance definition
/// @return exp_variog struct containing the experimental variogram
exp_variog exp_variogram (const std::vector<double> &values, const vector<double> &coord_z,
                          const int &n_points,
                          const double &limit_dist,
                          const double &tol_factor)
{
    exp_variog ev;

    double max_dist = -DBL_MAX;
    double min_dist = DBL_MAX;
    double coverage = -DBL_MAX;

    // 1) Compute distances (max distances stored)
    if(limit_dist != -DBL_MAX) //se è diverso dal default (!= -DBL_MAX)
    {
        coverage = limit_dist;
        //std::cout << "Coverage is set on limit distance: " << coverage << std::endl;
    }
    else
    {
        for(unsigned int i = 0; i < values.size(); i++)
        {
            for(unsigned int j = i+1; j < values.size(); j++)
            {
                double md = sqrt((coord_z[i]-coord_z[j])*(coord_z[i]-coord_z[j]));

                if(md == 6956.2)
                    std::cout << coord_z[i] << "; " << coord_z[j] << std::endl;

                if(md > max_dist)
                    max_dist = md;
                if(md < min_dist)
                    min_dist = md;
            }
        }
        coverage = max_dist/2;
    }


    std::cout << "Max distance is set on: " << max_dist << std::endl;
    std::cout << "Min distance is set on: " << min_dist << std::endl;
    std::cout << "Coverage is set on: " << coverage << std::endl;


    // 2) Compute experimental variogram with variable lag (raggruppiamo i punti sul variogramma in modo da avere 15 punti (distanza;gamma))
    //n_points sono il numero dei punti del diagramma discretizzato che voglio ottenere -> devo aggiungere 1 per contare la tacca dell'origine
    int n_lags = (n_points+1)-1; //se parto da 0

    double lag_spacing = coverage/n_lags;
    std::cout << "Lag spacing is set on (max_dist/2)/n_lags: " << lag_spacing << std::endl;

    double tolerance = lag_spacing/tol_factor;
    std::cout << "\033[0;33mTol_factor is set on: " << tol_factor << "\033[0m" << std::endl;
    std::cout << "Lag tolerance is set on lag_spacing/tol_factor: " << tolerance << std::endl;


    // Tacche sull'asse x: riferimento al vettore seq della funzione del vario_exp
    ev.h.resize(n_points);

    ev.h[0] = lag_spacing; //primo valore di h = lag spacing

    //cout<<"Computed h(0): "<< ev.h[0] << std::endl;
    for(size_t i = 1; i < ev.h.size(); i++)
    {
        ev.h[i] = ev.h[i-1] + lag_spacing;
    }

    ev.N.resize(n_points, 0.0);
    ev.gamma.resize(n_points);


    // Compute discretized variogram
    std::vector<double> sum (ev.h.size(), 0.0); //vettore somma dei punti che ricadono all'interno della banda di tolleranza (check sulla distanza)

    for(unsigned int row = 0; row < values.size(); row++)
    {
        for(unsigned int col = row+1; col < values.size(); col++)
        {
            double md = sqrt((coord_z[row]-coord_z[col])*(coord_z[row]-coord_z[col]));

            //cout<<"Computed distance: "<< md << " Indici: row/col " << row << "/" << col << std::endl;
            for(size_t j = 0; j < ev.h.size(); j++)
            {
                //std::cout << "j = " << j << "; h(j) = " << ev.h.at(j) << std::endl;

                double min_tol = ev.h[j] - tolerance;
                double max_tol = ev.h[j] + tolerance;

                if(md >= min_tol && md<=max_tol)
                {
                    ev.N[j]+=1; //conto le coppie di punti che rientrano in quella banda di tolleranza

                    // Compute variogram
                    double mv = (values[row]-values[col]) * (values[row]-values[col])/2;

                    sum.at(j) += mv;

                    //cout<<"Check on distance: YES. Counter++ Nh = "<< ev.N.at(j) << endl;
                }
            }
        }
    }

    cout<<endl;
    cout<<"####################################"<<endl;
    cout<<"###### EXPERIMENTAL VARIOGRAM ######"<<endl;
    cout<<"####################################"<<endl;

    for(unsigned int j = 0; j < ev.h.size(); j++)
    {
        if (ev.N[j]>0)
            ev.gamma[j] = sum.at(j) / ev.N[j]; //media punti
        else
        {
            //std::cout << "N of points pair is = 0" << std::endl;
            ev.gamma[j] = 0.0;
        }
        cout<<ev.N[j] <<" ; "<<ev.h[j]<<" ; "<<ev.gamma[j]<<endl;
    }

    cout<<"####################################"<<endl;
    cout<<"####################################"<<endl;
    cout<<endl;

    return ev;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
/// \brief exp_variogram_constlag: OMNIDIRECTIONAL 3D EXPERIMENTAL VARIOGRAM - CONSTANT LAG SPACING
/// \param values
/// \param coord_x
/// \param coord_y
/// \param coord_z
/// \param n_points
/// \param limit_dist
/// \param tol_factor
/// \return
///
exp_variog exp_variogram_constlag (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const vector<double> &coord_z,
                                    const int &n_points,
                                    const double &limit_dist,
                                    const double &tol_factor)
{
    exp_variog ev;

    double max_dist = -DBL_MAX;
    double min_dist = DBL_MAX;
    double coverage = -DBL_MAX;

    for(unsigned int i = 0; i < values.size(); i++)
    {
        for(unsigned int j = i+1; j < values.size(); j++)
        {
            double md = sqrt((coord_x[i]-coord_x[j])*(coord_x[i]-coord_x[j])+
                                            (coord_y[i]-coord_y[j])*(coord_y[i]-coord_y[j]) +
                                            (coord_z[i]-coord_z[j])*(coord_z[i]-coord_z[j]));
            if(md > max_dist)
                max_dist = md;
            if(md < min_dist)
                min_dist = md;
        }
    }
    if(limit_dist != -DBL_MAX) //se è diverso dal default (!= -DBL_MAX)
        coverage = limit_dist;
    else
        coverage = max_dist/2;


    std::cout << "Max distance is set on: " << max_dist << std::endl;
    std::cout << "Min distance is set on: " << min_dist << std::endl;
    std::cout << "Coverage is set on: " << coverage << std::endl;


    // 2) Compute experimental variogram with variable lag (raggruppiamo i punti sul variogramma in modo da avere 15 punti (distanza;gamma))
    //n_points sono il numero dei punti del diagramma discretizzato che voglio ottenere -> devo aggiungere 1 per contare la tacca dell'origine
    int n_lags = (n_points+1)-1; //se parto da 0
    //int n_lags = (n_points)-1; //##################AGGIUNTO

    double lag_spacing = coverage/n_lags;
    std::cout << "Lag spacing is set on (max_dist/2)/n_lags: " << lag_spacing << std::endl;
    //double lag_spacing = (coverage-min_dist)/n_lags; //##################AGGIUNTO
    //std::cout << "Lag spacing is set on ((max_dist-min_dist)/2)/n_lags: " << lag_spacing << std::endl;

    double tolerance = lag_spacing/tol_factor;
    std::cout << "\033[0;33mTol_factor is set on: " << tol_factor << "\033[0m" << std::endl;
    std::cout << "Lag tolerance is set on lag_spacing/tol_factor: " << tolerance << std::endl;


    // Tacche sull'asse x: riferimento al vettore seq della funzione del vario_exp
    ev.h.resize(n_points);

    ev.h[0] = lag_spacing; //primo valore di h = lag spacing

    //cout<<"Computed h(0): "<< ev.h[0] << std::endl;
    for(size_t i = 1; i < ev.h.size(); i++)
    {
        ev.h[i] = ev.h[i-1] + lag_spacing;
    }

    ev.N.resize(n_points, 0.0);
    ev.gamma.resize(n_points);


    // Compute discretized variogram
    std::vector<double> sum (ev.h.size(), 0.0); //vettore somma dei punti che ricadono all'interno della banda di tolleranza (check sulla distanza)

    for(unsigned int row = 0; row < values.size(); row++)
    {
        for(unsigned int col = row+1; col < values.size(); col++)
        {
            double md = sqrt((coord_x[row]-coord_x[col])*(coord_x[row]-coord_x[col])+
                             (coord_y[row]-coord_y[col])*(coord_y[row]-coord_y[col]) +
                             (coord_z[row]-coord_z[col])*(coord_z[row]-coord_z[col]));

            //cout<<"Computed distance: "<< md << " Indici: row/col " << row << "/" << col << std::endl;
            for(size_t j = 0; j < ev.h.size(); j++)
            {
                //std::cout << "j = " << j << "; h(j) = " << ev.h.at(j) << std::endl;

                double min_tol = ev.h[j] - tolerance;
                double max_tol = ev.h[j] + tolerance;

                if(md >= min_tol && md<=max_tol)
                {
                    ev.N[j]+=1; //conto le coppie di punti che rientrano in quella banda di tolleranza

                    // Compute variogram
                    double mv = (values[row]-values[col]) * (values[row]-values[col])/2;

                    sum.at(j) += mv;

                    //cout<<"Check on distance: YES. Counter++ Nh = "<< ev.N.at(j) << endl;
                }
            }
        }
    }

    cout<<endl;
    cout<<"####################################"<<endl;
    cout<<"###### EXPERIMENTAL VARIOGRAM ######"<<endl;
    cout<<"####################################"<<endl;

    for(unsigned int j = 0; j < ev.h.size(); j++)
    {
        if (ev.N[j]>0)
            ev.gamma[j] = sum.at(j) / ev.N[j]; //media punti
        else
        {
            //std::cout << "N of points pair is = 0" << std::endl;
            ev.gamma[j] = 0.0;
        }
        cout<<ev.N[j] <<" ; "<<ev.h[j]<<" ; "<<ev.gamma[j]<<endl;
    }

    cout<<"####################################"<<endl;
    cout<<"####################################"<<endl;
    cout<<endl;

    return ev;
}

///
/// \brief exp_variogram_varlag: OMNIDIRECTIONAL 3D EXPERIMENTAL VARIOGRAM - VARIABLE LAG SPACING
/// \param values
/// \param coord_x
/// \param coord_y
/// \param coord_z
/// \param step
/// \param n_points
/// \param n_points_start
/// \param limit_dist
/// \param tol_factor
/// \param percent
/// \return
///
exp_variog exp_variogram_varlag (const vector<double> & values, const vector<double> & coord_x, const vector<double> & coord_y, const vector<double> & coord_z,
                                    const double &step,
                                    const int &n_points,
                                    const double &limit_dist,
                                    const double &tol_factor)
{
    exp_variog ev;

    std::cout << "\033[0;33mLimit distance is set on: " << limit_dist << "\033[0m" << std::endl;

    double max_dist = -DBL_MAX;
    double min_dist = DBL_MAX;
    double coverage = -DBL_MAX;


    // 1) Compute distances (max distances stored)
    for(unsigned int i = 0; i < values.size(); i++)
    {
        for(unsigned int j = i+1; j < values.size(); j++)
        {
            double md = sqrt((coord_x[i]-coord_x[j])*(coord_x[i]-coord_x[j])+
                                            (coord_y[i]-coord_y[j])*(coord_y[i]-coord_y[j]) +
                                            (coord_z[i]-coord_z[j])*(coord_z[i]-coord_z[j]));

            if(md > max_dist)
                max_dist = md;
            if(md < min_dist)
                min_dist = md;
        }
    }
    if(limit_dist != -DBL_MAX) //se è diverso dal default (!= -DBL_MAX)
        coverage = limit_dist;
    else
        coverage = max_dist/2;


    std::cout << "Max distance is set on: " << max_dist << std::endl;
    std::cout << "Min distance is set on: " << min_dist << std::endl;
    std::cout << "Coverage is set on: " << coverage << std::endl;


/*    // 2) Compute experimental variogram with costant lag (raggruppiamo i punti sul variogramma in modo da avere 15 punti (distanza;gamma))
    int n_lags = n_points-1;

    double lag_spacing = coverage/n_lags; // max_dist/2 = variogram coverage
    std::cout << "\033[0;33mWARNING: Lag spacing is set on (max_dist/2)/n_lags = " << lag_spacing << "\033[0m" << std::endl;

    // Defining tolerance as lag_spacing/2 (as default)
    double tolerance = lag_spacing/tol_factor;
    std::cout << "\033[0;33mWARNING: Lag tolerance is set on lag_spacing/tol_factor = " << tolerance << "\033[0m" << std::endl;

    vector<double> tol(n_points+1);
    tol[0] = tolerance/4;
    for(uint i=1;i<tol.size(); i++)
    {
        tol[i] = tol[i-1] * (1+percent/100 + i*0.0125);
        //cout<<"tol[ "<<i<<" ]"<<tol[i]<<endl;
    }


    // Tacche sull'asse x: riferimento al vettore seq della funzione del vario_exp
    ev.h.resize(n_points);

    ev.h[0] = tol[0] + tol[1]; //primo valore di h = lag spacing

    //cout<<"Computed h(0): "<< ev.h[0] << std::endl;
    for(size_t i = 1; i < ev.h.size(); i++)
    {
        ev.h[i] = ev.h[i-1] + tol[i] + tol[i+1];

//        if(ev.h[i]> coverage)
//            ev.h[i] = coverage;

        //cout<<"Computed h(j): "<< ev.h[i] << std::endl;
    }*/



    vector<double> tol;

    //Vecchio metodo per il calcolo delle h a lag variabile + tolerance
    //ev.h = lagvar1(coverage, n_points, percent, tol);

    //Nuovo metodo per il calcolo delle h a lag variabile + tolerance
    //ev.h = lagvar(coverage, min_dist, step, n_points, n_points_start, tol_factor, tol);
    ev.h = lagvar_new(coverage, min_dist, n_points, step, tol_factor, tol);


    ev.N.resize(ev.h.size(), 0.0);
    ev.gamma.resize(ev.h.size());


    // Compute discretized variogram
    std::vector<double> sum (ev.h.size(), 0.0); //vettore somma dei punti che ricadono all'interno della banda di tolleranza (check sulla distanza)

    for(unsigned int row = 0; row < values.size(); row++)
    {
        for(unsigned int col = row+1; col < values.size(); col++)
        {
            double md = sqrt((coord_x[row]-coord_x[col])*(coord_x[row]-coord_x[col])+
                             (coord_y[row]-coord_y[col])*(coord_y[row]-coord_y[col]) +
                             (coord_z[row]-coord_z[col])*(coord_z[row]-coord_z[col]));

            for(size_t j = 0; j < ev.h.size(); j++)
            {
                double min_tol = ev.h[j] - tol[j];
                double max_tol = 0.0;

                if(j == ev.h.size()-1) //se sono sull'ultimo lag
                    max_tol = ev.h[j] + tol[j];
                else
                    max_tol = ev.h[j] + tol[j+1];

                if(md >= min_tol && md<=max_tol)
                {
                    ev.N[j]+=1; //conto le coppie di punti che rientrano in quella banda di tolleranza

                    // Compute variogram
                    double mv = (values[row]-values[col]) * (values[row]-values[col])/2;
                    sum.at(j) += mv;
                }
            }
        }
    }

    cout<<endl;
    cout<<"####################################"<<endl;
    cout<<"###### EXPERIMENTAL VARIOGRAM ######"<<endl;
    cout<<"####################################"<<endl;

    for(unsigned int j = 0; j < ev.h.size(); j++)
    {
        if (ev.N[j]>0)
            ev.gamma[j] = sum.at(j) / ev.N[j]; //media punti
        else
        {
            //std::cout << "N of points pair is = 0" << std::endl;
            ev.gamma[j] = 0.0;
        }
        cout<<ev.N[j] <<" ; "<<ev.h[j]<<" ; "<<ev.gamma[j]<<endl;
    }

    cout<<"####################################"<<endl;
    cout<<"####################################"<<endl;
    cout<<endl;

    return ev;
}


/// \brief exp_variogram: OMNIDIRECTIONAL 3D EXPERIMENTAL VARIOGRAM - LAG SPACING DEFINED IN INPUT
/// \param values (normal) values of the sampled variable
/// \param coord_x x coordinates of the sampled points
/// \param coord_y y coordinates of the sampled points
/// \param coord_z z coordinates of the sampled points
/// \param lagspac_type type of lag spacing (VARIABLE or CONSTANT)
/// \param step step for the variable lag spacing (if lagspac_type = VARIABLE)
/// \param n_points number of points of the experimental variogram (for both variable and constant lag spacing)
/// \param limit_dist limit distance for the experimental variogram (if != -DBL_MAX)
/// \param tol_factor factor for the tolerance definition (tolerance = lag_spacing/tol_factor)
/// \return exp_variog struct containing the experimental variogram (h, gamma, N)
exp_variog exp_variogram (const vector<double> & values, const vector<double> & coord_x, const vector<double> & coord_y, const vector<double> & coord_z,
                            const std::string &lagspac_type,
                            const double &step,
                            const int &n_points,
                            const double &limit_dist,
                            const double &tol_factor)
{
    if(lagspac_type.compare("VARIABLE") == 0)
    {
        std::cout << "##### Experimental variogram with variable lag spacing ..." << std::endl;
        return exp_variogram_varlag(values, coord_x, coord_y, coord_z, step, n_points, limit_dist, tol_factor);
    }
    else if(lagspac_type.compare("CONSTANT") == 0)
    {
        std::cout << "##### Experimental variogram with constant lag spacing ..." << std::endl;
        return exp_variogram_constlag(values, coord_x, coord_y, coord_z, n_points, limit_dist, tol_factor);
    }
    else
        return exp_variog();
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



///
/// \brief dir_exp_variogram_lagconst: DIRECTIONAL 2D EXPERIMENTAL VARIOGRAM - CONSTANT LAG SPACING
/// \param values
/// \param coord_x
/// \param coord_y
/// \param seq
/// \param degree_tolerance
/// \param bandwidth
/// \param n_points
/// \param limit_dist
/// \param tol_factor
/// \return
///
std::vector<exp_variog> dir_exp_variogram_lagconst (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y,
                                                    const std::vector<double> &seq,
                                                    const double &degree_tolerance,
                                                    const double &bandwidth,
                                                    const int &n_points,
                                                    const double &limit_dist,
                                                    const double &tol_factor)
{
    int n_directions = seq.size();

    std::vector<exp_variog> dev;
    dev.resize(n_directions);

    //Ciclo sulle direzioni e calcolo il variogramma per ciascuna di esse
    for(size_t dir=0; dir<seq.size(); dir++)
    {
        double check_dir = seq[dir];

        std::cout << std::endl;
        std::cout << "### ID direction: " << dir << std::endl;
        std::cout << "Direction (degree): " << seq.at(dir) << std::endl;
        std::cout << "Degree tolerance (degree): " << degree_tolerance << std::endl;
        std::cout << "\033[0;33mBandwidth is set on: " << bandwidth << "\033[0m" << std::endl;
        std::cout << "\033[0;33mLimit distance is set on: " << limit_dist << "\033[0m" << std::endl;

        double max_dist = -DBL_MAX;
        double min_dist = DBL_MAX;
        double coverage = -DBL_MAX;

        // 1) Compute distances (max distances stored)
        for(unsigned int i = 0; i < values.size(); i++)
        {
            for(unsigned int j = i+1; j < values.size(); j++)
            {
//                    std::cout << "I_x" << coord_x[i] << std::endl;
//                    std::cout << "I_y" << coord_y[i] << std::endl;

//                    std::cout << "J_x" << coord_x[j] << std::endl;
//                    std::cout << "J_y" << coord_y[j] << std::endl;


                double md = sqrt((coord_x[i]-coord_x[j])*(coord_x[i]-coord_x[j])+
                                                (coord_y[i]-coord_y[j])*(coord_y[i]-coord_y[j]));
                //calcolo la direzione
                double md_x = coord_x[j]-coord_x[i];
                double md_y = coord_y[j]-coord_y[i];

                if((md_x ==0) && (md_y ==0))
                {
                    //std::cout << "undefined" << std::endl;
                    continue;
                }
                else
                {
                    double alfa_rad = atan2(md_y,md_x);
                    double dir_degree = 90 - get_degrees(alfa_rad);

                    if(dir_degree == 180)
                        dir_degree = 0;

                    if(check_dir == 180)
                        check_dir = 0;

                    //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
                    if(dir_degree >= check_dir-degree_tolerance && dir_degree <= check_dir+degree_tolerance)
                    {
                        double dretta = point_to_line_distance(0.0, 0.0, alfa_rad, md_x, md_y);
                        //std::cout << "distanza punto retta = " << dretta << std::endl;

                        if(dretta <= bandwidth)
                        {
                            //std::cout << "la distanza punto-retta è minore/uguale della bandwidth (definita in input)" << std::endl;
                            if(md > max_dist)
                                max_dist = md;
                            if(md < min_dist)
                                min_dist = md;
                        }
                    }
                }

            }
        }
        if(limit_dist != -DBL_MAX) //se è diverso dal default (!= -DBL_MAX)
            coverage = limit_dist;
        else
            coverage = max_dist/2;


        //else: utilizza come dist_max quella definita in input

        std::cout << "Max distance is set on: " << max_dist << std::endl;
        std::cout << "Min distance is set on: " << min_dist << std::endl;
        std::cout << "Coverage is set on: " << coverage << std::endl;


        // 2) Compute experimental variogram with variable lag (raggruppiamo i punti sul variogramma in modo da avere 15 punti (distanza;gamma))
        //n_points sono il numero dei punti del diagramma discretizzato che voglio ottenere -> devo aggiungere 1 per contare la tacca dell'origine
        int n_lags = (n_points+1)-1;

        double lag_spacing = coverage/n_lags;
        std::cout << "Lag spacing is set on (max_dist/2)/n_lags: " << lag_spacing << std::endl;

        double tolerance = lag_spacing/tol_factor;
        std::cout << "\033[0;33mTol_factor is set on: " << tol_factor << "\033[0m" << std::endl;
        std::cout << "Lag tolerance is set on lag_spacing/tol_factor: " << tolerance << std::endl;

        // Tacche sull'asse x: riferimento al vettore seq della funzione del vario_exp
        dev[dir].h.resize(n_points);

        dev[dir].h[0] = lag_spacing; //primo valore di h

        //cout<<"Computed h(0): "<< dev[dir].h[0] << std::endl;
        for(size_t i = 1; i < dev[dir].h.size(); i++)
        {
            dev[dir].h[i] = dev[dir].h[i-1] + lag_spacing;

//            if(dev[dir].h[i]> coverage)
//                dev[dir].h[i] = coverage;

            //cout<<"Computed h(j): "<< dev[dir].h[i] << std::endl;
        }

        //std::cout << "Computing constant lags ... COMPLETED." << std::endl;


        dev[dir].N.resize(n_points, 0.0);
        dev[dir].gamma.resize(n_points);



        // Compute discretized variogram
        std::vector<double> sum (dev[dir].h.size(), 0.0); //vettore somma dei punti che ricadono all'interno della banda di tolleranza (check sulla distanza)

        for(unsigned int row = 0; row < values.size(); row++)
        {
            for(unsigned int col = row+1; col < values.size(); col++)
            {
                double md = sqrt((coord_x[row]-coord_x[col])*(coord_x[row]-coord_x[col])+
                                 (coord_y[row]-coord_y[col])*(coord_y[row]-coord_y[col]));

                //calcolo la direzione
                double md_x = coord_x[col]-coord_x[row];
                double md_y = coord_y[col]-coord_y[row];

                if((md_x ==0) && (md_y ==0))
                {
                    //std::cout << "undefined" << std::endl;
                    continue;
                }
                else
                {
                    double alfa_rad = atan2(md_y,md_x);
                    double dir_degree = 90 - get_degrees(alfa_rad);

                    if(dir_degree == 180)
                        dir_degree = 0;

                    if(check_dir == 180)
                        check_dir = 0;

                    //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
                    if(dir_degree >= check_dir-degree_tolerance && dir_degree <= check_dir+degree_tolerance)
                    {
                        double dretta = point_to_line_distance(0.0, 0.0, alfa_rad, md_x, md_y);
                        //cout<<"Computed distance: "<< md << " Indici: row/col " << row << "/" << col << std::endl;

                        if(dretta <= bandwidth)
                        {
                            for(size_t j = 0; j < dev[dir].h.size(); j++)
                            {
                                double min_tol = dev[dir].h[j] - tolerance;
                                double max_tol = dev[dir].h[j] + tolerance;

                                if(md >= min_tol && md <= max_tol)
                                {
                                    dev[dir].N[j]+=1; //conto le coppie di punti che rientrano in quella banda di tolleranza

                                    // Compute variogram
                                    double mv = (values[row]-values[col]) * (values[row]-values[col])/2;
                                    sum.at(j) += mv;
                                }
                            }
                        }
                    }
                }
            }
        }

        cout<<endl;
        cout<<"################################################"<<endl;
        cout<<"###### DIRECTIONAL EXPERIMENTAL VARIOGRAM ######"<<endl;
        cout<<"########### Direction "<< dir << ": "<<seq[dir]<<" +/- "<<degree_tolerance<<" ############"<<endl;
        cout<<"################################################"<<endl;

        for(unsigned int j = 0; j < dev[dir].h.size(); j++)
        {
            if (dev[dir].N[j] > 0)
                dev[dir].gamma[j] = sum.at(j) / dev[dir].N[j]; //media punti
            else
                dev[dir].gamma[j]=0;

            cout<<dev[dir].N[j] <<" ; "<<dev[dir].h[j]<<" ; "<<dev[dir].gamma[j]<<endl;
        }

        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return dev;
}


///
/// \brief dir3DH_exp_variogram: HORIZONTAL DIRECTIONAL 3D EXPERIMENTAL VARIOGRAM
/// \param values
/// \param coord_x
/// \param coord_y
/// \param coord_z
/// \param lagspac_type: CONSTANT|VARIABLE
/// \param seq
/// \param hor_degree_tolerance
/// \param vert_degree_tolerance
/// \param hor_bandwidth
/// \param vert_bandwidth
/// \param step
/// \param n_points
/// \param limit_dist
/// \param tol_factor
/// \return
///
std::vector<exp_variog> dir3DH_exp_variogram (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const vector<double> &coord_z,
                                                    const std::string &lagspac_type,
                                                    const std::vector<double> &seq,
                                                    const double &hor_degree_tolerance,
                                                    const double &vert_degree_tolerance,
                                                    const double &hor_bandwidth,
                                                    const double &vert_bandwidth,
                                                    const double &step,
                                                    const int &n_points,
                                                    const double &limit_dist,
                                                    const double &tol_factor)
{
    int n_directions = seq.size();

    std::vector<exp_variog> dev;
    dev.resize(n_directions);

    double vert_seq = 0.0; //vertical direction for horizontal application

    //Ciclo sulle direzioni e calcolo il variogramma per ciascuna di esse
    for(size_t dir=0; dir<seq.size(); dir++)
    {
        double check_dir = seq[dir];

        std::cout << std::endl;
        std::cout << "### ID direction: " << dir << std::endl;
        std::cout << "Horizontal direction (degree): " << seq.at(dir) << std::endl;
        std::cout << "Vertical direction (degree): " << vert_seq << std::endl;
        std::cout << "Horizontal angle tolerance (degree): " << hor_degree_tolerance << std::endl;
        std::cout << "Vertical angle tolerance (degree): " << vert_degree_tolerance << std::endl;
        std::cout << FYEL("Horizontal bandwidth is set on: ") << hor_bandwidth << std::endl;
        std::cout << FYEL("Vertical bandwidth is set on: ") << vert_bandwidth << std::endl;
        std::cout << FYEL("Limit distance is set on: ") << limit_dist << std::endl;

        double max_dist = -DBL_MAX;
        double min_dist = DBL_MAX;
        double coverage = -DBL_MAX;

        // 1) Compute distances (max distances stored)
        for(unsigned int i = 0; i < values.size(); i++)
        {
            for(unsigned int j = i+1; j < values.size(); j++)
            {
                double md = sqrt((coord_x[i]-coord_x[j])*(coord_x[i]-coord_x[j])+
                                 (coord_y[i]-coord_y[j])*(coord_y[i]-coord_y[j])+
                                 (coord_z[i]-coord_z[j])*(coord_z[i]-coord_z[j]));

                //calcolo la direzione
                double md_x = coord_x[j]-coord_x[i];
                double md_y = coord_y[j]-coord_y[i];
                double md_z = coord_z[j]-coord_z[i];

                if((md_x ==0) && (md_y ==0))
                {
                    //std::cout << "undefined" << std::endl;
                    continue;
                }
                else
                {
                    double phi_rad = acos(md_z/md);
                    double theta_rad = atan2(md_y,md_x);
                    double dir_degree = 90 - get_degrees(theta_rad);
                    double vert_dir_degree = 90 - get_degrees(phi_rad);

                    //std::cout << "Computed direction in 3D space (degree) - HORIZONTAL: " << dir_degree << std::endl;
                    //std::cout << "Computed direction in 3D space (degree) - VERTICAL: " << vert_dir_degree << std::endl;

                    if(dir_degree == 180)
                        dir_degree = 0;

                    if(check_dir == 180)
                        check_dir = 0;

                    //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
                    //if(dir_degree >= seq[dir]-hor_degree_tolerance && dir_degree <= seq[dir]+hor_degree_tolerance)
                    if(dir_degree >= check_dir-hor_degree_tolerance && dir_degree <= check_dir+hor_degree_tolerance)
                    {
                        if(vert_dir_degree >= vert_seq-vert_degree_tolerance && vert_dir_degree <= vert_seq+vert_degree_tolerance)
                        {

                            double dretta = point_to_line_distance(0.0, 0.0, theta_rad, md_x, md_y);
                            //std::cout << "distanza punto retta = " << dretta << std::endl;

                            if(dretta <= hor_bandwidth)
                            {
                                //std::cout << "la distanza punto-retta è minore/uguale della bandwidth (definita in input)" << std::endl;
                                if(md > max_dist)
                                    max_dist = md;
                                if(md < min_dist)
                                    min_dist = md;
                            }
                        }
                    }
                }
            }
        }
        if(limit_dist != -DBL_MAX) //se è diverso dal default (!= -DBL_MAX)
            coverage = limit_dist;
        else
            coverage = max_dist/2;

        //}
        //else: utilizza come dist_max quella definita in input

        std::cout << "Max distance is set on: " << max_dist << std::endl;
        std::cout << "Min distance is set on: " << min_dist << std::endl;
        std::cout << "Coverage is set on: " << coverage << std::endl;

        vector<double> tol;
        double tolerance = 0.0;

        if(lagspac_type.compare("CONSTANT") == 0)
        {
            // 2) Compute experimental variogram with variable lag (raggruppiamo i punti sul variogramma in modo da avere 15 punti (distanza;gamma))
            //n_points sono il numero dei punti del diagramma discretizzato che voglio ottenere -> devo aggiungere 1 per contare la tacca dell'origine
            int n_lags = (n_points+1)-1;

            double lag_spacing = coverage/n_lags;
            std::cout << "Lag spacing is set on (max_dist/2)/n_lags: " << lag_spacing << std::endl;

            tolerance = lag_spacing/tol_factor;
            std::cout << "\033[0;33mTol_factor is set on: " << tol_factor << "\033[0m" << std::endl;
            std::cout << "Lag tolerance is set on lag_spacing/tol_factor: " << tolerance << std::endl;

            // Tacche sull'asse x: riferimento al vettore seq della funzione del vario_exp
            dev[dir].h.resize(n_points);

            dev[dir].h[0] = lag_spacing; //primo valore di h

            //cout<<"Computed h(0): "<< dev[dir].h[0] << std::endl;
            for(size_t i = 1; i < dev[dir].h.size(); i++)
                dev[dir].h[i] = dev[dir].h[i-1] + lag_spacing;

        }
        else
        {
            if(coverage < min_dist)
            {
                std::cerr << FRED("ERROR: Coverage is minor than min distance!") << std::endl;
                //exit(1);
                //min_dist = coverage;
            }

            std::cout << "\033[0;33mWARNING: Step factor is set on: " << step << "\033[0m" << std::endl;
            std::cout << "\033[0;33mWARNING: n points is set on: " << n_points << "\033[0m" << std::endl;
            dev[dir].h = lagvar_new(coverage, min_dist, n_points, step, tol_factor, tol);
        }


        dev[dir].N.resize(dev[dir].h.size(), 0.0);
        dev[dir].gamma.resize(dev[dir].h.size());



        // Compute discretized variogram
        std::vector<double> sum (dev[dir].h.size(), 0.0); //vettore somma dei punti che ricadono all'interno della banda di tolleranza (check sulla distanza)

        for(unsigned int row = 0; row < values.size(); row++)
        {
            for(unsigned int col = row+1; col < values.size(); col++)
            {
                double md = sqrt((coord_x[col]-coord_x[row])*(coord_x[col]-coord_x[row])+
                                 (coord_y[col]-coord_y[row])*(coord_y[col]-coord_y[row])+
                                 (coord_z[col]-coord_z[row])*(coord_z[col]-coord_z[row]));

                //calcolo la direzione
                double md_x = coord_x[col]-coord_x[row];
                double md_y = coord_y[col]-coord_y[row];
                double md_z = coord_z[col]-coord_z[row];

                if((md_x ==0) && (md_y ==0))
                {
                    //std::cout << "undefined" << std::endl;
                    continue;
                }
                else
                {
                    double phi_rad = acos(md_z/md);
                    //double theta_rad = acos(md_x/(md*sin(phi_rad)));
                    double theta_rad = atan2(md_y,md_x);
                    double dir_degree = 90 - get_degrees(theta_rad);
                    double vert_dir_degree = 90 - get_degrees(phi_rad);

                    //std::cout << "Computed direction in 3D space (degree) - HORIZONTAL: " << dir_degree << std::endl;
                    //std::cout << "Computed direction in 3D space (degree) - VERTICAL: " << vert_dir_degree << std::endl;

                    if(dir_degree == 180)
                        dir_degree = 0;

                    if(check_dir == 180)
                        check_dir = 0;

                    //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
                    //if(dir_degree >= seq[dir]-hor_degree_tolerance && dir_degree <= seq[dir]+hor_degree_tolerance)
                    if(dir_degree >= check_dir-hor_degree_tolerance && dir_degree <= check_dir+hor_degree_tolerance)
                    {
                        if(vert_dir_degree >= vert_seq-vert_degree_tolerance && vert_dir_degree <= vert_seq+vert_degree_tolerance)
                        {

                            double dretta = point_to_line_distance(0.0, 0.0, theta_rad, md_x, md_y);
                            //cout<<"Computed distance: "<< md << " Indici: row/col " << row << "/" << col << std::endl;

                            if(dretta <= hor_bandwidth)
                            {
                                for(size_t j = 0; j < dev[dir].h.size(); j++)
                                {
                                    double min_tol = 0.0;
                                    double max_tol = 0.0;

                                    if(lagspac_type.compare("CONSTANT") == 0)
                                    {
                                        min_tol = dev[dir].h[j] - tolerance;
                                        max_tol = dev[dir].h[j] + tolerance;
                                    }
                                    else
                                    {
                                        min_tol = dev[dir].h[j] - tol[j];
                                        if(j == dev[dir].h.size()-1) //se sono sull'ultimo lag
                                            max_tol = dev[dir].h[j] + tol[j];
                                        else
                                            max_tol = dev[dir].h[j] + tol[j+1];
                                    }

                                    if(md >= min_tol && md <= max_tol)
                                    {
                                        dev[dir].N[j]+=1; //conto le coppie di punti che rientrano in quella banda di tolleranza

                                        // Compute variogram
                                        double mv = (values[row]-values[col]) * (values[row]-values[col])/2;
                                        sum.at(j) += mv;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        cout<<endl;
        cout<<"################################################"<<endl;
        cout<<"###### DIRECTIONAL EXPERIMENTAL VARIOGRAM ######"<<endl;
        cout<<"########### Direction "<< dir << ": "<<seq[dir]<<" +/- "<<hor_degree_tolerance<<" ############"<<endl;
        cout<<"################################################"<<endl;

        for(unsigned int j = 0; j < dev[dir].h.size(); j++)
        {
            if (dev[dir].N[j] > 0)
                dev[dir].gamma[j] = sum.at(j) / dev[dir].N[j]; //media punti
            else
                dev[dir].gamma[j]=0;

            cout<<dev[dir].N[j] <<" ; "<<dev[dir].h[j]<<" ; "<<dev[dir].gamma[j]<<endl;
        }

        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return dev;
}


///
/// \brief dir3DV_exp_variogram: VERTICAL DIRECTIONAL 3D EXPERIMENTAL VARIOGRAM
/// \param values
/// \param coord_x
/// \param coord_y
/// \param coord_z
/// \param lagspac_type
/// \param seq
/// \param vert_degree_tolerance
/// \param vert_bandwidth
/// \param step
/// \param n_points
/// \param limit_dist
/// \param tol_factor
/// \return
///
std::vector<exp_variog> dir3DV_exp_variogram (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const vector<double> &coord_z,
                                                    const std::string &lagspac_type,
                                                    const double &spacing_samples,
                                                    const std::vector<double> &seq,
                                                    const double &vert_degree_tolerance,
                                                    const double &vert_bandwidth,
                                                    const double &step,
                                                    const int &n_points,
                                                    const double &limit_dist,
                                                    const double &tol_factor)
{
    int n_directions = seq.size();

    std::vector<exp_variog> dev;
    dev.resize(n_directions);

    //double vert_seq = 0.0; //vertical direction for horizontal application

    //Ciclo sulle direzioni e calcolo il variogramma per ciascuna di esse
    for(size_t dir=0; dir<seq.size(); dir++)
    {
        std::cout << std::endl;
        std::cout << "### ID direction: " << dir << std::endl;
        std::cout << "Vertical direction (degree): " << seq.at(dir) << std::endl;
        std::cout << "Vertical angle tolerance (degree): " << vert_degree_tolerance << std::endl;
        std::cout << FYEL("Vertical bandwidth is set on: ") << vert_bandwidth << std::endl;
        std::cout << FYEL("Limit distance is set on: ") << limit_dist << std::endl;

        double max_dist = -DBL_MAX;
        double min_dist = DBL_MAX;
        double coverage = -DBL_MAX;


        // 1) Compute distances (max distances stored)
        for(unsigned int i = 0; i < values.size(); i++)
        {
            for(unsigned int j = i+1; j < values.size(); j++)
            {
                double md = sqrt((coord_x[i]-coord_x[j])*(coord_x[i]-coord_x[j])+
                                 (coord_y[i]-coord_y[j])*(coord_y[i]-coord_y[j])+
                                 (coord_z[i]-coord_z[j])*(coord_z[i]-coord_z[j]));

                //calcolo la direzione
                double md_x = coord_x[j]-coord_x[i];
                double md_y = coord_y[j]-coord_y[i];
                double md_z = coord_z[j]-coord_z[i];

                double phi_rad = acos(md_z/md);
                //double theta_rad = atan2(md_y,md_x);
                //double dir_degree = 90 - get_degrees(theta_rad);
                double vert_dir_degree = get_degrees(phi_rad);

                //std::cout << "Computed direction in 3D space (degree) - HORIZONTAL: " << dir_degree << std::endl;
                //std::cout << "Computed direction in 3D space (degree) - VERTICAL: " << vert_dir_degree << std::endl;

                if(vert_dir_degree == 180)
                    vert_dir_degree = 0;

                //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
                if(vert_dir_degree >= seq[dir]-vert_degree_tolerance && vert_dir_degree <= seq[dir]+vert_degree_tolerance)
                {
                    double dretta = point_to_line_distance(0.0, 0.0, phi_rad, md_x, md_y);
                    //std::cout << "distanza punto retta = " << dretta << std::endl;

                    if(dretta <= vert_bandwidth)
                    {
                        //std::cout << "la distanza punto-retta è minore/uguale della bandwidth (definita in input)" << std::endl;
                        if(md > max_dist)
                            max_dist = md;
                        if(md < min_dist)
                            min_dist = md;
                    }
                }
            }
        }

        if(limit_dist != -DBL_MAX) //se è diverso dal default (!= -DBL_MAX)
            coverage = limit_dist;
        else
            coverage = max_dist/2;
        //else: utilizza come dist_max quella definita in input

        std::cout << "Max distance is set on: " << max_dist << std::endl;
        std::cout << "Min distance is set on: " << min_dist << std::endl;
        std::cout << "Coverage is set on: " << coverage << std::endl;


        vector<double> tol;
        double tolerance = 0.0;

        if(lagspac_type.compare("CONSTANT") == 0)
        {
            // 2) Compute experimental variogram with variable lag (raggruppiamo i punti sul variogramma in modo da avere 15 punti (distanza;gamma))
            //n_points sono il numero dei punti del diagramma discretizzato che voglio ottenere -> devo aggiungere 1 per contare la tacca dell'origine
            int n_lags = (n_points+1)-1;

            double lag_spacing = coverage/n_lags;
            std::cout << "Lag spacing is set on (max_dist/2)/n_lags: " << lag_spacing << std::endl;

            tolerance = lag_spacing/tol_factor;
            std::cout << "\033[0;33mTol_factor is set on: " << tol_factor << "\033[0m" << std::endl;
            std::cout << "Lag tolerance is set on lag_spacing/tol_factor: " << tolerance << std::endl;

            // Tacche sull'asse x: riferimento al vettore seq della funzione del vario_exp
            dev[dir].h.resize(n_points);

            dev[dir].h[0] = lag_spacing; //primo valore di h

            //cout<<"Computed h(0): "<< dev[dir].h[0] << std::endl;
            for(size_t i = 1; i < dev[dir].h.size(); i++)
                dev[dir].h[i] = dev[dir].h[i-1] + lag_spacing;
        }
        else if(lagspac_type.compare("FIXED") == 0) //fissato pari alla spaziatura media tra i campioni
        {
            double lag_spacing = spacing_samples;
            std::cout << "Lag spacing is set on: " << lag_spacing << std::endl;
            tolerance = spacing_samples/tol_factor;
            std::cout << "\033[0;33mTol_factor is set on: " << tol_factor << "\033[0m" << std::endl;
            std::cout << "Lag tolerance is set on lag_spacing/tol_factor: " << tolerance << std::endl;

            // Tacche sull'asse x: riferimento al vettore seq della funzione del vario_exp
            dev[dir].h.resize(coverage/spacing_samples);
            dev[dir].h[0] = lag_spacing; //primo valore di h

            //cout<<"Computed h(0): "<< dev[dir].h[0] << std::endl;
            for(size_t i = 1; i < dev[dir].h.size(); i++)
                dev[dir].h[i] = dev[dir].h[i-1] + lag_spacing;
        }
        else
        {
            dev[dir].h = lagvar_new(coverage, min_dist, n_points, step, tol_factor, tol);
            std::cout << "\033[0;33mWARNING: Step factor is set on: " << step << "\033[0m" << std::endl;
            std::cout << "\033[0;33mWARNING: n points is set on: " << n_points << "\033[0m" << std::endl;
        }



//        dev[dir].N.resize(n_points, 0.0);
//        dev[dir].gamma.resize(n_points);

        dev[dir].N.resize(dev[dir].h.size(), 0.0);
        dev[dir].gamma.resize(dev[dir].h.size());



        // Compute discretized variogram
        std::vector<double> sum (dev[dir].h.size(), 0.0); //vettore somma dei punti che ricadono all'interno della banda di tolleranza (check sulla distanza)

        for(unsigned int row = 0; row < values.size(); row++)
        {
            for(unsigned int col = row+1; col < values.size(); col++)
            {
                double md = sqrt((coord_x[col]-coord_x[row])*(coord_x[col]-coord_x[row])+
                                 (coord_y[col]-coord_y[row])*(coord_y[col]-coord_y[row])+
                                 (coord_z[col]-coord_z[row])*(coord_z[col]-coord_z[row]));

                //calcolo la direzione
                double md_x = coord_x[col]-coord_x[row];
                double md_y = coord_y[col]-coord_y[row];
                double md_z = coord_z[col]-coord_z[row];

                double phi_rad = acos(md_z/md);
                //double theta_rad = acos(md_x/(md*sin(phi_rad)));
                //double theta_rad = atan2(md_y,md_x);
                //double dir_degree = 90 - get_degrees(theta_rad);
                double vert_dir_degree = get_degrees(phi_rad);

                //std::cout << "Computed direction in 3D space (degree) - HORIZONTAL: " << dir_degree << std::endl;
                //std::cout << "Computed direction in 3D space (degree) - VERTICAL: " << vert_dir_degree << std::endl;

                if(vert_dir_degree == 180)
                    vert_dir_degree = 0;

                //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
                if(vert_dir_degree >= seq[dir]-vert_degree_tolerance && vert_dir_degree <= seq[dir]+vert_degree_tolerance)
                {

                    double dretta = point_to_line_distance(0.0, 0.0, phi_rad, md_x, md_y);
                    //cout<<"Computed distance: "<< md << " Indici: row/col " << row << "/" << col << std::endl;

                    if(dretta <= vert_bandwidth)
                    {
                        for(size_t j = 0; j < dev[dir].h.size(); j++)
                        {
                            double min_tol = 0.0;
                            double max_tol = 0.0;

                            if(lagspac_type.compare("CONSTANT") == 0 || lagspac_type.compare("FIXED") == 0)
                            {
                                min_tol = dev[dir].h[j] - tolerance;
                                max_tol = dev[dir].h[j] + tolerance;
                            }
                            else
                            {
                                min_tol = dev[dir].h[j] - tol[j];
                                if(j == dev[dir].h.size()-1) //se sono sull'ultimo lag
                                    max_tol = dev[dir].h[j] + tol[j];
                                else
                                    max_tol = dev[dir].h[j] + tol[j+1];
                            }

                            if(md >= min_tol && md <= max_tol)
                            {
                                dev[dir].N[j]+=1; //conto le coppie di punti che rientrano in quella banda di tolleranza

                                // Compute variogram
                                double mv = (values[row]-values[col]) * (values[row]-values[col])/2;
                                sum.at(j) += mv;
                            }
                        }
                    }
                }
            }
        }

        cout<<endl;
        cout<<"################################################"<<endl;
        cout<<"###### DIRECTIONAL EXPERIMENTAL VARIOGRAM ######"<<endl;
        cout<<"########### Direction "<< dir << ": "<<seq[dir]<<" +/- "<<vert_degree_tolerance<<" ############"<<endl;
        cout<<"################################################"<<endl;

        for(unsigned int j = 0; j < dev[dir].h.size(); j++)
        {
            if (dev[dir].N[j] > 0)
                dev[dir].gamma[j] = sum.at(j) / dev[dir].N[j]; //media punti
            else
                dev[dir].gamma[j]=0;

            cout<<dev[dir].N[j] <<" ; "<<dev[dir].h[j]<<" ; "<<dev[dir].gamma[j]<<endl;
        }

        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return dev;
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// std::vector<exp_variog> dir_exp_variogram_lagvar (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const double &degree_step, const double &degree_tolerance, const int n_points,
//                                                                 const double &limit_dist,
//                                                                 const double &tol_factor,
//                                                                 const double &percent,
//                                                                 bool compute_max)
// {
//     // 0) Definire le direzioni e lavorare su ciascuna di esse
//     int n_directions = 180/degree_step;

//     vector<double> seq(n_directions);
//     seq[0] = 0; //prima direzione: nord geografico (0 gradi)
//     for(size_t i=1; i< seq.size(); i++)
//         seq[i] = seq[i-1] + degree_step;

//     std::vector<exp_variog> dev;
//     dev.resize(n_directions);

//     //Ciclo sulle direzioni e calcolo il variogramma per ciascuna di esse
//     for(size_t dir=0; dir<seq.size(); dir++)
//     {
//         //bool compute_max = false;
// //        if(max_dist == -DBL_MAX) //valore di default del massimo
// //            compute_max = true;

//         double max_dist = -DBL_MAX;

//         if(compute_max == false)
//             max_dist = limit_dist;
//         else //(compute_max == true) //se è sul valore di default, allora calcola il massimo della distanza; altrimenti rimane il valore di input
//         {
//             // 1) Compute distances (max distances stored)
//             for(unsigned int i = 0; i < values.size(); i++)
//             {
//                 for(unsigned int j = i+1; j < values.size(); j++)
//                 {
//                     double md = sqrt((coord_x[i]-coord_x[j])*(coord_x[i]-coord_x[j])+
//                                                     (coord_y[i]-coord_y[j])*(coord_y[i]-coord_y[j]));
// //                    if(md<1)
// //                        md=0; //punti coincidenti (non calcolo la direzione)
// //                    else
// //                    {
//                         //calcolo la direzione
//                         double md_x = coord_x[j]-coord_x[i];
//                         double md_y = coord_y[j]-coord_y[i];

//                         double dir_degree = 90 - (atan2(md_y,md_x) * 180 / M_PI);

//                         if(dir_degree == 180)
//                             dir_degree = 0;

//                         //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
//                         if(dir_degree >= seq[dir]-degree_tolerance && dir_degree <= seq[dir]+degree_tolerance)
//                         {
//                             if(md > max_dist)
//                                 max_dist = md;
//                         }
//                     //}
//                 }
//             }
//         }
//         //else: utilizza come dist_max quella definita in input

//         std::cout << "ID direction: " << dir << std::endl;
//         std::cout << "Computed max distance: " << max_dist << std::endl;


//         // 2) Compute experimental variogram with variable lag (raggruppiamo i punti sul variogramma in modo da avere 15 punti (distanza;gamma))
//         int n_lags = n_points-1;

//         double coverage = max_dist/2;

//         double lag_spacing = coverage/n_lags;
//         std::cout << "\033[0;33mWARNING: Lag spacing is set on (max_dist/2)/n_lags = " << lag_spacing << "\033[0m" << std::endl;

//         double tolerance = lag_spacing/tol_factor;
//         std::cout << "\033[0;33mWARNING: Lag tolerance is set on lag_spacing/tol_factor = " << tolerance << "\033[0m" << std::endl;

//         //double tolerance = lag_spacing/1.0;

//         vector<double> tol(n_points+1);
//         tol[0] = tolerance/4;
//         for(uint i=1;i<tol.size(); i++)
//         {
//             tol[i] = tol[i-1] * (1+percent/100 + i*0.0125);
//             //cout<<"tol[ "<<i<<" ]"<<tol[i]<<endl;
//         }




//         // Tacche sull'asse x: riferimento al vettore seq della funzione del vario_exp
//         dev[dir].h.resize(n_points);

//         dev[dir].h[0] = tol[0] + tol[1]; //primo valore di h

//         //cout<<"Computed h(0): "<< dev[dir].h[0] << std::endl;
//         for(size_t i = 1; i < dev[dir].h.size(); i++)
//         {
//             dev[dir].h[i] = dev[dir].h[i-1] + tol[i] + tol[i+1];

// //            if(dev[dir].h[i]> coverage)
// //                dev[dir].h[i] = coverage;

//             //cout<<"Computed h(j): "<< dev[dir].h[i] << std::endl;
//         }
//         std::cout << "Computing variable lags ... COMPLETED." << std::endl;




//         dev[dir].N.resize(n_points, 0.0);
//         dev[dir].gamma.resize(n_points);



//         // Compute discretized variogram
//         std::vector<double> sum (dev[dir].h.size(), 0.0); //vettore somma dei punti che ricadono all'interno della banda di tolleranza (check sulla distanza)

//         for(unsigned int row = 0; row < values.size(); row++)
//         {
//             for(unsigned int col = row+1; col < values.size(); col++)
//             {
//                 double md = sqrt((coord_x[row]-coord_x[col])*(coord_x[row]-coord_x[col])+
//                                  (coord_y[row]-coord_y[col])*(coord_y[row]-coord_y[col]));

// //                if(md<1)
// //                    md=0; //punti coincidenti (non calcolo la direzione)

// //                else
// //                {
//                     //calcolo la direzione
//                     double md_x = coord_x[col]-coord_x[row];
//                     double md_y = coord_y[col]-coord_y[row];

//                     double dir_degree = 90 - (atan2(md_y,md_x) * 180 / M_PI);

//                     if(dir_degree == 180)
//                         dir_degree = 0;

//                     //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
//                     if(dir_degree >= seq[dir]-degree_tolerance && dir_degree <= seq[dir]+degree_tolerance)
//                     {
//                         //cout<<"Computed distance: "<< md << " Indici: row/col " << row << "/" << col << std::endl;

//                         for(size_t j = 0; j < dev[dir].h.size()-1; j++)
//                         {
//                             //std::cout << "j = " << j << "; h(j) = " << dev[dir].h.at(j) << std::endl;


//                             if(j == dev[dir].h.size()-1) //se sono sull'ultimo lag
//                             {
//                                 double min_tol = dev[dir].h[j] - tol[j];
//                                 //double max_tol = dev[dir].h[j] + tol[j];

//                                 if(md >= min_tol && md <= coverage)
//                                 {
//                                     dev[dir].N[j]+=1; //conto le coppie di punti che rientrano in quella banda di tolleranza

//                                     // Compute variogram
//                                     double mv = (values[row]-values[col]) * (values[row]-values[col])/2;
// //                                    if(mv<0.0001)
// //                                        mv=0.0001;

//                                     sum.at(j) += mv;

//                                     //cout<<"Check on distance: YES. Counter++ Nh = "<< dev[dir].N.at(j) << endl;
//                                 }
//                             }
//                             else
//                             {
//                                 double min_tol = dev[dir].h[j] - tol[j];
//                                 double max_tol = dev[dir].h[j] + tol[j];

//                                 if(md >= min_tol && md <= max_tol)
//                                 {
//                                     dev[dir].N[j]+=1; //conto le coppie di punti che rientrano in quella banda di tolleranza

//                                     // Compute variogram
//                                     double mv = (values[row]-values[col]) * (values[row]-values[col])/2;
// //                                    if(mv<0.0001)
// //                                        mv=0.0001;

//                                     sum.at(j) += mv;

//                                     //cout<<"Check on distance: YES. Counter++ Nh = "<< dev[dir].N.at(j) << endl;
//                                 }
//                             }
//                         }
//                     }
//                 //}
//             }
//         }

//         cout<<endl;
//         cout<<"################################################"<<endl;
//         cout<<"###### DIRECTIONAL EXPERIMENTAL VARIOGRAM ######"<<endl;
//         cout<<"########### Direction "<< dir << ": "<<seq[dir]<<" +/- "<<degree_tolerance<<" ############"<<endl;
//         cout<<"################################################"<<endl;

//         for(unsigned int j = 0; j < dev[dir].h.size(); j++)
//         {
//             if (dev[dir].N[j] > 0)
//                 dev[dir].gamma[j] = sum.at(j) / dev[dir].N[j]; //media punti
//             else
//                 dev[dir].gamma[j]=0;

//             cout<<dev[dir].N[j] <<" ; "<<dev[dir].h[j]<<" ; "<<dev[dir].gamma[j]<<endl;
//         }

//         cout<<"####################################"<<endl;
//         cout<<"####################################"<<endl;
//         cout<<endl;
//     }

//     return dev;
// }


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
/// \brief lagvar computes the variable lag spacing and variable tolerance
/// \param coverage is the max extension of the variogram (assumed as dist_max/2)
/// \param dist_min is the min distance between pair of points
/// \param factor is the step factor, to divide h axis (from 0 to coverage/2) step-by-step
/// \param n_points is the number of points for discretized variogram
/// \param n_points_start is the number of points in the first zone (from 0 to coverage/2)
/// \param tol_factor is factor to divide tolerance
/// \param tol is the vector of tolerance
/// \return vector of lag h (and tolerance tol)
///
std::vector<double> lagvar (const double &coverage, const double &dist_min, const double &factor, const int &n_points, const int &n_points_start, const double &tol_factor, std::vector<double> &tol)
{
    std::cout << std::endl;
    std::cout << FMAG("##############################################") << std::endl;
    std::cout << FMAG("NUOVO METODO PER IL CALCOLO DI LAG SPACING VARIABILE") << std::endl;
    std::cout << FMAG("Da 0 a coverage/2: i lag variabili vengono calcolati su n_points_start, dividendo in maniera ricorsiva la spaziatura tra i lag") << std::endl;
    std::cout << FMAG("Da coverage/2 a coverage: i lag vengono calcolati su n_points - n_points_start, applicando un lag spacing constante") << std::endl;
    std::cout << FMAG("##############################################") << std::endl;
    std::cout << std::endl;

//    if(coverage < dist_min)
//    {
//        std::cout << FRED("ERROR: coverage is minor than min distance between points.") << std::endl;
//        exit(1);
//    }

    std::vector<double> h;

    //double m = coverage * 0.5; //divido la coverage in due: la prima parte viene infittita rispettando n_points_start, la seconda parte applico la spaziatura costante
    double m = (coverage-dist_min) * 0.5 + dist_min; //divido la coverage in due: la prima parte viene infittita rispettando n_points_start, la seconda parte applico la spaziatura costante

    h.push_back(m);
    for(size_t i=1; i<n_points_start; i++)
    {
        double p = h.at(i-1)*factor;

        if(p >= dist_min)
            h.push_back(p);
        else
            break;

        //h.push_back(p);

        //std::cout << p << std::endl;
    }
    sort(h.begin(), h.end());

    //int n_points_end = (n_points - n_points_start) + 1;
    int n_points_end = (n_points - h.size()) + 1; //il +1 è per il punto sovrapposto
    double lag_spacing = (coverage - m)/(n_points_end-1);

    size_t id_start = h.size();

    for(size_t i=id_start; i<n_points; i++)
    {
        double p = h.at(i-1)+lag_spacing;
        h.push_back(p);
    }

    tol.push_back(h.at(0)/tol_factor);
    for(size_t i = 1; i < h.size(); i++)
    {
        double delta = h.at(i) - h.at(i-1);
        //std::cout << "tolerance = " << delta << std::endl;
        tol.push_back(delta/tol_factor);
    }


//    for(size_t i=0; i<h.size(); i++)
//    {
//        std::cout << h.at(i) << std::endl;
//    }

    return h;
}

///
/// \brief lagvar1 computes the variable lag spacing and variable tolerance, as implemented in the geostaslib
/// \param coverage is the max extension of the variogram (assumed as dist_max/2)
/// \param n_points is the number of points for discretized variogram
/// \param percent is fixed to 10 (default)
/// \param tol is the vector of variable tolerance
/// \return vector of lag h (and tolerance tol)
///
std::vector<double> lagvar1 (const double &coverage, const double &n_points, const double &percent, std::vector<double> &tol)
{
    std::vector<double> h;

    int n_lags = n_points-1;
    double lag_spacing = coverage/n_lags;

    tol.resize(n_points+1);
    tol[0] = lag_spacing/4;

    for(uint i=1; i<tol.size(); i++)
    {
        tol[i] = tol[i-1] * (1 + percent/100 + i*0.0125);
        cout<<"tol[ "<<i<<" ]"<<tol[i]<<endl;
    }

    h.resize(n_points);
    h[0] = tol[0] + tol[1]; //primo valore di h

    for(size_t i = 1; i < h.size(); i++)
    {
        h[i] = h[i-1] + tol[i] + tol[i+1];
    }
    std::cout << "Computing variable lags ... COMPLETED." << std::endl;

    return h;
}


std::vector<double> lagvar2 (const double &coverage, const double &dist_min, const double &step_factor, const int &n_points)
{
    std::vector<double> h;

    double m = coverage * 0.5; //divido la coverage in due

    h.push_back(m);
    for(size_t i=1; i<n_points; i++)
    {
        double p = h.at(i-1)*step_factor;
        if(p >= dist_min)
            h.push_back(p);
        else
            break;

        //std::cout << p << std::endl;
    }
    sort(h.begin(), h.end());

    int n_points_end = (n_points - h.size()) + 1; //il +1 è per il punto sovrapposto
    double lag_spacing = (coverage - m)/(n_points_end-1);

    size_t id_start = h.size();

    for(size_t i=id_start; i<n_points; i++)
    {
        double p = h.at(i-1)+lag_spacing;
        h.push_back(p);
    }


    for(size_t i=0; i<h.size(); i++)
    {
        std::cout << h.at(i) << std::endl;
    }

    return h;
}


std::vector<double> lagvar_new (const double &coverage, const double &dist_min, const int &n_points, const double &delta_factor, const double &tol_factor, std::vector<double> &tol)
{
    if(coverage < dist_min)
    {
        std::cerr << "ERROR: Coverage is minor than minimum distance!" << std::endl;
        exit(1);
    }

    std::vector<double> h_temp; //(n_points, 0.0);

    double h0 = dist_min;
    if(dist_min == 0)
    {
        double lag_const = (coverage-dist_min)/(n_points-1);
        //double lag_const = (coverage)/(n_points+1-1);
        std::cout << "### Constant lag is equal to " << lag_const << std::endl;

        h0 = lag_const/4;
    }
    std::cout << "### h0 is set on " << h0 << std::endl;
    h_temp.push_back(h0);

    double delta = (delta_factor * 100)/(n_points);
    //std::cout << "### delta =" << delta << std::endl;

    std::vector<double> fac(n_points, 0.0);
    for(size_t i=0; i< n_points; i++)
    {
        if(i==0)
            fac.at(i) = delta;
        else
            fac.at(i) = fac.at(i-1) + delta;
    }

    for(size_t i=0; i< fac.size(); i++)
    {
        double spac = 0.0;
        if(i==0)
            spac = h0;
            //spac = lag_const/4;
        else
            spac = h_temp.at(i) - h_temp.at(i-1);

        double v = h_temp.at(i) + spac + spac*fac.at(i)/100;

        if(v < coverage)
            h_temp.push_back(v);
        else
            break;
    }

    if (n_points > h_temp.size())
    {
        double m_diff = coverage - h_temp.at(h_temp.size()-1);
        //std::cout << "m_diff =" << m_diff << std::endl;

        double last_spac = 0.0;
        if(h_temp.size() > 1)
            last_spac = h_temp.at(h_temp.size()-1) - h_temp.at(h_temp.size()-2);
        else
            last_spac = h_temp.at(h_temp.size()-1);

        int n_last_spac = std::round(m_diff/last_spac);
        //std::cout << "n_last_spac =" << n_last_spac << std::endl;

        int j=0;
        while(j < n_last_spac)
        {
            double p = h_temp.at(h_temp.size()-1) + last_spac;
            h_temp.push_back(p);

            j++;
        }
    }

    //tol.push_back((lag_const/4)/tol_factor);
    tol.push_back(h0/tol_factor);
    for(size_t i = 1; i < h_temp.size(); i++)
    {
        double delta = h_temp.at(i) - h_temp.at(i-1);
        //std::cout << "tolerance = " << delta << std::endl;
        tol.push_back(delta/tol_factor);
    }
    return h_temp;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
/// \brief dir_exp_variogram_lagvar: DIRECTIONAL 2D EXPERIMENTAL VARIOGRAM - VARIABLE LAG SPACING
/// \param values
/// \param coord_x
/// \param coord_y
/// \param seq
/// \param degree_tolerance
/// \param bandwidth
/// \param step
/// \param n_points
/// \param limit_dist
/// \param tol_factor
/// \return
///
std::vector<exp_variog> dir_exp_variogram_lagvar (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y,
                                                    const std::vector<double> &seq,
                                                    const double &degree_tolerance,
                                                    const double &bandwidth,
                                                    const double &step,
                                                    const int &n_points,
                                                    const double &limit_dist,
                                                    const double &tol_factor)
{
    int n_directions = seq.size();

    std::vector<exp_variog> dev;
    dev.resize(n_directions);

    //Ciclo sulle direzioni e calcolo il variogramma per ciascuna di esse
    for(size_t dir=0; dir<seq.size(); dir++)
    {
        double check_dir = seq[dir];

        std::cout << std::endl;
        std::cout << "### ID direction: " << dir << std::endl;
        std::cout << "Direction (degree): " << seq.at(dir) << std::endl;
        std::cout << "Degree tolerance (degree): " << degree_tolerance << std::endl;
        std::cout << "\033[0;33mBandwidth is set on: " << bandwidth << "\033[0m" << std::endl;
        std::cout << "\033[0;33mLimit distance is set on: " << limit_dist << "\033[0m" << std::endl;

        double max_dist = -DBL_MAX;
        double min_dist = DBL_MAX;
        double coverage = -DBL_MAX;

        // 1) Compute distances (max distances stored)
        for(unsigned int i = 0; i < values.size(); i++)
        {
            for(unsigned int j = i+1; j < values.size(); j++)
            {
                double md = sqrt((coord_x[i]-coord_x[j])*(coord_x[i]-coord_x[j])+
                                                (coord_y[i]-coord_y[j])*(coord_y[i]-coord_y[j]));
                //calcolo la direzione di md
                double md_x = coord_x[j]-coord_x[i];
                double md_y = coord_y[j]-coord_y[i];

                if((md_x ==0) && (md_y ==0))
                {
                    //std::cout << "undefined" << std::endl;
                    continue;
                }
                else
                {
                    double alfa_rad = atan2(md_y,md_x);
                    double dir_degree = 90 - get_degrees(alfa_rad);

                    //double dir_degree = 90 - (atan2(md_y,md_x) * 180 / M_PI);
    //                    std::cout << "degree = " << get_degrees(alfa_rad) << std::endl;
    //                    std::cout << "dir degree= " << dir_degree << std::endl;

                    if(dir_degree == 180)
                        dir_degree = 0;

                    if(check_dir == 180)
                        check_dir = 0;

                    //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
                    if(dir_degree >= check_dir-degree_tolerance && dir_degree <= check_dir+degree_tolerance)
                    {
                        double dretta = point_to_line_distance(0.0, 0.0, alfa_rad, md_x, md_y);
                        //std::cout << "distanza punto retta = " << dretta << std::endl;

                        if(dretta <= bandwidth)
                        {
                            //std::cout << "la distanza punto-retta è minore/uguale della bandwidth (definita in input)" << std::endl;
                            if(md > max_dist)
                                max_dist = md;
                            if(md < min_dist)
                                min_dist = md;
                        }
                    }
                }
            }
        }
        //coverage = (max_dist-min_dist)/2 + min_dist;

        if(limit_dist != -DBL_MAX) //se è diverso dal default (!= -DBL_MAX)
            coverage = limit_dist;
        else
            coverage = max_dist/2;

        std::cout << "Max distance is set on: " << max_dist << std::endl;
        std::cout << "Min distance is set on: " << min_dist << std::endl;
        std::cout << "Coverage is set on: " << coverage << std::endl;

        if(max_dist == -DBL_MAX || min_dist == DBL_MAX)
        {
            std::cout << FRED("Cannot compute variogram in direction: " << seq[dir] << " degree. No pair of points in that direction!") << std::endl;
            std::cout << std::endl;

            dev[dir].h.resize(n_points, 0.0);
            dev[dir].N.resize(n_points, 0.0);
            dev[dir].gamma.resize(n_points, 0.0);

            continue;
        }


        // 2) Compute experimental variogram with variable lag (raggruppiamo i punti sul variogramma in modo da avere 15 punti (distanza;gamma))
        //n_points sono il numero dei punti del diagramma discretizzato che voglio ottenere -> devo aggiungere 1 per contare la tacca dell'origine
        //int n_lags = n_points-1;

        //double lag_spacing = coverage/n_lags;
        //std::cout << "\033[0;33mWARNING: Lag spacing is set on (max_dist/2)/n_lags = " << lag_spacing << "\033[0m" << std::endl;

        //double tolerance = lag_spacing/tol_factor;
        //std::cout << "\033[0;33mWARNING: Lag tolerance is set on lag_spacing/tol_factor = " << tolerance << "\033[0m" << std::endl;


        /*vector<double> tol(n_points+1);
        tol[0] = tolerance/4;
        for(uint i=1;i<tol.size(); i++)
        {
            tol[i] = tol[i-1] * (1+percent/100 + i*0.0125);
            //cout<<"tol[ "<<i<<" ]"<<tol[i]<<endl;
        }

        // Tacche sull'asse x: riferimento al vettore seq della funzione del vario_exp
        dev[dir].h.resize(n_points);

        dev[dir].h[0] = tol[0] + tol[1]; //primo valore di h

        //cout<<"Computed h(0): "<< dev[dir].h[0] << std::endl;

        //bool compute_scale_h = false;
        for(size_t i = 1; i < dev[dir].h.size(); i++)
        {
            dev[dir].h[i] = dev[dir].h[i-1] + tol[i] + tol[i+1];

//            if(dev[dir].h[i] > coverage)
//                compute_scale_h = true;
                //dev[dir].h[i] = coverage;

            //cout<<"Computed h(j): "<< dev[dir].h[i] << std::endl;
        }
        std::cout << "Computing variable lags ... COMPLETED." << std::endl;*/


//        if(compute_scale_h == true)
//        {
//            double scale_factor = dev[dir].h.at(dev[dir].h.size()-1)/coverage;
//            std::cout << "Scaling h-axis by scale factor of: " << scale_factor << std::endl;
//            for(size_t i=0; i< dev[dir].h.size(); i++)
//            {
//                dev[dir].h.at(i) = dev[dir].h.at(i)/scale_factor;
//                cout<<"Computed scaled h(j): "<< dev[dir].h[i] << std::endl;
//            }
//        }




        //Nuovo metodo per il calcolo delle h a lag variabile + tolerance
        vector<double> tol;
        //dev[dir].h = lagvar1(coverage, n_points, percent, tol);



        //dev[dir].h = lagvar(coverage, min_dist, step, n_points, n_points_start, tol_factor, tol);
        dev[dir].h = lagvar_new(coverage, min_dist, n_points, step, tol_factor, tol);


//        tol.push_back(0.0);
//        for(size_t i = 1; i < dev[dir].h.size(); i++)
//        {
//            double delta = dev[dir].h.at(i) - dev[dir].h.at(i-1);
//            //std::cout << "tolerance = " << delta << std::endl;

//            tol.push_back(delta);
//        }


        //dev[dir].h = lagvar2(coverage, min_dist, step, n_points);

//        tol.push_back(dev[dir].h.at(0)/tol_factor);
//        for(size_t i = 1; i < dev[dir].h.size(); i++)
//        {
//            double delta = dev[dir].h.at(i) - dev[dir].h.at(i-1);
//            //std::cout << "tolerance = " << delta << std::endl;

//            tol.push_back(delta/tol_factor);
//        }

        std::cout << "\033[0;33mWARNING: Step factor is set on: " << step << "\033[0m" << std::endl;
        std::cout << "\033[0;33mWARNING: n points is set on: " << n_points << "\033[0m" << std::endl;
        //std::cout << "\033[0;33mWARNING: n start points is set on: " << n_points_start << "\033[0m" << std::endl;




        dev[dir].N.resize(dev[dir].h.size(), 0.0);
        dev[dir].gamma.resize(dev[dir].h.size());



        // Compute discretized variogram
        std::vector<double> sum (dev[dir].h.size(), 0.0); //vettore somma dei punti che ricadono all'interno della banda di tolleranza (check sulla distanza)

        for(unsigned int row = 0; row < values.size(); row++)
        {
            for(unsigned int col = row+1; col < values.size(); col++)
            {
                double md = sqrt((coord_x[row]-coord_x[col])*(coord_x[row]-coord_x[col])+
                                 (coord_y[row]-coord_y[col])*(coord_y[row]-coord_y[col]));

                //calcolo la direzione
                double md_x = coord_x[col]-coord_x[row];
                double md_y = coord_y[col]-coord_y[row];

                if((md_x ==0) && (md_y ==0))
                {
                    //std::cout << "undefined" << std::endl;
                    continue;
                }
                else
                {
                    double alfa_rad = atan2(md_y,md_x);
                    double dir_degree = 90 - get_degrees(alfa_rad);

                    //double dir_degree = 90 - (atan2(md_y,md_x) * 180 / M_PI);
    //                    std::cout << "degree = " << get_degrees(alfa_rad) << std::endl;
    //                    std::cout << "dir degree= " << dir_degree << std::endl;

                    if(dir_degree == 180)
                        dir_degree = 0;

                    if(check_dir == 180)
                        check_dir = 0;

                    //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
                    if(dir_degree >= check_dir-degree_tolerance && dir_degree <= check_dir+degree_tolerance)
                    {
                        double dretta = point_to_line_distance(0.0, 0.0, alfa_rad, md_x, md_y);
                        //std::cout << "distanza punto retta = " << dretta << std::endl;

                        if(dretta <= bandwidth)
                        {
                            //std::cout << "la distanza punto-retta è minore/uguale della bandwidth (definita in input)" << std::endl;
                            //cout<<"Computed distance: "<< md << " Indici: row/col " << row << "/" << col << std::endl;

                            for(size_t j = 0; j < dev[dir].h.size(); j++)
                            {
                                double min_tol = dev[dir].h[j] - tol[j];
                                double max_tol = 0.0;
                                if(j == dev[dir].h.size()-1) //se sono sull'ultimo lag
                                    max_tol = dev[dir].h[j] + tol[j];
                                else
                                    max_tol = dev[dir].h[j] + tol[j+1];


                                if(md >= min_tol && md <= max_tol)
                                {
                                    dev[dir].N[j]+=1; //conto le coppie di punti che rientrano in quella banda di tolleranza

                                    // Compute variogram
                                    double mv = (values[row]-values[col]) * (values[row]-values[col])/2;
                                    sum.at(j) += mv;
                                }
                            }
                        }
                    }
                }
            }
        }

        cout<<endl;
        cout<<"################################################"<<endl;
        cout<<"###### DIRECTIONAL EXPERIMENTAL VARIOGRAM ######"<<endl;
        cout<<"########### Direction "<< dir << ": "<<seq[dir]<<" +/- "<<degree_tolerance<<" ############"<<endl;
        cout<<"################################################"<<endl;

        for(unsigned int j = 0; j < dev[dir].h.size(); j++)
        {
            if (dev[dir].N[j] > 0)
                dev[dir].gamma[j] = sum.at(j) / dev[dir].N[j]; //media punti
            else
                dev[dir].gamma[j]=0;

            cout<<dev[dir].N[j] <<" ; "<<dev[dir].h[j]<<" ; "<<dev[dir].gamma[j]<<endl;
        }

        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return dev;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
/// \brief dir_exp_variogram: DIRECTIONAL 2D EXPERIMENTAL VARIOGRAM
/// \param values
/// \param coord_x
/// \param coord_y
/// \param lagspac_type
/// \param seq
/// \param degree_tolerance
/// \param bandwidth
/// \param step
/// \param n_points
/// \param n_points_start
/// \param limit_dist
/// \param tol_factor
/// \param percent
/// \return
///
std::vector<exp_variog> dir_exp_variogram (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y,
                                                    const std::string &lagspac_type,
                                                    const std::vector<double> &seq,
                                                    const double &degree_tolerance,
                                                    const double &bandwidth,
                                                    const double &step,
                                                    const int &n_points,
                                                    const double &limit_dist,
                                                    const double &tol_factor)
{
    if(lagspac_type.compare("VARIABLE") == 0)
    {
        std::cout << "##### Directional experimental variogram with variable lag spacing ..." << std::endl;
        return dir_exp_variogram_lagvar(values, coord_x, coord_y, seq, degree_tolerance, bandwidth, step, n_points, limit_dist, tol_factor);
    }
    else if(lagspac_type.compare("CONSTANT") == 0)
    {
        std::cout << "##### Directional experimental variogram with constant lag spacing ..." << std::endl;
        return dir_exp_variogram_lagconst(values, coord_x, coord_y, seq, degree_tolerance, bandwidth, n_points, limit_dist, tol_factor);
    }
    else
        return std::vector<exp_variog>();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//DA CONTROLLARE
std::vector<exp_variog> dir_exp_variogram_range (const vector<double> &values, const vector<double> &coord_x, const vector<double> &coord_y, const double &degree_step, const double &degree_tolerance, const int n_points, double range_omni)
{
    // 0) Definire le direzioni e lavorare su ciascuna di esse
    int n_directions = 180/degree_step;

    vector<double> seq(n_directions);
    seq[0] = 0; //prima direzione: nord geografico (0 gradi)
    for(size_t i=1; i< seq.size(); i++)
        seq[i] = seq[i-1] + degree_step;

    std::vector<exp_variog> dev;
    dev.resize(n_directions);

    //Ciclo sulle direzioni e calcolo il variogramma per ciascuna di esse
    for(size_t dir=0; dir<seq.size(); dir++)
    {
        // 2) Compute experimental variogram with variable lag (raggruppiamo i punti sul variogramma in modo da avere 15 punti (distanza;gamma))
        int n_lags = n_points-1;

        double lag_spacing = (range_omni*1.5)/n_lags;
        std::cout << "WARNING: Lag spacing is set on (range_omni*1.5)/n_lags = " << lag_spacing << std::endl;


        // Tacche sull'asse x: riferimento al vettore seq della funzione del vario_exp
        dev[dir].h.resize(n_points);

        dev[dir].h[0] = lag_spacing; //primo valore di h

        //cout<<"Computed h(0): "<< dev[dir].h[0] << std::endl;
        for(size_t i = 1; i < dev[dir].h.size(); i++)
        {
            dev[dir].h[i] = dev[dir].h[i-1] + lag_spacing;

//            if(dev[dir].h[i]> max_dist)
//                dev[dir].h[i] = max_dist;

        }

        dev[dir].N.resize(n_points, 0.0);
        dev[dir].gamma.resize(n_points);

        double tolerance = lag_spacing/1.5;
        std::cout << "WARNING: Lag tolerance is set on lag_spacing/1.5 = " << tolerance << std::endl;


        // Compute discretized variogram
        std::vector<double> sum (dev[dir].h.size(), 0.0); //vettore somma dei punti che ricadono all'interno della banda di tolleranza (check sulla distanza)

        for(unsigned int row = 0; row < values.size(); row++)
        {
            for(unsigned int col = row+1; col < values.size(); col++)
            {
                double md = sqrt((coord_x[row]-coord_x[col])*(coord_x[row]-coord_x[col])+
                                 (coord_y[row]-coord_y[col])*(coord_y[row]-coord_y[col]));

                if(md<1)
                    md=0; //punti coincidenti (non calcolo la direzione)

                else
                {
                    //calcolo la direzione
                    double md_x = coord_x[col]-coord_x[row];
                    double md_y = coord_y[col]-coord_y[row];

                    double dir_degree = 90 - (atan2(md_y,md_x) * 180 / M_PI);

                    //se dir_degree rientra nella fascia che sto considerando (-/+tolerance_degree)
                    if(dir_degree >= seq[dir]-degree_tolerance && dir_degree <= seq[dir]+degree_tolerance)
                    {
                        //cout<<"Computed distance: "<< md << " Indici: row/col " << row << "/" << col << std::endl;

                        for(size_t j = 0; j < dev[dir].h.size(); j++)
                        {
                            //std::cout << "j = " << j << "; h(j) = " << dev[dir].h.at(j) << std::endl;

                            double min_tol = dev[dir].h[j] - tolerance;
                            double max_tol = dev[dir].h[j] + tolerance;

                            //check sulla distanza massima! valutare se lasciare o meno (dato che sto considerando la distmax/2)
                            //fare il check sulla distmax/2??
//                            if(max_tol >= max_dist)
//                                max_tol = max_dist;

                            if(md >= min_tol && md<=max_tol)
                            {
                                dev[dir].N[j]+=1; //conto le coppie di punti che rientrano in quella banda di tolleranza

                                // Compute variogram
                                double mv = (values[row]-values[col]) * (values[row]-values[col])/2;
                                if(mv<0.0001)
                                    mv=0.0001;

                                sum.at(j) += mv;

                                //cout<<"Check on distance: YES. Counter++ Nh = "<< dev[dir].N.at(j) << endl;
                            }
                        }
                    }
                }
            }
        }

        cout<<endl;
        cout<<"################################################"<<endl;
        cout<<"###### DIRECTIONAL EXPERIMENTAL VARIOGRAM ######"<<endl;
        cout<<"########### Direction "<< dir << ": "<<seq[dir]<<" +/- "<<degree_tolerance<<" ############"<<endl;
        cout<<"################################################"<<endl;

        for(unsigned int j = 0; j < dev[dir].h.size(); j++)
        {
            if (dev[dir].N[j] > 0)
                dev[dir].gamma[j] = sum.at(j) / dev[dir].N[j]; //media punti
            else
                dev[dir].gamma[j]=0;

            cout<<dev[dir].N[j] <<" ; "<<dev[dir].h[j]<<" ; "<<dev[dir].gamma[j]<<endl;
        }

        cout<<"####################################"<<endl;
        cout<<"####################################"<<endl;
        cout<<endl;
    }

    return dev;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//Funzione di creazione nuovi lag che garantiscono stabilità variogramma (accorpamento dei punti del variogramma fino a raggiungere un numero minimo di coppie di punti per ciascun lag)
//Miglioramento: stabilità, riduzione del rumore e qualità del fitting del modello teorico (soprattutto per i primi lag, che sono quelli più importanti per il fitting del modello teorico)
exp_variog clean_exp_variogram (const exp_variog &ev, const uint min_npoints)
{
    double c=0, b=0;
    uint ntot=0;

    exp_variog clean_ev;

    for(unsigned int l = 0; l < ev.N.size(); l++)
    {
        //variabile di accumulo del numero di coppie per il lag corrente
        //serve per verificare se ho abbastanza coppie per considerare il punto del variogramma come "valido" (altrimenti lo accorpo al punto precedente, fino a raggiungere un numero di coppie sufficiente)
        ntot = ntot + ev.N[l];

        if(ntot<min_npoints)
        {
            c = c + ev.gamma[l]*ev.N[l]; //semivarianza pesata
            b = b + ev.h[l]*ev.N[l]; //distanza pesata
            //calcolo pesato per tenere conto del numero di coppie che contribuiscono a quel punto del variogramma (ev.N[l])
        }
        else
        {
            //calcolo media pesata
            clean_ev.N.push_back(ntot);
            clean_ev.h.push_back((b + ev.h[l]*ev.N[l]) / ntot); //nuovo lag
            clean_ev.gamma.push_back((c + ev.gamma[l]*ev.N[l]) / ntot);
            
            //reset variabili di accumulo per ricominciare con il lag successivo del variogramma
            ntot = 0;
            c = 0;
            b = 0;
        }
    }
    if(ntot>0) //se alla fine del ciclo ho ancora coppie di punti che non ho considerato (perché non raggiungono il numero minimo), le accorpo all'ultimo punto del variogramma
    {
        clean_ev.N.push_back(ntot);
        clean_ev.h.push_back(b / ntot);
        clean_ev.gamma.push_back(c / ntot);
    }


    cout<<endl;
    cout<<"##############################################"<<endl;
    cout<<"###### EXPERIMENTAL VARIOGRAM (cleaned) ######"<<endl;
    cout<<"##############################################"<<endl;
    for(uint j=0;j<clean_ev.N.size();j++)
    cout<<clean_ev.N[j] <<" ; "<<clean_ev.h[j]<<" ; "<<clean_ev.gamma[j]<<endl;
    cout<<"##############################################"<<endl;
    cout<<"##############################################"<<endl;
    cout<<endl;

    return clean_ev;
}


exp_variog clean_exp_variogram (const exp_variog &ev, const uint min_npoints, int &nzeros)
{
    double c=0, b=0;
    uint ntot=0;

    exp_variog clean_ev;

    double limit_for_zeros = 4*ev.h.at(0);
    std::cout << "### h limit for counting of zeros (4*ev.h.at(0)): " << limit_for_zeros << std::endl;

    nzeros = 0;
    for(unsigned int l = 0; l < ev.N.size(); l++)
    {
        if(ev.h.at(l) <= limit_for_zeros)
            if(ev.N.at(l) == 0)
                nzeros++;

        ntot = ntot + ev.N[l];

        if(ntot<min_npoints)
        {
            c = c + ev.gamma[l]*ev.N[l];
            b = b + ev.h[l]*ev.N[l];
        }
        else
        {
            clean_ev.N.push_back(ntot);
            //clean_ev.h.push_back(ev.h[l]);
            clean_ev.h.push_back((b + ev.h[l]*ev.N[l]) / ntot);
            clean_ev.gamma.push_back((c + ev.gamma[l]*ev.N[l]) / ntot);
            ntot = 0;
            c = 0;
            b = 0;
        }
    }

    if(ntot>0) //se alla fine del ciclo ho ancora coppie di punti che non ho considerato (perché non raggiungono il numero minimo), le accorpo all'ultimo punto del variogramma
    {
        clean_ev.N.push_back(ntot);
        clean_ev.h.push_back(b / ntot);
        clean_ev.gamma.push_back(c / ntot);
    }

    std::cout << "### Numbers of counted zeros: " << nzeros << std::endl;


    cout<<endl;
    cout<<"##############################################"<<endl;
    cout<<"###### EXPERIMENTAL VARIOGRAM (cleaned) ######"<<endl;
    cout<<"##############################################"<<endl;
    for(uint j=0;j<clean_ev.N.size();j++)
    cout<<clean_ev.N[j] <<" ; "<<clean_ev.h[j]<<" ; "<<clean_ev.gamma[j]<<endl;
    cout<<"##############################################"<<endl;
    cout<<"##############################################"<<endl;
    cout<<endl;

    return clean_ev;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

/// \brief Diagnose experimental variogram - check for anomalies, outliers, and potential issues
/// \param ev experimental variogram
/// \param values data values used to compute the variogram
void diagnose_exp_variogram (const exp_variog &ev, const std::vector<double> &values)
{
    using namespace std;
    cout << endl;
    cout << "################################### VARIOGRAM DIAGNOSTICS ###################################" << endl;
    
    // Statistics on gamma values
    double min_gamma = DBL_MAX;
    double max_gamma = -DBL_MAX;
    double sum_gamma = 0.0;
    int count_gamma = 0;
    
    for(size_t i = 0; i < ev.gamma.size(); i++)
    {
        if(ev.N[i] > 0)  // Only count lags with data
        {
            min_gamma = min(min_gamma, ev.gamma[i]);
            max_gamma = max(max_gamma, ev.gamma[i]);
            sum_gamma += ev.gamma[i];
            count_gamma++;
        }
    }
    
    double avg_gamma = (count_gamma > 0) ? sum_gamma / count_gamma : 0.0;
    
    // Standard deviation
    double sum_var = 0.0;
    for(size_t i = 0; i < ev.gamma.size(); i++)
    {
        if(ev.N[i] > 0)
        {
            sum_var += pow(ev.gamma[i] - avg_gamma, 2);
        }
    }
    double std_gamma = (count_gamma > 0) ? sqrt(sum_var / count_gamma) : 0.0;
    
    cout << "\n=== GAMMA STATISTICS ===" << endl;
    cout << "Min gamma: " << min_gamma << endl;
    cout << "Max gamma: " << max_gamma << endl;
    cout << "Mean gamma: " << avg_gamma << endl;
    cout << "Std Dev gamma: " << std_gamma << endl;
    cout << "Lags with data: " << count_gamma << " / " << ev.gamma.size() << endl;
    
    // Data value statistics
    double min_val = *min_element(values.begin(), values.end());
    double max_val = *max_element(values.begin(), values.end());
    double sum_val = accumulate(values.begin(), values.end(), 0.0);
    double avg_val = sum_val / values.size();
    
    double variance_data = 0.0;
    for(double v : values)
    {
        variance_data += pow(v - avg_val, 2);
    }
    variance_data /= values.size();
    
    cout << "\n=== DATA STATISTICS ===" << endl;
    cout << "Min value: " << min_val << endl;
    cout << "Max value: " << max_val << endl;
    cout << "Mean value: " << avg_val << endl;
    cout << "Data variance: " << variance_data << endl;
    cout << "Data range: " << (max_val - min_val) << endl;
    cout << "Max diff^2/2: " << pow(max_val - min_val, 2) / 2 << " (theoretical max gamma)" << endl;
    
    // Check for anomalies
    cout << "\n=== ANOMALY DETECTION ===" << endl;
    
    double threshold_lpc = avg_gamma + 2.0 * std_gamma;  // 2 sigma rule
    bool has_anomalies = false;
    
    for(size_t i = 0; i < ev.gamma.size(); i++)
    {
        if(ev.N[i] > 0 && ev.gamma[i] > threshold_lpc)
        {
            cout << "WARNING: Lag #" << i << " (h=" << ev.h[i] << ") has high gamma=" << ev.gamma[i] 
                 << " (N=" << ev.N[i] << " pairs)" << endl;
            has_anomalies = true;
        }
    }
    
    // Check sill consistency
    if(max_gamma > 1.2 && fabs(variance_data - 1.0) < 0.1)  // Should be ~1 for normal score
    {
        cout << "\nWARNING: Data appears to be normal score (variance=" << variance_data 
             << "), but gamma reaches " << max_gamma << "." << endl;
        cout << "Possible causes:" << endl;
        cout << "  1. Incomplete normal score transformation" << endl;
        cout << "  2. Directional anisotropy in this direction" << endl;
        cout << "  3. Outliers in the data after NS transformation" << endl;
    }
    
    cout << "\n#############################################################################################################" << endl;
    cout << endl;
}