#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPen>
#include <QPainter>
#include <QFileDialog>
#include "MyMath.h"
#include <QFile>
#include <QtDebug>
#include <memory>

namespace Ui {
  class MainWindow;
}

//Struct for pointer values

class MainWindow : public QMainWindow{
	Q_OBJECT

  public:
	bool shouldTabulate;
	bool tabulateMagn;
	//X border points
	double startPoint, endPoint;
	//Y border points
	double yStartPoint, yEndPoint;
	double maxMagn;
	bool isYValuesOverriden;
	//Points and function type
	int pointsCount;
	Point *points, *magnPoints;

	//For Furiers series
	double *aCoeficients;
	double *bCoeficients;
	int numberOfHarmonics;

	//For animation
	int currentShift;
	int animationSpeed;

	//Graphics variables
	//Drawing canvas coordinates
	int startX, endX, startY, endY;
	//On grid step to draw lines
	int xStep, yStep;
	//Scaling coeficients
	double xScaling, yScaling;
	//Pens for different visuals
	QPen gridPen, functionPen;

	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	//Updater for values
	void updateValues();

	//Calculating functions
	void tabulateFunc(std::function<double(double)> func);
	void scaleFunc();
	void tabulateMagnitudes(double *aParams, double *bParams);
	void scaleMagnitudes();

	//Painting functions
	void drawGrid(QPainter *painter);
	void drawFunc(QPainter *painter);
	void animateFunc(QPainter *painter);
	void drawFreqDomainSig(QPainter *painter);

	//On paint event
	virtual void paintEvent(QPaintEvent*);


  private slots:
	void on_DrawButton_clicked();

	void on_UseFouriersRadioButton_clicked(bool checked);

	void on_UseFunctionRadioButton_clicked(bool checked);

	void on_ExportButton_clicked();

	void on_ImportButton_clicked();

	void on_checkBox_stateChanged(int arg1);

	void on_AnimSpeedHorizontalSlider_valueChanged(int value);

  private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
