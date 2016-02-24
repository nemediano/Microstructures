/*
 * Blob.cpp
 *
 *  Created on: 27/01/2011
 *      Author: jorge
 */

#include "Blob.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <string>
#include <algorithm>

using namespace std;

namespace blobs {
	const double PI = 3.1415926535897932384626433832795;

Blob::Blob(unsigned short m, float a, float alpha) {
	valuesComputed = fourierTransformComputed = gradientComputed = false;
	if (a <= 0.0) {
		cerr << "Incorrect radix to create a blob" << endl;
	} else {
		this->a = a;
	}

	if (alpha < 0.0) {
		cerr << "Incorrect decay parameter to create a blob" << endl;
	} else {
		this->alpha = alpha;
	}

	if (m != 0 && m != 1 && m != 2) {
		cerr << "Incorrect order to create a blob" << endl;
	} else {
		this->m = m;
	}

	fillBlobValues();
	fillGradientValues();
}

Blob::Blob(float a) {
    this->m = 2;
	valuesComputed = fourierTransformComputed = gradientComputed = false;
	
	if (a <= 0.0) {
		cerr << "Incorrect radix to create a blob" << endl;
	} else {
		this->a = a;
	}
	float u = 6.987932f; //zero of bessel function
	this->alpha = static_cast<float>(sqrt(4.0 * PI * PI * this->a * this->a - u * u));

	fillBlobValues();
	fillGradientValues();
}

Blob::Blob(float a, float alpha) {
	this->m = 2;
	valuesComputed = fourierTransformComputed = gradientComputed = false;
	if (a <= 0.0) {
		cerr << "Incorrect radix to create a blob" << endl;
	} else {
		this->a = a;
	}

	if (alpha < 0.0) {
		cerr << "Incorrect decay parameter to create a blob" << endl;
	} else {
		this->alpha = alpha;
	}

	fillBlobValues();
	fillGradientValues();
}

Blob::Blob() {
	valuesComputed = fourierTransformComputed = gradientComputed = false;
	this->a = 2.4f;
	this->alpha = 10.4f;
	this->m = 2;
	fillBlobValues();
	fillGradientValues();
}

Blob& Blob::operator=(const Blob& rhs) {
	//Prevent auto asignation
	if (this == &rhs) {
		return *this;
	}
	//Free resources
	delete[] blobValues;
	delete[] gradientValues;
	//Copy static members
	m = rhs.m;
	a = rhs.a;
	alpha = rhs.alpha;
	//Allocating new resources
	blobValues = new double[BLOB_VEC_SIZE];
	gradientValues = new double[BLOB_VEC_SIZE];
	//Copy dynamic members
	std::copy(rhs.blobValues, rhs.blobValues + BLOB_VEC_SIZE, blobValues);
	std::copy(rhs.gradientValues, rhs.gradientValues + BLOB_VEC_SIZE, gradientValues);

	return *this;
}


Blob::Blob(const Blob& copy) {
	//Copy static members
	m = copy.m;
	a = copy.a;
	alpha = copy.alpha;
	//Allocating new resources
	blobValues = new double[BLOB_VEC_SIZE];
	gradientValues = new double[BLOB_VEC_SIZE];
	//Copy dynamic members
	std::copy(copy.blobValues, copy.blobValues + BLOB_VEC_SIZE, blobValues);
	std::copy(copy.gradientValues, copy.gradientValues + BLOB_VEC_SIZE, gradientValues);
}

Blob::~Blob() {
	if (valuesComputed) {
		delete [] blobValues;
		valuesComputed = false;
	}

	if (gradientComputed) {
		delete [] gradientValues;
		gradientComputed = false;
	}

	if (fourierTransformComputed) {

	}
}

ostream& operator<< (ostream &os, const Blob &b) {
	return os << "Blob(m = " << b.m << ", a = " << b.a << ", alpha = " << b.alpha << ")";
}

//ostream Blob::valores(int tipo) {
//	ostream s;
//	return s;
//}

float Blob::fblob(float r) const {

	float fr = 0.0f;

	r = fabs(r);

	//Si estamos fuera del radio de este blob regresa cero
	if (r > a) {
		return fr;
	}

	/*Checamos que ya hayamos inicializado el arreglo de valores del blob, si no es asi
	 * lo hacemos
	 */
	if (!valuesComputed) {
		//fillBlobValues();
		cerr << "Los valores de este blob no estan inicializados" << endl;
	}

	/* Hacer interpolacion lineal para calcular el valor de fr, con los
	 * dos valores mas cercanos que tengamos guardados en el arreglo
	 */
	int indexfleft, indexfright;
	float delta = a / (BLOB_VEC_SIZE - 1);

	//Calculamos el indice del valor ma proximo a la izquierda y a la derecha
	indexfleft = static_cast<int>(floor(static_cast<int>(r / delta)));
	indexfright = static_cast<int>(ceil(static_cast<int>(r / delta)));

	//Si son iguales le pegamos a un punto exacto que si tenemos guardado
	//Y lo devolvemos
	if (indexfleft == indexfright) {
		fr = static_cast<float>(blobValues[indexfleft]);
	} else { //No son iguales haz interpolacion lineal entre los dos valores
		double fLeft, fRight;
		float t;
		fLeft = blobValues[indexfleft];
		fRight = blobValues[indexfright];
		t = r / delta - indexfleft;
		fr = static_cast<float>((1.0 - t) * fLeft + t * fRight);
	}

	return fr;
}

double Blob::dblob(double r) const {

	double fr = 0.0;

	r = fabs(r);

	//Si estamos fuera del radio de este blob regresa cero
	if (r > a) {
		return fr;
	}

	/*Checamos que ya hayamos inicializado el arreglo de valores del blob, si no es asi
	 * lo hacemos
	 */
	if (!valuesComputed) {
		//fillBlobValues();
		cerr << "Los valores de este blob no estan inicializados" << endl;
	}

	/* Hacer interpolacion lineal para calcular el valor de fr, con los
	 * dos valores mas cercanos que tengamos guardados en el arreglo
	 */
	int indexfleft, indexfright;
	double delta = a / (BLOB_VEC_SIZE - 1);

	//Calculamos el indice del valor ma proximo a la izquierda y a la derecha
	indexfleft = static_cast<int>(floor(r / delta));
	indexfright = static_cast<int>(ceil(r / delta));

	//Si son iguales le pegamos a un punto exacto que si tenemos guardado
	//Y lo devolvemos
	if (indexfleft == indexfright) {
		fr = blobValues[indexfleft];
	} else { //No son iguales haz interpolacion lineal entre los dos valores
		double fLeft, fRight, t;

		fLeft = blobValues[indexfleft];
		fRight = blobValues[indexfright];

		t = r / delta - indexfleft;
		fr = (1.0 - t) * fLeft + t * fRight;
	}

	return fr;
}

float Blob::gradient(float r) const {
	float gr = 0.0f;

	r = fabs(r);

	//Si estamos fuera del radio de este blob regresa cero
	if (r > a) {
		return gr;
	}

	/*Checamos que ya hayamos inicializado el arreglo de valores del blob, si no es asi
	 * lo hacemos
	 */
	if (!gradientComputed) {
		//fillGradientValues();
		cerr << "Los valores del gradiente de este blob no estan inicializados" << endl;
	}

	/* Hacer interpolacion lineal para calcular el valor de gr, con los
	 * dos valores mas cercanos que tengamos guardados en el arreglo
	 */
	int indexgleft, indexgright;
	float delta = a / (BLOB_VEC_SIZE - 1);

	//Calculamos el indice del valor ma proximo a la izquierda y a la derecha
	indexgleft = static_cast<int>(floor(r / delta));
	indexgright = static_cast<int>(ceil(r / delta));

	//Si son iguales le pegamos a un punto exacto que si tenemos guardado
	//Y lo devolvemos
	if (indexgleft == indexgright) {
		gr = static_cast<float>(gradientValues[indexgleft]);
	} else { //No son iguales haz interpolacion lineal entre los dos valores
		double gLeft, gRight;
		float t;
		gLeft = gradientValues[indexgleft];
		gRight = gradientValues[indexgright];
		t = r / delta - indexgleft;
		gr = static_cast<float>((1.0 - t) * gLeft + t * gRight);
	}

	return gr;
}

unsigned short Blob::getM() const {
	return m;
}

float Blob::getA() const {
	return a;
}

float Blob::getAlpha() const {
	return alpha;
}

bool Blob::fillBlobValues() {

	if (valuesComputed) {
		return valuesComputed;
	}

	blobValues = new double[BLOB_VEC_SIZE];

	if (!blobValues) {
		cerr <<"Error al llenar el vector de valores del blob" << endl;
		valuesComputed = false;
	}

	int i;
	double fr;
	double r = 0.0;
	double delta = a / (BLOB_VEC_SIZE - 1);

	for (i = 0; i < BLOB_VEC_SIZE; i++) {
		kaiser(&fr, r, a, alpha, m);
		blobValues[i] = fr;
		r += delta;
	}
	valuesComputed = true;


	return valuesComputed;
}

bool Blob::fillGradientValues() {

	if (gradientComputed) {
		return gradientComputed;
	}

	gradientValues = new double[BLOB_VEC_SIZE];

	if (!gradientValues) {
		cerr <<"Error al llenar el vector de valores del blob" << endl;
		gradientComputed = false;
	}

	int i;
	double gr;
	double r = 0.0;
	double delta = a / (BLOB_VEC_SIZE - 1);

	for (i = 0; i < BLOB_VEC_SIZE; i++) {
		kaisergradient(&gr, r, a, alpha, m);
		gradientValues[i] = gr;
		r += delta;
	}
	gradientComputed = true;


	return gradientComputed;
}

void Blob::printValues2File(int tipo, unsigned int granularidad, string fileName) const {
	ofstream outputFile;
	outputFile.open(fileName.c_str());

	if (outputFile.fail()) {
		cerr << "Imposible crear archivo de salida " << fileName << endl;
		return;
	}

	unsigned int i;
	float x = 0.0, fx;
	float delta = a / granularidad;
	outputFile << "#Blob: " << (*this) << endl;

	switch (tipo) {

		case BLOB_VALUES:
			outputFile << "#Blob Profile" << endl;
			for (i = 0; i <= granularidad; i++) {
				fx = fblob(x);
				outputFile << x << "\t" << fx << endl;
				x += delta;
			}

		break;

		case BLOB_GRADIENT:
			outputFile << "#Blob Gradient profile" << endl;
			for (i = 0; i <= granularidad; i++) {
				fx = gradient(x);
				outputFile << x << "\t" << fx << endl;
				x += delta;
			}
		break;

		case BLOB_FURIER_TRANSFORM:
		break;

		default:
			cerr << "Invalid type to print a file" << endl;
		break;
	}

	outputFile.close();
	return;
}
/*==============================================================================*/
/*============Funciones tomadas del codigo de sampling de Edgar=================*/
/*==============================================================================*/

/* Function to compute the Bessel 0 */
double Blob::bessi0(double x)
{
 double y,ax,ans;

 ax=fabs(x);
 if(ax < 3.75) {
    y=(x*x)/(3.75*3.75);
    ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492
       +y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
   }
 else{
   y=3.75/ax;
   ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1
      +y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2
      +y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1
      +y*0.392377e-2))))))));
  }
 return ans;
}

