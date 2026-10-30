function [y, n] = OSTROWSKI_acoc(f, df, x0, e)
    xn = x0;
    n = 0;
    max_iter = 50; 
    
    d = [];      
    ln_rn_table = [];   
    
    mprintf("\n--- Ostrowski Method ACOC Analysis ---\n");
    mprintf("%-3s | %-12s | %-12s | %-12s | %-12s | %-12s\n", ...
            "n", "x_n", "d_n", "r_n", "ln(r_n)", "p_n (ACOC)");
    mprintf("-----------------------------------------------------------------------------\n");
    
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
        d = [d, dn];
        
        
        if n == 0 then
            mprintf("%-3d | %-12.8f | %-12.4e | %-12s | %-12s | %-12s\n", ...
                    n, xn, dn, "-", "-", "-");
        elseif n == 1 then
            rn = d(n + 1) / d(n);
            ln_rn = log(rn);
            ln_rn_table = [ln_rn_table, ln_rn];
            
            mprintf("%-3d | %-12.8f | %-12.4e | %-12.4e | %-12.4f | %-12s\n", ...
                    n, xn, dn, rn, ln_rn, "-");
        else
            rn = d(n + 1) / d(n);
            ln_rn = log(rn);
            ln_rn_table = [ln_rn_table, ln_rn];
            pn = ln_rn_table(n) / ln_rn_table(n - 1);
            
            mprintf("%-3d | %-12.8f | %-12.4e | %-12.4e | %-12.4f | %-12.4f\n", ...
                    n, xn, dn, rn, ln_rn, pn);
        end
        
        n = n + 1;
        
        if dn <= e then
            mprintf("%-3d | %-12.8f | %-12.4e | %-12.4e | %-12.4f | %-12s\n", ...
                    n, xn, dn, rn, ln_rn, "-");
            mprintf("-----------------------------------------------------------------------------\n");
            y = xn_plus1;
            return;
        end
        
        xn = xn_plus1;
    end
    
    y = xn;
endfunction
