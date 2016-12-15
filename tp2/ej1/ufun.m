function u = ufun(x,y)
    f = (x-(x.^2)./2).*(y-(y.^2)./2);
    u = f;
end

