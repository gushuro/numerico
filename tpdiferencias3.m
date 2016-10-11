Uref = adi(1/10000,30);
deltast = [1/100, 1/200, 1/300, 1/400, 1/500];
y = zeros(size(deltast,1));
el = 1
for i = deltast
    U = adi(i, 30);
    y(el) = max(max(abs(Uref-U)));
    el = el+1
end
plot(log(deltast), log(y), '.');

orden_empirico = log(y(2)/y(1)) / log(deltast(2)/deltast(1))