#include <iostream>
#include <istream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <vector>
#include <stdlib.h>
#include <iomanip>
#include <time.h>
#include <string.h>

using namespace std;

#define COUT if (0) cout

ifstream ArchivoEntrada;

typedef vector < vector < double > > matriz; // CAMBIAR A INT ACA Y EN MAIN PARA KNN SIN REDUCCION Y COMENTAR TODO EL CODIGO QUE NO TENGA QUE VER CON EL METODO 0 (FUNCIONA MUCHO MAS RAPIDO), DOUBLE NECESARIO PARA METODOS DE REDUCCION

void imagenes_A_Vectores(matriz& a, matriz& b, ifstream& TestEntrada, int NoHayTest, int o, string RutaImgs) // capaz a "o" la llamaría "indice"
{
	COUT << "PASANDO IMAGENES A VECTORES " << o + 1 << endl;
	COUT << endl;

	string RutaImgsEntrenamiento;
	RutaImgsEntrenamiento.append(RutaImgs+"train.csv");
	ArchivoEntrada.open(RutaImgsEntrenamiento.c_str());
	int v = 0;
	int i = 0;
	int w = 0;
	int j, h, t;
	char m;
	const int CANT_IMGS_ENTRENAMIENTO = 42000; // leí que es buena práctica poner a las constantes en mayúsculas
	const int CANT_PIXELS_EN_IMG = 784; // 28*28

	while(v < CANT_IMGS_ENTRENAMIENTO) // este número capaz lo haría una constante en vez de hardcodearlo // Una variable decis? Es la idea, pero no se como. // creo que como puse arriba
	{
		j = 0;
		TestEntrada >> t;

		//cout << "t vale: " << t << endl;

		if(t == 1 || NoHayTest == 1) // si "NoHayTest" está activado, no se particiona "train"
		{
			a.resize(i + 1);
			a[i].resize(785);

		}else
		{
			b.resize(w + 1);
			b[w].resize(785);
		}

		while(j<785)
		{	
			if(j == 784)
			{
				ArchivoEntrada >> h;
			}else
			{
				ArchivoEntrada >> h >> m;
			}

			if(t == 1 || NoHayTest == 1)
			{			
				a[i][j] = h;
				j++;

			}else
			{
				b[w][j] = h;
				j++;
			}		
		}

		if(t == 1 || NoHayTest == 1)
		{
			i++;

		}else
		{

			w++;
		}

		v++;
	}
	//cout << "b[0][0]: " << b[0][0] << endl;
	//cout << "dimension de b = " << b.size() << " por " << b[0].size() << endl;
	//cout << "dimension de a = " << a.size() << " por " << a[0].size() << endl;
     
	 ArchivoEntrada.close();	 
}

vector<pair<int,double> > ordenarPrimeraskDistancias(vector<pair<int,double> >& distancias, int k)
{
	vector<pair<int,double> > k_vecinos;
	int min,j;
	vector<pair<int,double> > aux;
	int i;
	k_vecinos.resize(k);
	aux.resize(1);

    for(i = 0; i < k; i++)
    {
        min = i;

        for(j = i + 1; j < distancias.size(); j++)
        {	
            if(distancias[min].second > distancias[j].second)
                min = j;
                aux[0] = make_pair(distancias[min].first,distancias[min].second);
                distancias[min] = distancias[i];
                distancias[i] = aux[0] ;
        }
        k_vecinos[i] = distancias[i];        
    }  

    return k_vecinos;
}

