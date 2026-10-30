function [y, n, X] = NEWTON(f, df, x0, e)
    X = [];
    n = 0;
    y = x0;
    stop = %f;
    while (~stop)
        n = n + 1;
        x = y;
        X = [X, x];
        if (abs(df(x)) == 0) then
            error('Derivative is zero.');
        end
        y = x - ( f(x) / df(x) );
        err = abs(y-x);
        if (err < e)
            stop = %t;
            X = [X, y];
        end
    end
endfunction
