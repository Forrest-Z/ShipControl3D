/*****************************************************************
**				Project:	ShipControl(WOPC)					**
**				Author:		Dong Shengwei						**
**				Library:	BestSea								**
**				Date:		2014-01-04							**
******************************************************************/

//mainwindow.cpp

#include "ShipGraph.h"
#include "DataStruct.h"
#include "ShipControl.h"
#include "ShipParameter.h"
#include "OptionDialog.h"
#include "mainwindow.h"
#include <QtWidgets>
#include <QTimer>
#include <QTextCodec>
#include <QMetaType>
#include <iostream>
using namespace std;

MainWindow::MainWindow()
{
	//qRegisterMetaType<DataSetStruct>("DataSetStruct");

    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

	timer = new QTimer(this);

	optionDlg = new OptionDialog;

    shipPara = new ShipParameter;
    shipCtrl = new ShipControl;

    shipPara->readMat("E:/Program/matlab_program/s175.mat");

    Data *shipData = new Data;
    shipData = &(shipPara->getData());

    shipCtrl->setData(shipData);
	shipCtrl->setParameter();

    shipWidget = new ShipGraph;

    glWidgetArea = new QScrollArea;
    glWidgetArea->setWidget(shipWidget);
    glWidgetArea->setWidgetResizable(true);
    glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    glWidgetArea->setMinimumSize(50, 50);

    xSlider = createSlider(SIGNAL(xRotationChanged(int)),
                           SLOT(setXRotation(int)));
    ySlider = createSlider(SIGNAL(yRotationChanged(int)),
                           SLOT(setYRotation(int)));
    zSlider = createSlider(SIGNAL(zRotationChanged(int)),
						   SLOT(setZRotation(int)));

	xSlider->setRange(90*16, 270*16);
	ySlider->setRange(90*16, 270*16);

	zoomSlider = new QSlider(Qt::Vertical);
	zoomSlider->setRange(1*16, 50 * 16);
	zoomSlider->setSingleStep(16);
	zoomSlider->setPageStep(10*16);
	zoomSlider->setTickInterval(10*16);
	zoomSlider->setTickPosition(QSlider::TicksRight);
	connect(zoomSlider, SIGNAL(valueChanged(int)), shipWidget, SLOT(setZoom(int)));
	connect(shipWidget, SIGNAL(zoomChanged(int)), zoomSlider, SLOT(setValue(int)));

	startButton = new QPushButton("Start");
	connect(startButton, SIGNAL(clicked()), this, SLOT(controlStart()));

	stopButton = new QPushButton("Pause");
	connect(stopButton, SIGNAL(clicked()), this, SLOT(controlStop()));

	connect(timer, SIGNAL(timeout()), this, SLOT(updateShip()));

	connect(optionDlg, SIGNAL(dataChanged(DataSetStruct)), shipCtrl, SLOT(receivDataSet(DataSetStruct)));
	connect(shipCtrl, SIGNAL(sendDataSet(DataSetStruct)) , optionDlg, SLOT(dataInit(DataSetStruct)));
	connect(shipCtrl, SIGNAL(runState(bool)), optionDlg, SLOT(shipControlRun(bool)));

    createActions();
    createMenus();

    QGridLayout *centralLayout = new QGridLayout;
	centralLayout->addWidget(glWidgetArea, 0, 0, 3, 3);
	centralLayout->addWidget(startButton, 0, 3, 1, 1);
	centralLayout->addWidget(stopButton, 1, 3, 1, 1);
	centralLayout->addWidget(zoomSlider, 2, 3, 1, 1);
    centralLayout->addWidget(xSlider, 3, 0, 1, 4);
    centralLayout->addWidget(ySlider, 4, 0, 1, 4);
    centralLayout->addWidget(zSlider, 5, 0, 1, 4);
    centralWidget->setLayout(centralLayout);

    xSlider->setValue(150 * 16);
    ySlider->setValue(200 * 16);
	zSlider->setValue(150 * 16);
	zoomSlider->setValue(10 * 16);

//    QTextCodec *codec = QTextCodec::codecForName("GB18030");
//    setWindowTitle(codec->toUnicode("船舶模拟�?));
    resize(500, 600);

	stopButton->setDisabled(true);

	//��ʼ��Ϊ�������Ƴ�������
	firstRun = true;
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About ShipControl(3D)"),
            tr("The <b>ShipControl 3D</b> is designed by Dong Shengwei."));
}

//ֹͣ��������
void MainWindow::controlStart()
{
	if (firstRun)
	{
		shipCtrl->start();
		timer->start(20);
		firstRun = false;
	} else {
		shipCtrl->startRun();
	}
	
	startButton->setDisabled(true);
	stopButton->setEnabled(true);

	emit shipCtrl->runState(true);
}

//ֹͣ��������
void MainWindow::controlStop()
{
	shipCtrl->stopRun();
	stopButton->setDisabled(true);
	startButton->setEnabled(true);

	emit shipCtrl->runState(false);
}

//���´���λ����̬
void MainWindow::updateShip()
{
	shipWidget->shipEta(shipCtrl->getEta());
}

void MainWindow::createActions()
{
    setDialogAct = new QAction(tr("Op&tion..."), this);
    setDialogAct->setShortcut(tr("Ctrl+T"));
    connect(setDialogAct, SIGNAL(triggered()),
            optionDlg, SLOT(show()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(setDialogAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

QSlider *MainWindow::createSlider(const char *changedSignal,
                                  const char *setterSlot)
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    connect(slider, SIGNAL(valueChanged(int)), shipWidget, setterSlot);
    connect(shipWidget, changedSignal, slider, SLOT(setValue(int)));
    return slider;
}
