%% Elementos finitos para el problema.
N=20;
M=20;
%mallado en el -1,1 X -1,1
gen_mesh_rectangle(N, M, -1, 1, -1, 1);
vertex_coordinates = dlmread('vertex_coordinates.txt');
elem_vertices = dlmread('elem_vertices.txt');
dirichlet = dlmread('dirichlet.txt');
cantNodos = size(vertex_coordinates,1);
cantTriang = size(elem_vertices,1);
A = zeros(cantNodos);
ref = [-1,-1;1,0;0,1];
%f = @(x)(2*(2-x(1)^2 - x(2)^2));
f = @(x)(x(1)-(x(1)^2)/2 + x(2)-(x(2)^2)/2); % + (x(1)-(x(1)^2)/2)*x(2)-(x(2)^2)/2);
g1 = @(x)(-3/2*(x(2) - (x(2)^2)/2));
g2 = @(x)(-3/2*(x(1) - (x(1)^2)/2));

inversasB = zeros(2, cantTriang*2);

F = zeros(cantNodos,1);
G = zeros(3,1);
for i = 1:cantTriang
    v1 = vertex_coordinates(elem_vertices(i,1),:);
    v2 = vertex_coordinates(elem_vertices(i,2),:);
    v3 = vertex_coordinates(elem_vertices(i,3),:);
    a12 = (v1+v2)/2;
    a13 = (v1+v3)/2;
    a23 = (v2+v3)/2;
    B = [(v2-v1)', (v3-v1)'];
    Binv = inv(B);
    inversasB(:,[2*i-1,2*i]) = Binv;
    C = ref*(Binv * Binv') * ref';
    med = abs(det(B)/2);
    Ma = med*C;
    G(1) = med/6 * (f(a12)+f(a13));
    G(2) = med/6 * (f(a12)+f(a23));
    G(3) = med/6 * (f(a13)+f(a23));
    
    A(elem_vertices(i,:), elem_vertices(i,:)) = A(elem_vertices(i,:), elem_vertices(i,:)) + Ma;
    F(elem_vertices(i,:)) = F(elem_vertices(i,:)) + G;
end

% Acá fijamos las condiciones de borde:
for i = dirichlet'
    elem = vertex_coordinates(i,:);
    if elem(1) == -1
        A(i,:) = zeros(cantNodos,1);
        A(i,i) = 1;
        F(i) = g1(elem);
    end
    if elem(2) == -1
        A(i,:) = zeros(cantNodos,1);
        A(i,i) = 1;
        F(i) = g2(elem);
    end
end

uh = A\F;
trimesh(elem_vertices, vertex_coordinates(:,1), vertex_coordinates(:,2), uh)
%% Para armar matriz y surfear.
hold on;

deltax = 1/N;
deltay = 1/M;
xgrid = (-1+deltax):deltax:(1-deltax);
ygrid = (-1+deltay):deltay:(1-deltay);
[X,Y] = meshgrid(xgrid,ygrid);
Umat = (X-(X.^2)/2).*(Y-(Y.^2)/2);
surf(X,Y,Umat);


%surf(X,Y,Umat-uh);
%% Con esto graficamos el error! 
UpostaVector = zeros(size(uh,1),1);
for i = 1:size(uh,1)
    elem = vertex_coordinates(i,:)
    UpostaVector(i,1) = ufun(elem(1), elem(2));
end
% Para graficar la u posta en la triangulación
%trimesh(elem_vertices, vertex_coordinates(:,1), vertex_coordinates(:,2), UpostaVector)


%Para graficar el error en la triangulación
trimesh(elem_vertices, vertex_coordinates(:,1), vertex_coordinates(:,2), uh -UpostaVector)

grad = zeros(cantNodos,2);

errorL2 = 0
errorH1 = 0
for i = 1:cantTriang
    numv1 = elem_vertices(i,1);
    numv2 = elem_vertices(i,2);
    numv3 = elem_vertices(i,3);
    v1 = vertex_coordinates(numv1,:);
    v2 = vertex_coordinates(numv2,:);
    v3 = vertex_coordinates(numv3,:);
    med = abs(det(B)/2);
    
    grad(numv1,:) = grad(numv1,:) + uh(numv1) * ([-1,-1]*inversasB(:,[2*i-1,2*i]));
    grad(numv2,:) = grad(numv2,:) + uh(numv2) * ([1,0]*inversasB(:,[2*i-1,2*i]));
    grad(numv3,:) = grad(numv3,:) + uh(numv3) * ([0,1]*inversasB(:,[2*i-1,2*i]));
    baricentro = (v1+v2+v3)/3;
    aprox = uh(numv1)+ uh(numv2) + uh(numv3);
    errorL2 = errorL2 + (ufun(baricentro(1),baricentro(2)) - aprox/3 )^2;
end
errorH1 = errorL2;

for i = 1:cantNodos
    e = grad(i,:)-gradufun(vertex_coordinates(i,1), vertex_coordinates(i,2))
    errorH1 = errorH1 + (e(1)^2 + e(2)^2);
    
end
errorL2
errorH1 = sqrt(errorH1)
    
    
    