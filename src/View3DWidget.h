#ifndef VIEW3DWIDGET_H
#define VIEW3DWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QFrame>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include "ClassRepository/Space/Space"
#include "MessagesManager.h"

class View3DWidget : public QOpenGLWidget, private QOpenGLFunctions
{
	Q_OBJECT
public:
	View3DWidget(QFrame *frame);

	void setTopPlane(Plane *plane);

private:
	QVector<Item*> objectsInSpace;
	QMatrix4x4 projection;
	QQuaternion itemRotation;
	Camera camera;

	QOpenGLShaderProgram vertexProgram;

	//copy of all vertexes data
	std::vector<Vertex> vertexData;
	QOpenGLBuffer vertexBuffer;
	QOpenGLVertexArrayObject vertexBufferObject;

	QTreeWidgetItem *targetItem;

	//plane data
	QVector<Plane*> planes;
	std::vector<Vertex> planeVertexData;
	QOpenGLBuffer planeBuffer;
	QOpenGLVertexArrayObject planeBufferObject;


	// Shader Information
	int itemToSpace;
	int worldToCamera;
	int cameraToView;
	int itemToRotate;

	int selectedItemColor;
	int isSelected;

	int transparentColorValue;
	int selectedTransparentValue;

	QPoint lastPos;

	void allocateNewItem(Item *item);
	void generatePlaneVertexes();

	//events
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;

	void initializeGL() override;
	void resizeGL(int width, int height) override;
	void paintGL() override;

public slots:
	void addItem(std::vector<QPolygonF> polygons, QString sketch);
	void deleteItem(Item *item);
	void recieveTargetItem(QTreeWidgetItem *item);
	void reallocateMemory();
	void update();
	void addPlane(Plane *plane);
	void removePlane(Plane *plane);
};

inline void View3DWidget::update() { QOpenGLWidget::update(); }

#endif // VIEW3DWIDGET_H
