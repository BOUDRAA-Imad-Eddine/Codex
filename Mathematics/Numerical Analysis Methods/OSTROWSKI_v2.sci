function [y, n] = OSTROWSKI_v2(f, df, x0, e)
    xn = x0;
    n = 0;
    max_iter = 50; 
    
    // Print Table Header
    mprintf("\n--- Ostrowski Method Iterations ---\n");
    mprintf("%-5s | %-14s | %-14s | %-14s\n", "n", "x_n", "f(x_n)", "d_n = |x_n+1 - x_n|");
    mprintf("-------------------------------------------------------------\n");
    
    while n < max_iter
        if abs(df(xn)) == 0 then
            error('Derivative is zero');
        end
        
        // Step 1: Predictor step (Newton step)
        yn = xn - f(xn) / df(xn);

        if abs(f(xn) - 2 * f(yn)) == 0 then
            error('Denominator is zero');
        end
        
        // Step 2: Corrector step
        xn_plus1 = yn - (f(xn) / (f(xn) - 2 * f(yn))) * (f(yn) / df(xn));
        
        // Step size (increment)
        dn = abs(xn_plus1 - xn);
        
        // Print row for iteration n
        mprintf("%-5d | %-14.8f | %-14.8e | %-14.8e\n", n, xn, f(xn), dn);
        
        n = n + 1;
        
        // Stopping criterion based on increment size
        if dn <= e then
            // Print final iterate row before exiting
            mprintf("%-5d | %-14.8f | %-14.8e | %-14s\n", n, xn_plus1, f(xn_plus1), "-");
            mprintf("-------------------------------------------------------------\n");
            y = xn_plus1;
            return;
        end
        
        xn = xn_plus1;
    end
    
    y = xn;
endfunction
