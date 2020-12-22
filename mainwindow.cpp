#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <limits>

//Function variables
static int functionIndex;
//Params
static double aParam, bParam, cParam;

//Constructors and destructors
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
  ui->setupUi(this);

  points = new Point[1];

  //Setting size of drawing area
  startX = 280;
  endX = 940;
  startY = 10;
  endY = 380;

  //Setting starting values
  aParam = 0;
  bParam = 0;
  cParam = 0;

  numberOfHarmonics = 1;
  tabulateMagn = false;

  isYValuesOverriden = false;
  shouldTabulate = false;

  currentShift = 0;

  animationSpeed = 1;
}
MainWindow::~MainWindow(){
  delete ui;
}

//Mathematical stuff
double func(double x){
  switch (functionIndex) {
	case 1:
	  return aParam * x * x + bParam * x + cParam;
	case 2:
	  return aParam * sin(bParam * x) + cParam;
	case 3:
	  return aParam * log(bParam * x) + cParam;
	case 4:
	  return 1 / pow(aParam * x, bParam) + cParam;
  }
}

//Switches for colours
QColor colorChooser(int colorID){
  switch(colorID){
	case 1:{
	  return QColor(255, 0, 0);//That's red
	}
	case 2:{
	  return QColor(0, 0, 0);//That's black
	}
	case 3:{
	  return QColor(255, 255, 255);//That's white
	}
	default:{
	  return QColor(255, 255, 255);//Black for default
	}
  }
}
Qt::PenStyle styleChooser(int styleID){
  switch(styleID){
	case 1: return Qt::SolidLine;
	case 2: return Qt::DotLine;
	case 3: return Qt::DashLine;
	default: return Qt::SolidLine;

  }
}

//Value checker
bool isValueChanged(double newValue, double* valueToChange){
  if(newValue != *valueToChange){
	  *valueToChange = newValue;
	  return true;
  }
  return false;
}
bool isValueChanged(int newValue, int* valueToChange){
  if(newValue != *valueToChange){
	  *valueToChange = newValue;
	  return true;
  }
  return false;
}
bool isValueChanged(bool newValue, bool* valueToChange){
  if(newValue != *valueToChange){
	  *valueToChange = newValue;
	  return true;
  }
  return false;
}

//Updater that would call drawer
void MainWindow::updateValues(){
  shouldTabulate = false;

  //Getting parameters values and checking if we need to retabulate function
  //Getting function index
  shouldTabulate = isValueChanged(ui->FunctionChooserComboBox->currentIndex(), &functionIndex) || shouldTabulate;
  //Checking parameters
  shouldTabulate = isValueChanged(ui->AParamDoubleSpinBox->value(), &aParam) || shouldTabulate;
  shouldTabulate = isValueChanged(ui->BParamDoubleSpinBox->value(), &bParam) || shouldTabulate;
  shouldTabulate = isValueChanged(ui->CParamDoubleSpinBox->value(), &cParam) || shouldTabulate;
  //Checking points
  shouldTabulate = isValueChanged(ui->StartPointDoubleSpinBox->value(), &startPoint) || shouldTabulate;
  shouldTabulate = isValueChanged(ui->EndPointDoubleSpinBox->value(), &endPoint) || shouldTabulate;
  shouldTabulate = isValueChanged(ui->PointsCountSpinBox->value(), &pointsCount) || shouldTabulate;
  //Checking Y points
  shouldTabulate = isValueChanged(ui->OverrideYBoundsCheckBox->isChecked(), &isYValuesOverriden) || shouldTabulate;
  if(isYValuesOverriden){
	shouldTabulate = isValueChanged(ui->YStartPointDoubleSpinBox->value(), &yStartPoint) || shouldTabulate;
	shouldTabulate = isValueChanged(ui->YEndPointDoubleSpinBox->value(), &yEndPoint) || shouldTabulate;
  }
  if(ui->UseFouriersRadioButton->isChecked()){
	shouldTabulate = isValueChanged(ui->HarmonicsNumberSpinBox->value(), &numberOfHarmonics)|| shouldTabulate;
	shouldTabulate = isValueChanged(ui->ShowHarmonicsCheckBox->isChecked(), &tabulateMagn)|| shouldTabulate;
  }

  //Getting grid and function pen properties
  //Grid
  gridPen.setColor(colorChooser(ui->GridColorComboBox->currentIndex()));
  gridPen.setStyle(styleChooser(ui->GridStyleComboBox->currentIndex()));
  //Function
  functionPen.setColor(colorChooser(ui->FunctionColorComboBox->currentIndex()));
  functionPen.setStyle(styleChooser(ui->FunctionStyleComboBox->currentIndex()));
}