int vecinoGanador(vector<pair<int,double> >& k_vecinos, int f)// f es el numero de imagen
{
	int i;
	int j = 0;
	int cantRepeticiones;
	int sonIguales = -1;
	int mayoriaAbsoluta = 1;
	int respuesta = -1;

	while(j < k_vecinos.size())
	{
		i = j + 1;
		cantRepeticiones = 1;

		while(i < k_vecinos.size())
		{
			if(k_vecinos[j].first == k_vecinos[i].first)
			{
				cantRepeticiones++;
			}

			i++;
		}

		if(cantRepeticiones > mayoriaAbsoluta)
		{
			mayoriaAbsoluta = cantRepeticiones;
			respuesta = k_vecinos[j].first;

		}else
		{
			if(cantRepeticiones == mayoriaAbsoluta)
			{
				sonIguales = mayoriaAbsoluta;
			}
		}

		j++;
	}	

	if(mayoriaAbsoluta == sonIguales)
	{
		COUT << "NO HAY VECINO DOMINANTE PARA LA IMAGEN NUMERO " << f << endl;
	}

	return respuesta;
}

void mostrarVectorOrdenado(vector<pair<int,double> >& distancias)
{
	int i = 0;

	while(i < distancias.size())
	{
		cout << "Vector de distancias[" << i << "] = " << distancias[i].first << " , " << distancias[i].second << endl;
		i++;
	}
}

void mostrarVector(vector<double>& a)
{
	int i = 0;

	while(i < a.size())
	{
		cout << "vector[" << i << "] = " << a[i] << endl;
		i++;
	}
}

