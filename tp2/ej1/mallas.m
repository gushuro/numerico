gen_mesh_rectangle(N, M, xmin, xmax, ymin, ymax)%genera triangulacion en el rectangulo
gen_mesh_L_shape(N) %genera triangulacion de la L
%genera cada uno archivos

%vertex_coordinates.txt coordenadas de los vertices.
%elem_vertices.txt  numeracion de los vertices por elemento.
%dirichlet.txt son todos los vertices de borde.

%Para graficar añ finañ usar

trimesh(elem_vertices, vertex_coordinates(:,1), vertex_coordinates(:,2), uh)
%%
%Usando el pde tool
g='squareg';%genera un cuadrado
[p,e,t]=initmesh(g,'Hmax',h);%genera una malla con maximo lado h
[p,e,t]=poimesh(g,n,m);%genera una malla uniforme de nXm.
pdeplot(p,e,t) %dibuja la malla.
%p vertices
%e vertices de los lados borde (fila 1 y 2). Con la fila 1 solo tenemos los vertices de borde.
%t numeracion de los elementos (primeras 3 filas)

%Si quiero ir refinando la malla puedo poner

[p,e,t]=refinemesh(g,p,e,t);% en general es tal que los lados de los triangulos son la mitad del anterior.

%Aca se puede usar

pdesurf(p,t,uh)