//Function calcules
void MainWindow::tabulateFunc(std::function<double(double)> func){

  if(startPoint < endPoint && pointsCount > 2 ){
	  points = new Point[ pointsCount ];

	  double step = (endPoint - startPoint) / pointsCount;
	  int index = 0;

	  //If our values are override and correct, so we don't change them
	  if(!isYValuesOverriden || yEndPoint < yStartPoint){
		yStartPoint = func(startPoint);
		yEndPoint = func(endPoint);
	  }

	  //Tabulating
	  for(double x = startPoint; index < pointsCount; x += step){

		*points[index].x = x;
		*points[index].y = func(x);

		//Checking if our values are not overriden if they are not, finding max and min values
		if(!isYValuesOverriden || yEndPoint < yStartPoint){
		  //If this is not infinity then checking for max or min value
		  if(*(points[index].y) <= yStartPoint || (yStartPoint < 0 && (std::isinf(yStartPoint) || std::isnan(yStartPoint)))){
			 yStartPoint = *points[index].y;
		  }
		  if(*(points[index].y) >= yEndPoint || (yEndPoint > 0 && (std::isinf(yEndPoint) || std::isnan(yEndPoint)))){
			 yEndPoint = *points[index].y;
		  }
		}
		index++;
	  }
  }
}
void MainWindow::scaleFunc(){
  //Assigning drawing variables
  xStep = (endX - startX) / 10;
  yStep = (endY - startY) / 10;

  xScaling = (endPoint - startPoint) / (endX - startX);
  yScaling = (yEndPoint - yStartPoint) / (endY - startY);

  //Applying scaling
  double yScaledStartPoint = endY - (yStartPoint + abs(yStartPoint)) / yScaling;
  double yScaledEndPoint = endY - (yEndPoint + abs(yStartPoint)) / yScaling;

  for(int i = 0; i < pointsCount; i++){
	*points[i].x = (*(points[i].x) + abs(startPoint)) / xScaling + startX;

	if(*points[i].y >= yStartPoint && *points[i].y <= yEndPoint){
	  *points[i].y = endY - (*(points[i].y) + abs(yStartPoint)) / yScaling;
	}else if(*points[i].y < yStartPoint){
	  *points[i].y = yScaledStartPoint;
	}else{
	  *points[i].y = yScaledEndPoint;
	}
	//std::cout << "Point X: " << *(points[i].x) << " Y:" << *(points[i].y) << std::endl;
  }

}

void MainWindow::tabulateMagnitudes(double *aParams, double *bParams){
  magnPoints = new Point[numberOfHarmonics];

  *magnPoints[0].x = 0;
  *magnPoints[0].y = sqrt(pow(aParams[0], 2) + pow(bParams[0], 2));

  maxMagn = *magnPoints[0].y;

  double frequency = 2 * M_PI / (endPoint - startPoint);
  for(int i = 1; i < numberOfHarmonics; i++){
	*magnPoints[i].x = frequency * i;
	*magnPoints[i].y = sqrt(pow(aParams[i], 2) + pow(bParams[i], 2));
	if(*magnPoints[i].y > maxMagn){
	  maxMagn = *magnPoints[i].y;
	}
  }



}
void MainWindow::scaleMagnitudes(){
  //Drawing borders
  xStep = (endX - startX) / numberOfHarmonics;
  yStep = (endY - startY) / 10;

  xScaling = *magnPoints[numberOfHarmonics - 1].x / (endX - startX);
  yScaling = maxMagn / (endY - startY);

  for(int i = 0; i < numberOfHarmonics; i++){
	*magnPoints[i].x = *magnPoints[i].x / xScaling + startX;
	*magnPoints[i].y = endY - *magnPoints[i].y / yScaling;
  }
}