vector<int> Knn(matriz& ImagenesEntrenamiento, matriz& ImagenesTest, int k, int alfaOgamma, int metodo)
{
	COUT << "REALIZANDO Knn " << endl;
	COUT << endl;

	vector<pair<int,double> > distancias;
	vector<pair<int,double> > k_vecinos;
	vector<int> respuestas;
	int f = 0;
	int i, j, distanciaCoordendas; 
	double distanciaImagen;

	alfaOgamma = alfaOgamma + 1;

	respuestas.resize(ImagenesTest.size());
	distancias.resize(ImagenesEntrenamiento.size());
	//cout << "tamaño imagenes entrenamiento: " << ImagenesEntrenamiento.size() << endl;

	if(metodo == 0)
	{
		alfaOgamma = ImagenesEntrenamiento[0].size();
	}

	while(f < ImagenesTest.size())
	{
		i = 0;

		while(i < ImagenesEntrenamiento.size())
		{	
			j = 1;
			distanciaImagen = 0;
			distanciaCoordendas = 0;

			while(j < alfaOgamma)
			{
				//cout << "i vale: " << i << " , j vale: " << j << " , f vale: " << f << endl;
				distanciaCoordendas = distanciaCoordendas + ((ImagenesEntrenamiento[i][j] - ImagenesTest[f][j])*(ImagenesEntrenamiento[i][j] - ImagenesTest[f][j]));
				j++;
			}
			
			distanciaImagen = sqrt(distanciaCoordendas);
			distancias[i] = (make_pair(ImagenesEntrenamiento[i][0],distanciaImagen));
			i++;
		}

		//cout << "dimension de vector distancias: " << distancias.size() << endl;
		
		k_vecinos = ordenarPrimeraskDistancias(distancias, k);
		//mostrarVectorOrdenado(k_vecinos);
		respuestas[f] = vecinoGanador(k_vecinos, f);
		//cout << "respuesta[" << f << "] = " << respuestas[f] << endl;

		//cout << "respuesta: " << respuestas[f] << endl;

		f++;
	}	

	//mostrarVector(respuestas);

	return respuestas;
}
float Precision(matriz& ImagenesTest, vector<int>& resultados, int j, FILE* ArchivoSalidaReporte)
{

	float precision = 0;
	int i = 0;
    vector<float> tpi;
    for (int h=0; h<10; ++h)
   	{
    		tpi.push_back(0.0);
    }
    vector<float> fpi;
    for (int k=0; k<10; ++k)
   	{
    		fpi.push_back(0.0);
    }
    vector<float> precClases;
    for (int k=0; k<10; ++k)
   	{
    		precClases.push_back(0.0);
    }
	//cout << endl;
	//cout << "TEST " << j+1 << endl;



	while(i < resultados.size())
	{
		 for (int m=0; m<10; ++m)
   		{
	    	if( m == resultados[i] )
			{
				if (ImagenesTest[i][0] == m)
				{
					//cout << "ENTRA A TPI " << endl;
					tpi[m] = tpi[m] + 1.0;

				}
				else
				{
					//cout << "ENTRA A FPI " << endl;
					fpi[m] = fpi[m] + 1.0;
				}
			}
    	}

		i++;
	}
	float divisor;
	for (int i=0; i<10; ++i)
   	{
   			//cout << "FPI " << fpi[i] << endl;
   			//cout << "TPI " << tpi[i] << endl;
   			float t = tpi[i];
   			float f = fpi[i];
   			divisor = t+f;
			//cout << "t " << t << endl;
			//cout << "f " << f << endl;
			 

   			float pre = t/(t+f);
   			//cout << "precison " << pre << endl;

   			divisor = tpi[i]+fpi[i];
   			//cout << "DIVISOR " << divisor << endl;

    		precClases[i] = pre;
    		//cout << "prec clase i " << precClases[i] << endl;
    		fprintf(ArchivoSalidaReporte, "Precision clase %d :  %4.6f\n", i, precClases[i]);
    		//cout << "Precision clase  " << i << " :" << precClases[i] << endl;
    }
    for (int i=0; i<10; ++i)
   	{
    		precision = precision + precClases[i];
    }
    precision = precision/10;



	return precision;
}
float Recall(matriz& ImagenesTest, vector<int>& resultados, int j, FILE* ArchivoSalidaReporte)
{

	float recall = 0;
	int i = 0;
    vector<float> tpi;
    for (int h=0; h<10; ++h)
   	{
    		tpi.push_back(0.0);
    }
    vector<float> fni;
    for (int k=0; k<10; ++k)
   	{
    		fni.push_back(0.0);
    }
    vector<float> recallClases;
    for (int k=0; k<10; ++k)
   	{
    		recallClases.push_back(0.0);
    }
	//cout << endl;
	//cout << "TEST " << j+1 << endl;



	while(i < resultados.size())
	{
		 for (int m=0; m<10; ++m)
   		{
	    	if( m == resultados[i] )
			{
				if (ImagenesTest[i][0] == m)
				{
					//cout << "ENTRA A TPI " << endl;
					tpi[m] = tpi[m] + 1.0;

				}

			}
			if ((ImagenesTest[i][0] == m && ImagenesTest[i][0] != resultados[i] ))
			{
				fni[m] = fni[m] + 1.0;
			}

    	}


		i++;
	}
	float divisor;
	for (int i=0; i<10; ++i)
   	{
   			//cout << "FNI " << fni[i] << endl;
   			//cout << "TPI " << tpi[i] << endl;
   			float t = tpi[i];
   			float f = fni[i];
   			divisor = t+f;
			//cout << "t " << t << endl;
			//cout << "f " << f << endl;
			 

   			float pre = t/(t+f);
   			//cout << "precison " << pre << endl;

   			divisor = tpi[i]+fni[i];
   			//cout << "DIVISOR " << divisor << endl;

    		recallClases[i] = pre;
    		//cout << "recall clase i " << recallClases[i] << endl;
    		fprintf(ArchivoSalidaReporte, "Recall clase %d :  %4.6f\n", i, recallClases[i]);
    		//cout << "Recall clase  " << i << " :" << recallClases[i] << endl;
    }
    for (int i=0; i<10; ++i)
   	{
    		recall = recall + recallClases[i];
    }
    recall = recall/10;



	return recall;
}
float resultadosCorrectos(matriz& ImagenesTest, vector<int>& resultados, int j, FILE* ArchivoSalidaReporte)
{
	float resultadosCorrectos = 0;
	float resultadosIncorrectos = 0;
	float porcentajeAciertos;
	int i = 0;

	cout << endl;
	cout << "TEST " << j+1 << endl;

	if(!(resultados.size() == ImagenesTest.size()))
	{
		cout << "ALGO ANDA MAL" << endl;
	}

	while(i < resultados.size())
	{
		if(ImagenesTest[i][0] == resultados[i])
		{
			resultadosCorrectos++;
		}else
		{
			resultadosIncorrectos++;
		}

		i++;
	}

	if(!(resultadosIncorrectos + resultadosCorrectos == resultados.size()))
	{
		cout << "ALGO ANDA MAL" << endl;
	}

	porcentajeAciertos = (resultadosCorrectos/resultados.size())*100;


	// acá se muestra por pantalla
	cout << "Resultados correctos: " << resultadosCorrectos << endl;
	cout << "Resultados incorrectos: " << resultadosIncorrectos << endl;
	cout.setf(ios::fixed);
	cout <<"Porcentaje de aciertos: " << setprecision(3) << porcentajeAciertos << endl;
	cout << setprecision(0) << endl;
	cout << endl;

	// acá se escribe en un archivo
	fprintf(ArchivoSalidaReporte, "Resultados correctos: %4.6f\n", resultadosCorrectos);
	fprintf(ArchivoSalidaReporte, "Resultados incorrectos: %4.6f\n", resultadosIncorrectos);
	fprintf(ArchivoSalidaReporte, "Porcentaje de aciertos: %4.6f\n", porcentajeAciertos);
	return porcentajeAciertos;

}