/* Function to compute the Bessel 1 */
double Blob::bessi1(double x)
{
 double ax,ans,y;

 ax=fabs(x);
 if(ax < 3.75){
    y=x/3.75;
    y*=y;
    ans=ax*(0.5+y*(0.87890594+y*(0.51498869+y*(0.15084934
        +y*(0.2658733e-1+y*(0.301532e-2+y*0.32411e-3))))));
   }
 else{
    y=3.75/ax;
    ans=0.2282967e-1+y*(-0.2895312e-1+y*(0.1787654e-1
       -y*0.420059e-2));
    ans=0.39894228+y*(-0.3988024e-1+y*(-0.362018e-2
    +y*(0.163801e-2+y*(-0.1031555e-1+y*ans))));
    ans *= (exp(ax)/sqrt(ax));
   }
 return (x < 0.0)? -ans : ans;
}

/* Function to compute the Bessel 2 */
double Blob::bessi2(double x)
{
 double ans;

 if(x==0.0)
    ans = 0.0;
 else
    ans = bessi0(x) - (2.0/x) * bessi1(x);

 return ans;
}

/* Function to compute the Bessel 3 */
double Blob::bessi3(double x)
{
 double ans;

 if(x==0.0)
    ans = 0.0;
 else
   ans = bessi1(x) - (4.0/x) * bessi2(x);

 return ans;
}