//Drawing functions
void MainWindow::drawGrid(QPainter *painter){
  if((startPoint < endPoint)){

	painter->setPen(gridPen);

	//Drawing grid and text
	painter->setFont(QFont("Arial", 4, 1, false));

	for (int i = 0; i < endX - startX; i += xStep) {
	  painter->drawLine(i + startX, startY, i + startX, endY);
	  painter->drawText(QPoint(startX + i, endY + 5), QString::number(i * xScaling + startPoint, ' ', 3));
	}
	for (int i = 0; i < endY - startY; i += yStep) {
	  painter->drawLine(startX, i + startY, endX, i + startY);
	  painter->drawText(startX - 15, i + startY, QString::number(yEndPoint - i * yScaling, ' ', 3));
	}

  }
}
void MainWindow::drawFunc(QPainter *painter){
  if((functionIndex > 0 || ui->UseFouriersRadioButton->isChecked()) && (startPoint < endPoint)){
	painter->setPen(functionPen);

	for(int i = 1; i < pointsCount; i++){
	  painter->drawLine(QPointF(*points[i - 1].x, *points[i - 1].y), QPointF(*points[i].x, *points[i].y));
	}
  }
}
void MainWindow::drawFreqDomainSig(QPainter *painter){
  painter->setPen(functionPen);
  for(int i = 0; i < numberOfHarmonics; i++){
	QPoint leftTop = QPoint(*magnPoints[i].x - yStep / 4 , *magnPoints[i].y -  yStep / 4);
	painter->drawArc(QRect(leftTop, QSize(yStep / 2, yStep / 2)), 0, 5760);
  }
}
void MainWindow::animateFunc(QPainter *painter){
  painter->setPen(functionPen);
  Point pointsToShow[pointsCount];

  currentShift += animationSpeed;

  if(currentShift >= pointsCount){
	currentShift = 0;
  }

  double step = (endPoint - startPoint) / pointsCount;
  int index = 0;

  for(int i = currentShift; index < pointsCount; i++){
	if(i >= pointsCount){
	  i = 0;
	}

	*pointsToShow[index].x = ((index * step) / xScaling) + startX;
	*pointsToShow[index].y = *points[i].y;

	++index;
  }

  for(int i = 1; i < pointsCount; i++){
	if(i != pointsCount - currentShift){
	  painter->drawLine(QPointF(*pointsToShow[i - 1].x, *pointsToShow[i - 1].y), QPointF(*pointsToShow[i].x, *pointsToShow[i].y));
	}
  }
  
  update();
}


//On draw event
void MainWindow::paintEvent(QPaintEvent*){
  QPainter painter(this);
  if(pointsCount == ui->PointsCountSpinBox->value()){
	drawGrid(&painter);
	if(tabulateMagn){
	  drawFreqDomainSig(&painter);
	}else{
	  if(ui->AnimateCheckBox->isChecked()){
		animateFunc(&painter);
	  }else{
		drawFunc(&painter);
	  }
	}
  }
}