// de acá para abajo comentás si querés hacer solo Knn usando matriz de ints


vector <double> calculoVectorMedias(matriz& ImagenesEntrenamiento)
{
	COUT << "CALCULANDO VECTOR DE MEDIAS" << endl;
	COUT << endl;

	vector <double> media;
	int i = 0;
	int j = 1;
	media.resize(ImagenesEntrenamiento[0].size());

	while(j < ImagenesEntrenamiento[0].size())
	{
		i = 0;
		media[j - 1] = 0;

		while(i < ImagenesEntrenamiento.size())
		{
			media[j - 1] = (media[j - 1] + ImagenesEntrenamiento[i][j]);
			i++;
		}

		media[j - 1] = media[j - 1]/(ImagenesEntrenamiento.size());
		j++;
	}

	return media;
}

matriz armarMatrizX(matriz& Imagenes, vector<double>& media, int n)// n es la cantidad de imagenes de entrenamiento
{
	matriz b;
	b.resize(Imagenes.size());

	int i = 0;
	int j = 0;

	while(i < b.size())
	{
		b[i].resize(Imagenes[i].size() - 1);
		i++;
	}

	i = 0;

	while(j < Imagenes[0].size())
	{
		i = 0;

		while(i < Imagenes.size())
		{
			b[i][j] = (Imagenes[i][j + 1] - media[j])/(sqrt(n - 1));
			i++;
		}

		j++;
	}

	return b;
}

matriz armarMatrizY(matriz& Imagenes, vector<double>& media, int n)// n es la cantidad de imagenes de entrenamiento
{
	int i = 0;
	int j = 0;

	while(j < Imagenes[0].size())
	{
		i = 0;

		while(i < Imagenes.size())
		{
			Imagenes[i][j] = (Imagenes[i][j] - media[j])/(sqrt(n - 1));
			i++;
		}

		j++;
	}

	return Imagenes;
}

void centrar(matriz& Imagenes, vector<double>& media, int n)
{
	int i = 0;
	int j = 1;
	while(j < Imagenes[0].size())
	{
		i = 0;

		while(i < Imagenes.size())
		{
			Imagenes[i][j] = (Imagenes[i][j] - media[j - 1])/(sqrt(n - 1));
			i++;
			
		}
		j++;
	}
}

matriz Trasponer(matriz& a)
{

	matriz Traspuesta;
	int i = 0;
	int j;

	Traspuesta.resize(a[0].size());

	while(i < a[0].size())
	{
		Traspuesta[i].resize(a.size());
		j = 0;

		while(j < a.size())
		{
			Traspuesta[i][j] = a[j][i];
			j++;
		}	
		i++;

	}


	return Traspuesta;
}

