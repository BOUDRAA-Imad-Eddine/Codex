clear;
clc;

getd("D:\Bibliotheque\ESI\ANUM\TP");

deff('y = f(x)', 'y = x^2 - 2'); 
deff('y = df(x)', 'y = 2*x');
x0 = 0.3;
x1 = 2.8;

deff('y = f2(x)', 'y = exp(x) - x - 2'); 
deff('y = df2(x)', 'y = exp(x) - 1'); 
x0_f2 = 2.0;
x1_f2 = 1.8;

e = 0.001;          
max_iter = 10;     

mprintf("=========================================================================\n");
mprintf(" PART 1: NUMERICAL METHODS RUNS (Via print_method_table) \n");
mprintf("=========================================================================\n");


[y_newton, n_newton, X_newton] = NEWTON(f, df, x0, e); 
[y_secant, X_secant] = SECANTE(x0, x1, f, max_iter); 
[y_ostr, n_ostr, X_ostr] = OSTROWSKI(f, df, x0, e);

print_method_table("NEWTON METHOD", X_newton, f);
print_method_table("SECANT METHOD", X_secant, f);
print_method_table("OSTROWSKI METHOD", X_ostr, f);


scf(0);
clf(0);
d_newton = abs(diff(X_newton)); 
d_secant = abs(diff(X_secant)); 
d_ostr = abs(diff(X_ostr));  

plot2d("nl", (0:length(d_newton)-1)', d_newton', style=2); 
plot2d("nl", (0:length(d_secant)-1)', d_secant', style=3);
plot2d("nl", (0:length(d_ostr)-1)', d_ostr', style=5); 
xtitle("Convergence Speed Comparison (Semi-Log Scale)", "Iteration Index (n)", "Step Size dn = |xn+1 - xn|"); 
legend(["Newton (p = 2)", "Secant (p = 1.618)", "Ostrowski (p = 4)"]); 
xgrid(1);

