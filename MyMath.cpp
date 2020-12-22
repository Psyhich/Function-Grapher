#include "MyMath.h"

double random(double start, double end){
	double f = static_cast<double>(rand()) / RAND_MAX;
    return f * (end - start) + start;
}
//Funcs
double derivFunc(std::function<double(double)> func, double x){
  const double delta = 1e-12;
  return (func(x + delta) - func(x - delta)) / (2 * delta);
}

double sum(std::function<double(double)> funcToSum, long int count, double startPoint, double changer){
  double outVal = 0;
  for(long int i = 0; i < count; i++){
	outVal += funcToSum(startPoint);
	startPoint += changer;
  }
  return outVal;
}

//Integrations
double integrateMonteCarlo(std::function<double(double)> func, double startPoint, double endPoint, long int dots=1e+12){
    double sum = 0;
    for(long int i = 0; i < dots; i++){ 
        sum += func(random(startPoint, endPoint));
    }

    return ((endPoint - startPoint) / dots) * sum;
}
double integrateRiemans(std::function<double(double)> func, double startPoint, double endPoint, long accuracy=1e+12, double eps=1e-12){
	double h = (endPoint - startPoint) / accuracy;
	return h * sum(func, accuracy, startPoint + h / 2, h) + eps;
}
double integrateTrapeziodal(std::function<double(double)> func, double startPoint, double endPoint, long accuracy=1e+12, double eps=1e-12){
    double h = (endPoint - startPoint) / accuracy;
    return (h / 2) * 
        (func(startPoint) + func(endPoint)
			+ 2 * sum(func, accuracy - 3, startPoint + h * 2, h) ) + eps;
}
double integrateSimpsons(std::function<double(double)> func, double startPoint, double endPoint, long int accuracy=1e+12){
    double h = (endPoint - startPoint) / accuracy;
    return (h / 6) * 
        (func(startPoint) + func(endPoint)
			+ 4 * sum(func, accuracy, startPoint + h / 2, h)
			+ 2 * sum(func, accuracy - 1, startPoint + h, h));
}

//Fourie
void fourieSeriesDecompose(std::function<double(double)> func, double startPoint, double endPoint, long iterations, double *&aParams, double *&bParams){
  aParams = new double[iterations];
  bParams = new double[iterations];
  double frequency = 2 * M_PI / (endPoint - startPoint);

  for(long i = 0; i < iterations; i++){
	auto cosineFunc = [&](double x) -> double{ return func(x) * cos(x * frequency * i);};
	aParams[i] = integrateRiemans(cosineFunc, 0, endPoint - startPoint, 1000);// * 2 / (endPoint - startPoint);

	auto sineFunc = [&](double x) -> double{ return func(x) * sin(x * frequency * i); };
	bParams[i] = integrateRiemans(sineFunc, 0, endPoint - startPoint, 1000);// * 2 / (endPoint - startPoint);
  }

}
double fourieSeriesCompose(double x, double startPoint, double endPoint, long iterations, double *aParams, double *bParams){
  double y = aParams[0] / 2;
  double frequency = 2 * M_PI / (endPoint - startPoint);

  for(int i = 1; i < iterations; i++){
	  y += 
		  sqrt(pow(aParams[i], 2) + pow(bParams[i], 2))
			* cos(i * frequency * x - atan2(bParams[i], aParams[i]));
  }
  return  y;
}
