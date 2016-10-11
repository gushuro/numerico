function Uref = adi(deltat, n)
    deltat = deltat;
    Nx = n;
    Ny = n;
    deltax = 2.0/(Nx-1);
    deltay = 2.0/(Ny-1);
    Tf = 0.5;
    U = eye(Nx-2);
    f = ones(n-2,n-2);
    xgrid = deltax:deltax:(2-deltax);
    ygrid = deltay:deltay:(2-deltay);
    [X,Y] = meshgrid(xgrid,ygrid);
    %inicializo U_0;
    for i = 1:(Nx-2)
        for j = 1:(Ny-2)
            U(i,j) = exp(-100*((i*deltax-1)^2 + (j*deltay-1)^2));
        end
    end

    c = [-2,1,zeros(1, Nx-4)];
    d = [0,-1,zeros(1, Nx-4)];
    d2 =[0,1, zeros(1, Nx-4)];
    Dx = toeplitz(c,c)/(deltax^2) + (5/(2*deltax)) * toeplitz(d,d2);   %operador discretizacion en x
    Dy = toeplitz(c,c)/(deltay^2) + (5/(2*deltay)) * toeplitz(d,d2);   %operador discretizacion en y
    sparse(Dx);
    sparse(Dy);
    %surf(X,Y,U);
    %shading interp;
    %view(0,50);
    %pause(1)
    for i = 1:(0.5/deltat)
        Us = inv(eye(n-2)- (1/2 * deltat)*Dx)*((deltat/2)*(U*Dy'+f) + U);
        %x = i
        U = (Us+ (1/2 * deltat)*(Dx*Us +f))*inv(eye(n-2) - (deltat/2)* Dy');
        %surf(X,Y,U);
        %shading interp;
        %view(0,50);
        %pause(0.001)
    end
    Uref = U;


