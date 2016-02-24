/*
 * Blob.h
 *
 *  Created on: 27/01/2011
 *      Author: jorge
 */

#ifndef BLOB_H_
#define BLOB_H_
#include <iostream>
#include <string>
using namespace std;

/**Esta clase representa el blob Kaiser Bessel generalizado
 * Como lo platica el apéndice del articulo de Robert M. Lewitt
 * Multidimensional digital image representation using generalized Kaiser-Bessel window functions
 * Tome parte del codigo de Edgar en el sampling
 */

namespace blobs {

class Blob {
private:
	unsigned short m; //Número de derivadas
	float a; //Radio del blob
	float alpha; //Parámetro de suavidad
	bool valuesComputed;
	bool fourierTransformComputed;
	bool gradientComputed;
	double *blobValues;
	double *gradientValues;
	bool fillBlobValues();
	bool fillGradientValues();
	/*Las rutinas copiadas del codigo de sampliing */
	double bessi0(double x);
	double bessi1(double x);
	double bessi2(double x);
	double bessi3(double x);
	int kaiser(double *y, double r, double a, double alpha, short m);
	int fkaiser(double *r, double t, double a, double alpha, double m, double n);
	int kaisergradient(double *y, double r, double a, double alpha, short m);

public:
	enum {BLOB_VEC_SIZE = 2000, //Número de muestras que se usan para aproximar el blob en todos sus valores
		  BLOB_VALUES = 1, //Pedir que imprima los valores del BLOB
		  BLOB_FURIER_TRANSFORM = 2, //Pedir que imprima los valores de la transformada de Furier del Blob
		  BLOB_GRADIENT = 3}; //Pedir que imprima los valores del gradiente del blob
	//Constructor completo, determina totalmente el blob
	Blob(unsigned short m, float a, float alpha);
	//Crea un blor con el radio especificado y la suavidad especificados, asume m=2
	Blob(float a, float alpha);
	//Crea el blob de radio a, con el alpha optimo usando el criterio de muestreo de fourier de la tesispara sc
	Blob(float a);
	//Crea el blob de default con el articulo de Edgar
	Blob();
	//Copy constructor implemented to follow the rule of three
	Blob(const Blob& copy);
	//Equal operator implemented to follow the rule of three
	Blob& operator=(const Blob& rhs);
	//Destructor limpia la memoria de los arreglos necesitados
	virtual ~Blob();
	//Imprime los parámetros del Blob usado
	friend ostream& operator<< (ostream &os, const Blob &b);
	//Manda los valores interpolados de este blob
	void printValues2File(int tipo, unsigned int granularidad, string fileName) const;
	//Evalua este blob en el punto a distancia r del origen
	float fblob(float r) const;
	//Evalua este blob en el punto a distancia r del origen. En precisión double
	double dblob(double r) const;
	//Evalua la transformada de furier de este blob en el puntoa distancia r, del origen
	float fourierTransform(float t) const;
	//Evalua el gradiente de este blob en el punto a distancia r del origen
	float gradient(float r) const;
	//Getters de este blob
	//Devuelve el numero de derivadas del blob
	unsigned short getM() const;
	//Devuelve el radio de este Blob
	float getA() const;
	//Devuelve el parametro de suavidad de este blob
	float getAlpha() const;
};

}

#endif /* BLOB_H_ */