/* Function to compute the value of the Spatial Kaiser-Bessel Function */
/*
 * r stands for the radial distance from the blob center (variable)
 * CONSTANTS:
 * a stands for the radius
 * alpha for the decay (shape of the blob)
 * m stands for the number of derivatives
 *
 */
int Blob::kaiser(double *y, double r, double a, double alpha, short m)
{
 double rda,rdas,arg,w;

 rda = r/a;
 rdas = rda*rda;
 if(rdas <= 1.0){
    arg = alpha * sqrt(1.0-rdas);
    if(m==0)
       w = bessi0(arg) / bessi0(alpha);
    else{
       if(m==1){
	  if(alpha==0.0)
	     w = 1.0-rdas;
	  else
	     w = sqrt(1.0-rdas) * bessi1(arg) / bessi1(alpha);
	 }
       else{
	  if(m==2){
             if(alpha==0.0)
	        w = (1.0-rdas)*(1.0-rdas);
	     else
	        w = (1.0-rdas) * bessi2(arg) / bessi2(alpha);
	    }
	  else
             return 0;
	 }
      }
   }
 else{
   *y = 0.0;
   return 0;
  }

 *y=w;

 return 1;
}

/* Function to compute the value of the Fourier Transformed Kaiser-Bessel Function */
/*
 * t stands for the Fourier Space variable
 * CONSTANTS:
 * a stands for the radius
 * alpha for the decay (shape of the blob)
 * m stands for the number of derivatives
 * n stands for number of dimensions
 *
 */