vector<double> matrizPorVector(matriz& A, vector<double>& x)
{
	vector<double> solucion;
	int i = 0;
	int j;
	solucion.resize(A.size());

	while(i < A.size())
	{
		j = 0;
		solucion[i] = 0;

		while(j < A[i].size())
		{
			solucion[i] = solucion[i] + A[i][j] * x[j];
			j++;
		}

		i++;
	}

	return solucion;
}

matriz matrizCovarianza(matriz ImagenesEntrenamiento, vector<double>& media)
{
	COUT << "CALCULANDO MATRIZ DE COVARIANZAS" << endl << endl;

	matriz matrizCovarianza, matrizX, Traspuesta;

	media = calculoVectorMedias(ImagenesEntrenamiento); // promedio (de cada dimension), μ

	COUT << "ARMANDO MATRIZ X" << endl;
	COUT << endl;

	matrizX = armarMatrizX(ImagenesEntrenamiento, media, ImagenesEntrenamiento.size()); // Matriz ImagenesEntrenamiento donde a cada coordenada se le resta μ
	Traspuesta = Trasponer(matrizX);

	int i = 0;
	int j; 

	COUT << "CALCULANDO MATRIZ DE COVARIANZAS COORDENADA A COORDENADA" << endl;
	COUT << endl;

	matrizCovarianza.resize(Traspuesta.size());

	while(i < Traspuesta.size())//La matriz resultante esta traspuesta pero no importa porque es simetrica
	{
		matrizCovarianza[i].resize(Traspuesta.size());
		matrizCovarianza[i] = matrizPorVector(Traspuesta, Traspuesta[i]);
		i++;
		COUT << i << " de 784 vectores terminados" << endl;
	}

	i = 0;

	while(i < matrizCovarianza.size())
	{
		j = 0;

		while(j < matrizCovarianza.size())
		{
			matrizCovarianza[i][j] = matrizCovarianza[i][j];//(Traspuesta[i].size() - 1);
			j++;
		}

		i++;
	}

	COUT << endl; 

	return matrizCovarianza;
}

void testCovarianzaSimetrica(matriz& a) // por qué "a" y no "m"? :P
{
	cout << "TESTEANDO SIMETRIA DE MATRIZ DE COVARIANZAS" << endl << endl;

	int i = 0; // indice_filas
	int j; // indice_columnas
	int contador;

	if (a.size() != a[i].size()) {cout << "ERROR, LA MATRIZ DE COVARIANZAS NO ES CUADRADA" << endl;}

	while(i < a.size())
	{
		j = i + 1;

		while(j < a.size())
		{
			//cout << "a[" << i << "][" << j << "] vale: " << a[i][j] << endl;
			if(a[i][j] == 0)
			{
				contador++;
			}

			if(a[i][j] != a[j][i])
			{
				cout << "ERROR, LA MATRIZ DE COVARIANZAS NO ES SIMETRICA" << endl;
				break;
			}else{j++;}
		}

		if((j != a.size()) && (a[i][j] != a[j][i])){break;}
		else{i++;}
	}

	if(contador == a.size() * a.size())
	{
		cout << "LA MATRIZ DE COVARIANZAS ES NULA" << endl << endl;
	}

	cout << "BIEN! LA MATRIZ DE COVARIANZAS ES SIMETRICA!" << endl;
	cout << endl;
}

double norma(vector <double>& a)
{
	double norma_a = 0;
	int i = 0;

	while(i < a.size())
	{
		norma_a = norma_a + a[i]*a[i];
		i++;
	}

	norma_a = sqrt(norma_a);

	return norma_a;
}

matriz deflacion(matriz& matrizCovarianzas, double& autovalor, vector<double>& autovector)
{
	int i = 0;
	int j;

	while(i < matrizCovarianzas.size())
	{
		j = 0;

		while(j < matrizCovarianzas.size())
		{
			matrizCovarianzas[i][j] = matrizCovarianzas[i][j] - autovalor * autovector[i] * autovector[j];
			j++;
		}

		i++;
	}

	return matrizCovarianzas;
}

