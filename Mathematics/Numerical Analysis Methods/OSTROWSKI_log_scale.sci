function [y, n] = OSTROWSKI_log_scale(f, df, x0, e)
    xn = x0;
    n = 0;
    max_iter = 50; 

    n_table = [];
    log_dn_table = [];
    
    mprintf("\n--- Ostrowski Method Iterations ---\n");
    mprintf("%-5s | %-14s | %-14s | %-14s\n", "n", "x_n", "f(x_n)", "d_n = |x_n+1 - x_n|");
    mprintf("-------------------------------------------------------------\n");
    
    while n < max_iter
        if abs(df(xn)) == 0 then
            error('Derivative is zero');
        end
        
        yn = xn - f(xn) / df(xn);

        if abs(f(xn) - 2 * f(yn)) == 0 then
            error('Denominator is zero');
        end

        xn_plus1 = yn - (f(xn) / (f(xn) - 2 * f(yn))) * (f(yn) / df(xn));
        
        dn = abs(xn_plus1 - xn);

        n_table = [n_table, n];
        log_dn_table = [log_dn_table, log10(dn)];

        mprintf("%-5d | %-14.8f | %-14.8e | %-14.8e\n", n, xn, f(xn), dn);
        
        if dn <= e then
            n = n + 1;
            mprintf("%-5d | %-14.8f | %-14.8e | %-14s\n", n, xn_plus1, f(xn_plus1), "-");
            mprintf("-------------------------------------------------------------\n");
            y = xn_plus1;
            break; 
        end
        
        n = n + 1;
        xn = xn_plus1;
        y = xn;
    end
    
    
    clf();
    plot2d(n_table, log_dn_table, style=2);
    xtitle("Convergence Behavior of Ostrowski Method", "Iteration Index (n)", "log10(|x_{n+1} - x_n|)");
    xgrid(1);
endfunction
