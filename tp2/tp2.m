N=50;
M=50;
%mallado en el -1,1 X -1,1
gen_mesh_rectangle(N, M, -1, 1, -1, 1);
vertex_coordinates = dlmread('vertex_coordinates.txt');
elem_vertices = dlmread('elem_vertices.txt');
dirichlet = dlmread('dirichlet.txt');
cantNodos = size(vertex_coordinates,1);
cantTriang = size(elem_vertices,1);
A = zeros(cantNodos);
ref = [-1,-1;1,0;0,1];
f = @(x)(2*(2-x(1)^2 - x(2)^2));
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
    C = ref*Binv * Binv' * ref';
    med = abs(det(B)/2);
    M = med*C;
    G(1) = med/6 * (f(a12)+f(a13));
    G(2) = med/6 * (f(a12)+f(a23));
    G(3) = med/6 * (f(a13)+f(a23));
    
    A(elem_vertices(i,:), elem_vertices(i,:)) = A(elem_vertices(i,:), elem_vertices(i,:)) + M;
    F(elem_vertices(i,:)) = F(elem_vertices(i,:)) + G;
end

A;

for i = dirichlet'
    A(i,:) = zeros(cantNodos,1);
    A(i,i) = 1;
    F(i) = 0;
end

uh = A\F;
trimesh(elem_vertices, vertex_coordinates(:,1), vertex_coordinates(:,2), uh)