vector<double> normalizoX(vector<double>& x)
{
	int i = 0;
	double norm;

	norm = norma(x);

	while(i < x.size())
	{
		x[i] = x[i]/norm;
		i++;
	}

	return x;
} 

vector<double> metodoDeLaPotencia(matriz& matrizCovarianzas, int alfa, matriz& autovectoresTraspuestos) // devuelve un vector<double> c/los avals. vamos a necesitar los avects también, para hacer el cambio de base de los datos. ¿modifica matrizCovarianzas poniendo avectores en sus cols? -- franco
{

	COUT << "CALCULANDO AUTOVALORES Y AUTOVECTORES" << endl;
	COUT << endl;

	vector<double> autovalores, x, K_MasUno; // X en la iteracion k + 1. Una vez que se tiene X = autovector, al multiplicarlo una vez mas por A tenemos un vector multiplo, a partir de ese vector y del anterior se obtiene lambda. // perdón, todavía no entiendo de qué la juega "K_MasUno" :(
	srand(time(NULL)); // para que los numeros random no sean siempre los mismos. Con esto van a depender de la hora del reloj de la computadora
	int k;
	double y, z;
	int i = 0;
	int infinito = 3000; // ACA HAY QUE TESTEAR CON QUE NUMERO ES SUFICIENTE // capaz le pondría "pseudoinfinito" en vez de "infinito" -- fp

	autovalores.resize(alfa);
	autovectoresTraspuestos.resize(alfa);
	x.resize(matrizCovarianzas.size());

	while(i < alfa)
	{

		for(int u = 0; u < x.size(); u++) //GENERAR UN VECTOR X DE TAMAÑO MATRIZCOVARIANZAS.SIZE() Y VALORES RANDOM 
		{
			x[u] = 0 + rand();
		}

		x = normalizoX(x);

		autovectoresTraspuestos[i].resize(x.size());
		autovalores[i] = 0;
		k = 0;
	
		while(k < infinito)
		{
				x = matrizPorVector(matrizCovarianzas, x);  
				x = normalizoX(x);
				k++;
		}

		K_MasUno = matrizPorVector(matrizCovarianzas,x);

		autovalores[i] = norma(K_MasUno);
		autovectoresTraspuestos[i] = x;
		if(alfa > 1)matrizCovarianzas = deflacion(matrizCovarianzas, autovalores[i], autovectoresTraspuestos[i]);
		i++;
	}	
	
	return autovalores;
}

vector<double> transformacionCaracteristica(vector<double>& Imagen_i, matriz& autovectoresTraspuestos)
{
	vector<double> solucion;
	int i = 1;
	int j;
	solucion.resize(autovectoresTraspuestos.size() + 1);//La primera coordenada es la etiqueta
	solucion[0] = Imagen_i[0];
	while(i < solucion.size())
	{
		j = 1;
		solucion[i] = 0;
		while(j < Imagen_i.size())
		{
			solucion[i] = solucion[i] + autovectoresTraspuestos[i - 1][j - 1] * Imagen_i[j];
			j++;
		}
		i++;
	}
	return solucion;
}

void escribirMatrizEnArchivo(matriz& m, FILE* archivo){
    int i = 0;
    int j;

    while(i < m.size())
    {
    	j = 0;

    	while(j < m[i].size())
    	{
  
    		if(j == m[i].size() - 1)
    		{
    			fprintf(archivo, "%.0f", m[i][j]);	
    		}else
			{
				fprintf(archivo, "%.0f,", m[i][j]);
			}
	      
	    	j++;
    	}

    	fprintf(archivo, "\n");
    	i++;
    }
}

matriz generarMatrizPreY(matriz& ImagenesEntrenamiento){

	matriz PreY;
	PreY.resize(ImagenesEntrenamiento.size());
	int i = 0;
	int j;
	while(i < PreY.size())
	{
		PreY[i].resize(10);
		j = 0;

		while(j < 10)
		{
			if (ImagenesEntrenamiento[i][0] == j)
			{
				PreY[i][j] = 1;
			}
			else
			{
				PreY[i][j] = -1;
			}

			j++;
		}

		i++;
	}

	return PreY;
}

