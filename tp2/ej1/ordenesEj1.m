function a = ordenesEj1()
    deltast = [1/10, 1/20, 1/30, 1/40, 1/50, 1/60, 1/70, 1/80];
    L2 = zeros(size(deltast,1));
    H1 = zeros(size(deltast,1));
    
    for i = 1:size(deltast,2)
        [L2(i), H1(i)] = ej1(1/deltast(i), 0, 0)
        i
    end
    plot(log10(deltast), log10(L2), '--', log10(deltast),log10(H1), '.');

    
    orden_L2 = log(L2(8)/L2(7)) / log(deltast(8)/deltast(7))
    orden_H1 = log(H1(8)/H1(7)) / log(deltast(8)/deltast(7))
    
end