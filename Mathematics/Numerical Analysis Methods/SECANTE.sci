function [y, X] = SECANTE(r0, r1, fonc, n)
    xn_1 = r0; 
    xn = r1;  
    nb_iter = 0;
    X = [r0, r1];
    
    while nb_iter < n 
        if abs(fonc(xn) - fonc(xn_1)) < 1e-14 then
            break;
        end
        
        xn_plus1 = xn - ((xn - xn_1) / (fonc(xn) - fonc(xn_1))) * fonc(xn);
        
        nb_iter = nb_iter + 1;
        
        xn_1 = xn;
        xn = xn_plus1;
        X = [X, xn_plus1];
    end
    y = xn;
endfunction
