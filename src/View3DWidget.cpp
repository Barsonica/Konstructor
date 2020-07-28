#include "View3DWidget.h"

View3DWidget::View3DWidget(QFrame *frame) : QOpenGLWidget(frame)
{
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3,3);
	setFormat(format);

	connect(DrawTool::getInstance(), &DrawTool::addItem,
			this, &View3DWidget::addItem
			);

	setFocus();
}

void View3DWidget::allocateNewItem(Item *item)
{
	int newItemIndex = vertexData.size();
	vertexData.resize(newItemIndex + item->size());

	std::vector<Vertex*> itemData;
	for(int i = newItemIndex; i < vertexData.size(); i++)
	{
		itemData.push_back(&vertexData[i]);
	}

	item->setVectorReference(itemData, newItemIndex);

	vertexBuffer.bind();
	vertexBuffer.allocate(vertexData.data(), vertexData.size() * sizeof(Vertex));
	vertexBuffer.release();

	update();
}

void View3DWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
	QOpenGLWidget::mousePressEvent(event);
}

void View3DWidget::mouseReleaseEvent(QMouseEvent *event)
{
	lastPos = event->pos();
	QOpenGLWidget::mouseReleaseEvent(event);
}

void View3DWidget::mouseMoveEvent(QMouseEvent *event)
{
	QOpenGLWidget::mouseMoveEvent(event);

	static const float rotSpeed   = 0.1f;

	if(event->buttons() == Qt::RightButton)
	{
		// Handle rotations
		camera.rotate(rotSpeed * (lastPos.x() - event->x()), Camera::LocalUp);
		camera.rotate(rotSpeed * (lastPos.y() - event->y()), camera.right());

		// redraw
		QOpenGLWidget::update();
	}

	lastPos = event->pos();
}

void View3DWidget::wheelEvent(QWheelEvent *event)
{
	QOpenGLWidget::wheelEvent(event);
}

void View3DWidget::keyPressEvent(QKeyEvent *event)
{
	QOpenGLWidget::keyPressEvent(event);

	static const float transSpeed = 0.05f;

	// Handle translations
	QVector3D translation;
	if (event->key() == Qt::Key_W)
	{
	  translation += camera.forward();
	}
	if (event->key() == Qt::Key_S)
	{
	  translation -= camera.forward();
	}
	if (event->key() == Qt::Key_A)
	{
	  translation -= camera.right();
	}
	if (event->key() == Qt::Key_D)
	{
	  translation += camera.right();
	}
	if (event->key() == Qt::Key_Q)
	{
	  translation -= camera.up();
	}
	if (event->key() == Qt::Key_E)
	{
	  translation += camera.up();
	}
	camera.translate(transSpeed * translation);

	// redraw
	QOpenGLWidget::update();
}

void View3DWidget::initializeGL()
{
	initializeOpenGLFunctions();
	//glEnable(GL_CULL_FACE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/src/shaders/simple.vert");
	program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/shaders/simple.frag");
	program.link();
	program.bind();

	itemToSpace = program.uniformLocation("itemToSpace");
	worldToCamera = program.uniformLocation("worldToCamera");
	cameraToView = program.uniformLocation("cameraToView");

	vertexBuffer.create();
	vertexBuffer.bind();
	vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vertexBuffer.allocate(vertexData.data(), vertexData.size() * sizeof(Vertex));

	vertexBufferObject.create();
	vertexBufferObject.bind();

	program.enableAttributeArray(0);
	program.enableAttributeArray(1);
	program.setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
	program.setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

	vertexBufferObject.release();
	vertexBuffer.release();

	program.release();
}

void View3DWidget::resizeGL(int width, int height)
{
	projection.setToIdentity();
	projection.perspective(45.0f, width / float(height), 0.0f, 1000.0f);
}

void View3DWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	program.bind();
	program.setUniformValue(worldToCamera, camera.toMatrix());
	program.setUniformValue(cameraToView, projection);

	vertexBufferObject.bind();
	foreach (Item *item, objectsInSpace)
	{
		program.setUniformValue(itemToSpace, item->toMatrix());
		glDrawArrays(GL_TRIANGLES, item->getItemIndex(), item->size());
	}
	vertexBufferObject.release();

	program.release();

}

void View3DWidget::addItem(Item *item)
{
	item->setText(0, "object " + QString::number(objectsInSpace.size()));

	objectsInSpace.append(item);
	connect(item, &Item::sizeChanged,
			this, &View3DWidget::reallocateMemory
			);

	allocateNewItem(item);
}

void View3DWidget::reallocateMemory()
{
	vertexData.clear();

	foreach(Item *item, objectsInSpace)
	{
		allocateNewItem(item);
	}
}