void generarMatriz_Mi(matriz& matrizX, matriz& matrizY, matriz& matriz_Mi)
{
	matriz TraspuestaX, TraspuestaY, aux, aux2;

	TraspuestaX = Trasponer(matrizX);
	TraspuestaY = Trasponer(matrizY);

	int i = 0;

	aux.resize(TraspuestaY.size());

	while(i < TraspuestaY.size()) // X traspuesta por Y. El resultado queda traspuesto
	{
		aux[i].resize(TraspuestaX.size());
		aux[i] = matrizPorVector(TraspuestaX, TraspuestaY[i]);
		i++;
	}

	aux = Trasponer(aux);

	i = 0;

	aux2.resize(TraspuestaX.size());

	while(i < TraspuestaX.size())
	{
		aux2[i].resize(TraspuestaY.size());
		aux2[i] = matrizPorVector(TraspuestaY, TraspuestaX[i]);
		i++;
	}

	i = 0;

	matriz_Mi.resize(aux2.size());

	while(i < aux2.size())
	{
		matriz_Mi[i].resize(aux.size());
		matriz_Mi[i] = matrizPorVector(aux, aux2[i]);
		i++;
	}

	matriz_Mi = Trasponer(matriz_Mi);

	/*aux = Trasponer(aux); // Lo traspongo para que quede bien
	aux2.resize(matrizY.size());

	while(i < matrizY.size()) // Lo de antes por Y traspuesta. Es decir X traspuesta por Y por Y traspuesta
	{
		aux2[i].resize(aux.size());
		aux2[i] = matrizPorVector(aux, matrizY[i]);
		i++;
	}

	i = 0;

	aux2 = Trasponer(aux2);
	matriz_Mi.resize(TraspuestaX.size());

	while(i < TraspuestaX.size()) // Lo de antes por X
	{
		matriz_Mi[i].resize(aux2.size());
		matriz_Mi[i] = matrizPorVector(aux2, TraspuestaX[i]);
		i++;
	}

	matriz_Mi = Trasponer(matriz_Mi);*/

}

matriz actualizoMatriz(matriz& a, vector<double>& Ti)
{
	matriz aTraspuesta;
	vector<double> b;
	int i = 0;
	int j;
	aTraspuesta = Trasponer(a);
	b = matrizPorVector(aTraspuesta, Ti);

	while(i < a.size())
	{
		j = 0;

		while(j < a[i].size())
		{
			a[i][j] = a[i][j] - Ti[i] * b[j];
			j++;
		}

		i++;
	}

	return a;
}

vector<double> PLS_DA(matriz& ImagenesEntrenamiento, vector<double>& media, int Gamma, matriz& autovectoresTraspuestos)
{
	matriz matrizY, matrizX, matriz_Mi, aux2;
	vector<double> Ti, aux, autovalores;
	autovalores.resize(Gamma);
	autovectoresTraspuestos.resize(Gamma);

	media = calculoVectorMedias(ImagenesEntrenamiento);

	matrizY = generarMatrizPreY(ImagenesEntrenamiento);
	matrizY = armarMatrizY(matrizY, media, ImagenesEntrenamiento.size());
	matrizX = armarMatrizX(ImagenesEntrenamiento, media, ImagenesEntrenamiento.size());

	int i = 0;

	while(i < Gamma)
	{
		COUT << "GENERANDO MATRIZ Mi: " << i + 1 << " DE " << Gamma << endl << endl;

		generarMatriz_Mi(matrizX, matrizY, matriz_Mi);
		aux = metodoDeLaPotencia(matriz_Mi, 1, aux2);
		Ti = matrizPorVector(matrizX, aux2[0]);
		normalizoX(Ti);
		matrizX = actualizoMatriz(matrizX, Ti);
		matrizY = actualizoMatriz(matrizY, Ti);
		autovalores[i] = aux[0];
		autovectoresTraspuestos[i] = aux2[0];
		i++;
	}

	return autovalores;
}


