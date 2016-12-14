function u = gradufun(x,y)
    f = [(y-(y.^2)./2)*(1-x), (x-(x.^2)./2) * (1-y)];
    u = f;
end
