#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QScrollBar>

#include "mainwindow.h"
#include "ClassRepository/Tools/DimensionTool.h"

class ViewWidget : public QGraphicsView
{
	Q_OBJECT
public:
	ViewWidget(QWidget *parent = nullptr);
	~ViewWidget();

	//file operations
	void loadFromFile(QString fileContents);
    void saveToFile(QString file);

    //axis
	Line *getAxisLine(int xDirection, int yDirection);

private:
	QGraphicsScene *sketchScene;

	//tools processing
	Point *mousePoint;

	DrawablesFactory *objectFactory;

	//tools
	Tool *selectedTool;
	QPen defaultPen;
	QBrush defaultBrush;

    //object managment
	QVector<DrawableObject*> *objectsInSketch;

	Line *upUpAxis, *upDownAxis, *downDownAxis, *downUpAxis;

	//draging
	int prevX, prevY;

	//initialisation
	void initializeTools();
	void initializeScene();

	//events
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void keyPressEvent(QKeyEvent *event);

signals:
    void keyPressed(QKeyEvent *event);

public slots:
	void setTool(QString toolName);
	void resetTool();

};

#endif // DRAWINGWIDGET_H
