#include <cmath>
#include <functional>

struct Point{
  double *x, *y;
  Point(){
	  x = new double(0);
	  y = new double(0);
	}
  Point(double x, double y){
	this->x = new double(x);
	this->y = new double(y);
  }
  Point(double *x, double*y){
	this->x = x;
	this->y = y;
  }
  ~Point(){
	delete x;
	delete y;
  }
};


//Misc
double random(double start, double end);
//Function
double derivFunc(std::function<double(double)>, double x);
double funcSum(std::function<double(double)>, long int count, double startPoint, double changer);
//integrations
double integrateMonteCarlo(std::function<double(double)>, double startPoint, double endPoint, long int accuracy);
double integrateRiemans(std::function<double(double)>, double, double, long, double);
double integrateTrapeziodal(std::function<double(double)>, double startPoint, double endPoint, long int accuracy, double eps);
double integrateSimpsons(std::function<double(double)>, double startPoint, double endPoint, long int accuracy);

//Fourie
void fourieSeriesDecompose(std::function<double(double)> func, double startPoint, double endPoint, long int iterations, double *&aParams, double *&bParams);
double fourieSeriesCompose(double x, double startPoint, double endPoint, long iterations, double *aParams, double *bParams);
