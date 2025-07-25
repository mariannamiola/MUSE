MUSE = Modelling of Uncertainty as a Support of Environment; Plot tool


USAGE: 

   plot  [-BEHhT] [--version] [-p <path>] [-x <file>] [-y <file>] [-z
         <file>]


Where: 

   -H,  --histogram
     Creation histogram plot

   -p <path>,  --pdir <path>
     Project directory

   -x <file>,  --x_variable <file>
     Name variable1

   -B,  --bivariate_plot
     Creation bivariate plot

   -y <file>,  --y_variable <file>
     Name variable2

   -E,  --error_plot
     Creation error plot

   -T,  --triangular_plot
     Creation triangular plot

   -z <file>,  --z_variable <file>
     Name variable3

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.



EXAMPLE: 

0. Histogram plot
   plot 
   ./plot -H -p /Users/mariannamiola/Desktop/work -x T
   
1. Bivariate plot
   ./plot -B -p /Users/mariannamiola/Desktop/work -x T -y Cr

2. Triangular plot
   ./plot -T -p /Users/mariannamiola/Desktop/work -x T -y Cr -z B