//Slots definitions
void MainWindow::on_DrawButton_clicked(){
  updateValues();
  //Tabulating function and finding highest and lowest values

  if(shouldTabulate && (startPoint < endPoint)){
	if(ui->UseFunctionRadioButton->isChecked()){
		tabulateFunc(func);
		scaleFunc();
	}else if(ui->UseFouriersRadioButton->isChecked()){
	  if(!ui->checkBox->isChecked()){
		auto myFunc = [](double x) -> double{
		  /*if(x >= 5){
			return 10 - x;
		  }
		  return x;*/
		  return x / 4;
		};

		fourieSeriesDecompose(myFunc, startPoint, endPoint, numberOfHarmonics, aCoeficients, bCoeficients);
	  }
	  if(tabulateMagn){
		tabulateMagnitudes(aCoeficients, bCoeficients);
		scaleMagnitudes();
	  }else{
		auto readyFunc = [&](double x) -> double{ return fourieSeriesCompose(x, startPoint, endPoint, numberOfHarmonics, aCoeficients, bCoeficients); };
		tabulateFunc(readyFunc);
		scaleFunc();
	  }

	}
  }

  update();

}

void MainWindow::on_UseFouriersRadioButton_clicked(bool checked){
  if(checked){
	ui->UseFunctionRadioButton->setChecked(false);
  }
}

void MainWindow::on_UseFunctionRadioButton_clicked(bool checked){
  if(checked){
	ui->UseFouriersRadioButton->setChecked(false);
  }
}

void MainWindow::on_ExportButton_clicked(){
  QFile file(QFileDialog::getSaveFileName(this, tr("Save current harmonics to:"), "", tr("Harmonics file (*.harm)")), this);
  file.open(QIODevice::WriteOnly | QIODevice::Truncate);

  file.write((char*)&startPoint, sizeof(double*));
  file.write((char*)&endPoint, sizeof(double*));
  file.write((char*)&pointsCount, sizeof(int*));
  file.write((char*)&numberOfHarmonics, sizeof(int*));
  for(int i = 0; i < numberOfHarmonics; i++){
	file.write((char*)&aCoeficients[i], sizeof(double*));
  }
  for(int i = 0; i < numberOfHarmonics; i++){
	file.write((char*)&bCoeficients[i], sizeof(double*));
  }
  file.close();
}

void MainWindow::on_ImportButton_clicked(){
	QFile file(QFileDialog::getOpenFileName(this, tr("Load harmonics:"), "", tr("Harmonics file (*.harm)")));
	file.open(QIODevice::ReadOnly);

	file.read((char*)&startPoint, sizeof (double*));
	file.read((char*)&endPoint, sizeof(double*));
	file.read((char*)&pointsCount, sizeof(int*));
	file.read((char*)&numberOfHarmonics, sizeof(int*));
	aCoeficients = new double[numberOfHarmonics];
	for(int i = 0; i < numberOfHarmonics; i++){
	  file.read((char*)&aCoeficients[i], sizeof(double*));
	}
	bCoeficients = new double[numberOfHarmonics];
	for(int i = 0; i < numberOfHarmonics; i++){
	  file.read((char*)&bCoeficients[i], sizeof(double*));
	}

	file.close();
	ui->StartPointDoubleSpinBox->setValue(startPoint);
	ui->EndPointDoubleSpinBox->setValue(endPoint);
	ui->PointsCountSpinBox->setValue(pointsCount);
	ui->HarmonicsNumberSpinBox->setValue(numberOfHarmonics);

	auto readyFunc = [&](double x) -> double{ return fourieSeriesCompose(x, startPoint, endPoint, numberOfHarmonics, aCoeficients, bCoeficients); };
	tabulateFunc(readyFunc);
	scaleFunc();

	return;
}

void MainWindow::on_checkBox_stateChanged(int state){
  bool isChecked = state;
  ui->UseFunctionRadioButton->setChecked(false);
  ui->UseFouriersRadioButton->setChecked(true);


  ui->UseFunctionRadioButton->setEnabled(!isChecked);
  ui->UseFouriersRadioButton->setEnabled(!isChecked);

  ui->StartPointDoubleSpinBox->setEnabled(!isChecked);
  ui->EndPointDoubleSpinBox->setEnabled(!isChecked);

  ui->HarmonicsNumberSpinBox->setEnabled(!isChecked);




}

void MainWindow::on_AnimSpeedHorizontalSlider_valueChanged(int value){
  animationSpeed = value;
}
