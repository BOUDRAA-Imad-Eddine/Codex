function print_method_table(method_name, X, f_func) 
    mprintf("\n========================================================================================\n"); 
    mprintf(" NUMERICAL DATA TABLE (Parts 4.1 & 4.3): %s\n", method_name); 
    mprintf("========================================================================================\n");
    mprintf(" n \t xn \t\t f(xn) \t\t dn=|xn+1-xn| \t rn=dn/dn-1 \t ACOC (pn)\n"); 
    mprintf("----------------------------------------------------------------------------------------\n"); 
    N = length(X); 
    d = abs(diff(X));
    r = []; p = []; 
    for i = 1:length(d) 
        if i >= 2 then 
            r(i) = d(i) / d(i-1); 
        else 
            r(i) = %nan; 
        end
        if i >= 3 && ~isnan(r(i-1)) && r(i-1) > 0 && ~isnan(r(i)) && r(i) > 0 then 
            p(i) = log(r(i)) / log(r(i-1)); 
        else 
            p(i) = %nan; 
        end 
    end
    for k = 1:N
        fx = f_func(X(k)); 
        if k <= length(d) then 
            dk_str = msprintf("%.4e", d(k)); 
        else 
            dk_str = " -- ";
        end
        if k <= length(r) && ~isnan(r(k)) then
            rk_str = msprintf("%.4f", r(k)); 
        else 
            rk_str = " -- ";
        end 
        if k <= length(p) && ~isnan(p(k)) then 
            pk_str = msprintf("%.4f", p(k)); 
        else 
            pk_str = " -- "; 
        end 
        mprintf("%2d \t %.8f \t %.4e \t %s \t %s \t %s\n", k-1, X(k), fx, dk_str, rk_str, pk_str); 
    end 
endfunction
