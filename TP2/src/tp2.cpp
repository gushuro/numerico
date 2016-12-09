#include <iostream>
#include <istream>
#include <fstream>
#include <cmath>
#include "tp2.h"
#include <cstdio>
#include <vector>
#include <stdlib.h>
#include <iomanip>
#include <time.h>
#include <ctime>

using namespace std;

typedef vector < vector < double > > matriz;
matriz ImagenesEntrenamiento, ImagenesTest, matrizCovarianzas, autovectoresTraspuestos, ImagenesEntrenamientoCentradas, ImagenesTestCentradas, ImagenesEntrenamientoPLS, ImagenesTestPLS, ImagenesEntrenamientoPCA, ImagenesTestPCA;

vector<int> resultados;
vector<double>autovalores, media;

string RutaArchivoEntrada = "", RutaArchivoSalida = "", RutaArchivoSalidaReporte = "";
ifstream TestEntrada;

int k, alfa, Gamma, metodo, K, NoHayTest;
string RutaImgs;

int main(int argc, char** argv){


	if (argc == 3) {
		RutaArchivoEntrada = argv[1];
		RutaArchivoSalida = argv[2];
		metodo = 1; // 0: kNN, 1: PCA + kNN, 2: PLS-DA + kNN
		TestEntrada.open(RutaArchivoEntrada.c_str());
	}
	
	if (argc == 4) {
		RutaArchivoEntrada = argv[1];
		RutaArchivoSalida = argv[2];
		metodo = atoi(argv[3]); // 0: kNN, 1: PCA + kNN, 2: PLS-DA + kNN
		TestEntrada.open(RutaArchivoEntrada.c_str());
	}

	if (argc < 3 || argc > 4) {
		cout << "NO HAY TEST" << endl;
		return 1; // salir con código de error 1
		/*
		NoHayTest = 1;
		K = 1;
		alfa = 50;
		Gamma = 10;
		k = 5;
		*/
	}

	TestEntrada >> RutaImgs >> k >> alfa >> Gamma >> K;

	RutaArchivoSalidaReporte = RutaArchivoSalida + ".report";
	FILE* ArchivoSalidaReporte 	= fopen(RutaArchivoSalidaReporte.c_str(),	 "w");
	FILE* ArchivoSalida 		= fopen(RutaArchivoSalida.c_str(),		 "w");

	string met;
	std::clock_t start;
	double duration = 0;
	double tiempo = 0;

	float promedio = 0;
	float aux = 0;
	float precision = 0;
	float auxprecision = 0;
	float recall = 0;
	float auxrecall = 0;

	for(int i = 0; i < K; i++){

		imagenes_A_Vectores(ImagenesEntrenamiento, ImagenesTest, TestEntrada, NoHayTest, i, RutaImgs);

		if(metodo == 0){
			met = "Knn";
			start = std::clock();
			resultados = Knn(ImagenesEntrenamiento, ImagenesTest, k, alfa, metodo);
			duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
			fprintf(ArchivoSalidaReporte, "Tiempo de esta corrida: %4.6f\n", duration);
			tiempo = tiempo + duration;
			aux = resultadosCorrectos(ImagenesTest, resultados, i, ArchivoSalidaReporte);
			promedio = promedio + aux;
			auxprecision = Precision(ImagenesTest, resultados, i, ArchivoSalidaReporte);
			precision = precision + auxprecision;
			auxrecall = Recall(ImagenesTest, resultados, i, ArchivoSalidaReporte);
			recall = recall + auxrecall;
		}

		if(metodo == 1)	{
			met = "PCA + Knn";
			start = std::clock();
		

			matrizCovarianzas = matrizCovarianza(ImagenesEntrenamiento, media);
			autovalores = metodoDeLaPotencia(matrizCovarianzas, alfa, autovectoresTraspuestos);
			mostrarVector(autovalores);

			COUT << "REALIZANDO TRANSFORMACION CARACTERISTICA" << endl << endl;

			ImagenesEntrenamientoPCA = ImagenesEntrenamiento;
			centrar(ImagenesEntrenamientoPCA, media, ImagenesEntrenamiento.size());

			int j = 0;
			while(j < ImagenesEntrenamiento.size()) {
				ImagenesEntrenamientoPCA[j] = transformacionCaracteristica(ImagenesEntrenamiento[j], autovectoresTraspuestos);
				j++;
			}

			ImagenesTestPCA = ImagenesTest;
			centrar(ImagenesTestPCA, media, ImagenesEntrenamiento.size());

			j = 0;
			while(j < ImagenesTest.size()) {
				ImagenesTestPCA[j] = transformacionCaracteristica(ImagenesTest[j], autovectoresTraspuestos);
				j++;
			}
				
			resultados = Knn(ImagenesEntrenamientoPCA, ImagenesTestPCA, k, alfa, metodo);

		}

		if (metodo == 1) {
			duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
			fprintf(ArchivoSalidaReporte, "Tiempo de esta corrida: %4.6f\n", duration);
			tiempo = tiempo + duration;
			aux = resultadosCorrectos(ImagenesTest, resultados, i, ArchivoSalidaReporte);
			promedio = promedio + aux;
			auxprecision = Precision(ImagenesTest, resultados, i, ArchivoSalidaReporte);
			precision = precision + auxprecision;
			auxrecall = Recall(ImagenesTest, resultados, i, ArchivoSalidaReporte);
			recall = recall + auxrecall;
		

			int j = 0;
			while (j < alfa) {
				fprintf(ArchivoSalida, "%1.6e\n", autovalores[j]);
				j++;
			}
			metodo++;
		}

		/* acá empieza el método 2 */

		if (metodo == 2) {
			met = "PLS-DA + Knn";
			start = std::clock();
		

			autovalores = PLS_DA(ImagenesEntrenamiento, media, Gamma, autovectoresTraspuestos);
			mostrarVector(autovalores);

			COUT << "REALIZANDO TRANSFORMACION CARACTERISTICA" << endl << endl;

			ImagenesEntrenamientoPLS = ImagenesEntrenamiento;
			centrar(ImagenesEntrenamientoPLS, media, ImagenesEntrenamiento.size());

			int j = 0;
			while(j < ImagenesEntrenamiento.size()) {
				ImagenesEntrenamientoPLS[j] = transformacionCaracteristica(ImagenesEntrenamiento[j], autovectoresTraspuestos);
				j++;
			}

			ImagenesTestPLS = ImagenesTest;
			centrar(ImagenesTestPLS, media, ImagenesEntrenamiento.size());

			j = 0;
			while(j < ImagenesTest.size()) {
				ImagenesTestPLS[j] = transformacionCaracteristica(ImagenesTest[j], autovectoresTraspuestos);
				j++;
			}
				
			resultados = Knn(ImagenesEntrenamientoPLS, ImagenesTestPLS, k, Gamma, metodo);

		}

		if (metodo == 2) {
		        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		        fprintf(ArchivoSalidaReporte, "Tiempo de esta corrida: %4.6f\n", duration);
			tiempo = tiempo + duration;
			aux = resultadosCorrectos(ImagenesTest, resultados, i, ArchivoSalidaReporte);
			promedio = promedio + aux;
			auxprecision = Precision(ImagenesTest, resultados, i, ArchivoSalidaReporte);
			precision = precision + auxprecision;
			auxrecall = Recall(ImagenesTest, resultados, i, ArchivoSalidaReporte);
			recall = recall + auxrecall;
		

			int j = 0;
			while (j < Gamma) {
				fprintf(ArchivoSalida, "%1.6e\n", autovalores[j]);
				j++;
			}
		}
		metodo--;
	}
		//matrizCovarianzas = ImagenesEntrenamiento; //	CUANDO CARGO DIRECTAMENTE LA MATRIZ DE COVARIANZAS

		//testCovarianzaSimetrica(matrizCovarianzas);

		//escribirMatrizEnArchivo(matrizCovarianzas, ArchivoSalida);
    
	
    
	fprintf(ArchivoSalidaReporte, "***RESULTADOS***\n");
	fprintf(ArchivoSalidaReporte, "PARAMETROS: k: %d, alfa: %d, gamma: %d, K: %d, metodo: %s\n", k,alfa,Gamma,K,met.c_str());


	fprintf(ArchivoSalidaReporte, "Tiempo: %4.6f\n", tiempo);

	promedio = promedio/K;
	fprintf(ArchivoSalidaReporte, "Promedio de aciertos: %4.6f\n", promedio);

	precision = precision/K;
	fprintf(ArchivoSalidaReporte, "Precision: %4.6f\n", precision);

	recall = recall/K;
	fprintf(ArchivoSalidaReporte, "Recall: %4.6f\n", recall);


	fclose(ArchivoSalidaReporte);
	fclose(ArchivoSalida);

	return 0;  // salir sin error
}