int Blob::fkaiser(double *r,double t,double a,double alpha,double m,double n)
{
 double c1,c2,nh,nhpm,beta,betsq,alfsq,z,f,gam,fact;
 short i;

 if(m==0)
    c1 = 1.0 / bessi0(alpha);
 else{
    if(m==1){
       if(alpha==0.0)
          c1 = 1.0;
       else
          c1 = 0.5 * alpha / bessi1(alpha);
      }
    else{
       if(m==2){
          if(alpha==0.0)
	     c1 = 2.0;
	  else
	     c1 = 0.5*0.5 * alpha*alpha / bessi2(alpha);
	 }
       else{
	  fprintf(stdout,"%c",7);
	  printf("ERROR, Computing the Fourier Transform of the blob");
	  return 0;
	 }
      }
   }

 nh = n/2;
 nhpm = nh + m;
 c2 = pow(PI,nh) * pow(a,n);

 /*
  * If n is even, compute simple factorial (n/2+m)!
  * If n is odd, compute (pi^(-1/2)) * Gamma(n/2+m+1)
  *
  */

 if(nh*2==n){
    fact = 0.0;
    gam = 1.0;
   }
 else{
    fact = 0.5;
    gam = 0.5;
   }
 for(i=1;i<=nhpm;i++){
     fact = fact + 1.0;
     gam = gam * fact;
    }

 beta  = 2 * PI * a * t;
 betsq = beta * beta;
 alfsq = alpha * alpha;
 z = sqrt(fabs(betsq-alfsq));
 f = c1 * c2 /gam;
 if(z==0.0){
    *r=z;
    return 1;
   }

 if((n+2*m) == 1){
    if(betsq < alfsq)
       f = f * (sinh(z)/z);
    else
       f = f * (sin(z)/z);
   }
 else{
    if((n+2*m) == 2){
       if(betsq < alfsq)
          f = f * (2.0*bessi1(z)/z);
       else
          f = f * (2.0*_j1(z)/z);
      }
    else{
       if((n+2*m) == 3){
          f = f * (3.0/(z*z));
	  if(betsq < alfsq)
	     f = f * (cosh(z) - sinh(z)/z);
	  else
	     f = f * (sin(z)/z - cos(z));
         }
       else{
          if((n+2*m) == 4){
	     if(betsq < alfsq)
	        f = f * (8.0*bessi2(z)/(z*z));
	     else
	        f = f * (8.0*_jn(2,z)/(z*z));
	    }
	  else{
	     if((n+2*m) == 5){
	        f = f * 15.0/(z*z*z);
		if(betsq < alfsq)
		   f = f * ((3.0/(z*z)+1.0)*sinh(z) - (3.0/z)*cosh(z));
		else
		   f = f * ((3.0/(z*z)-1.0)*sin(z) - (3.0/z)*cos(z));
	       }
	     else{
	        if((n+2*m) == 6){
		   if(betsq < alfsq)
		      f = f * (48.0*bessi3(z)/(z*z*z));
		   else
		      f = f * (48.0*_jn(3,z)/(z*z*z));
		  }
		else{
		   if((n+2*m) == 7){
		      f = f * (105.0/(z*z*z*z));
		      if(betsq < alfsq)
		         f = f * ((1.0+15.0/(z*z))*cosh(z) - (15.0/(z*z)+6.0)*sinh(z)/z);
		      else
		         f = f * ((1.0-15.0/(z*z))*cos(z) + (15.0/(z*z)-6.0)*sin(z)/z);
	             }
		   else{
		      if((n+2*m) == 8){
		         f = f * (384.0/(z*z*z*z));
			 if(betsq < alfsq)
			    f = f * (bessi2(z) - (6.0/z)*bessi3(z));
			 else
			    f = f * _jn(4,z);
			}
		      else{
		         fprintf(stdout,"%c",7);
			 printf("n+2*m out of range in function kaiser_trans()");
			 return 0;
			}
		     }
		  }
	       }
	    }
	 }
      }
  }

 *r=f;

 return 1;
}

/* Function to compute the gradient of a blob */
/* See Robert Lewitt's Paper "Multidimensional Digital Image Representations
   Using Generalized Kaise-Bessel Window Functions"
 */
int Blob::kaisergradient(double *y,double r,double a,double alpha,short m)
{
 double z,ub,db;

 if(r>=0 && r<=a){
    z=alpha*sqrt(1-(r*r/(a*a)));

    switch(m){
        case 0:
 	     fprintf(stdout,"%c",7);
	     printf("ERROR, Computing the gradient of the blob");
	     return 0;
	     break;
        case 1:
 	     *y=-(alpha*r*bessi0(z))/(a*a*z*bessi1(alpha));
	     break;
        case 2:
 	     ub=bessi1(z);
	     db=bessi2(alpha);
	     *y=(-1.0*r*z*ub)/(a*a*db);
	     break;
        default:
 	     printf("ERROR, Computing the gradient of the blob");
	     return 0;
	     break;
       }
   }
 else
    return 0;

 return 1;
}

}

