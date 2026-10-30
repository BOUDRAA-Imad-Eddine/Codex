function [y, n, X] = OSTROWSKI(f, df, x0, e)
    X = [x0];
    xn = x0;
    n = 0;
    max_iter = 50; 
    
    while n < max_iter
        if abs(df(xn)) == 0 then
            error('Derivative is zero');
        end
        yn = xn - f(xn) / df(xn);

        if abs(f(xn) - 2 * f(yn)) == 0 then
            error('Denominator is zero');
        end
        xn_plus1 = yn - (f(xn) / (f(xn) - 2 * f(yn))) * (f(yn) / df(xn));
        
        n = n + 1;
        X = [X, xn_plus1];
        
       
        if abs(xn_plus1 - xn) <= e then
            y = xn_plus1;
            return;
        end
        
        xn = xn_plus1;
    end
    
    y = xn;
endfunction
