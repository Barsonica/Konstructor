#include "View3DWidget.h"

View3DWidget::View3DWidget(QFrame *frame) : QOpenGLWidget(frame)
{
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3,3);
	setFormat(format);

	camera.translate(-camera.forward()*Settings::planeToSpaceRatio);
	itemRotation.setScalar(1);

	setFocus();
}

void View3DWidget::setTopPlane(Space::Plane *plane)
{
	planes.push_back(plane);
	generatePlaneVertexes();
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

	std::vector<Vertex> globalbuffer;
	for(uint32_t i = 0; i < planeVertexData.size(); i++)
	{
		globalbuffer.push_back(planeVertexData.at(i));
	}
	for(uint32_t i = 0; i < vertexData.size(); i++)
	{
		globalbuffer.push_back(vertexData.at(i));
	}

	vertexBuffer.bind();
	vertexBuffer.allocate(globalbuffer.data(), globalbuffer.size() * sizeof(Vertex));
	vertexBuffer.release();

	update();
}

void View3DWidget::generatePlaneVertexes()
{
	planeVertexData.clear();
	QVector3D planeColor = QVector3D(1, 0, 0);
	double planeSize = 20;

	for(uint32_t i = 0; i < planes.size(); i++ )
	{
		planeVertexData.push_back(
					Vertex(QVector3D(planeSize, planeSize, 0), planeColor)
					);
		planeVertexData.push_back(
					Vertex(QVector3D(planeSize, -planeSize, 0), planeColor)
					);
		planeVertexData.push_back(
					Vertex(QVector3D(-planeSize, -planeSize, 0), planeColor)
					);
		planeVertexData.push_back(
					Vertex(QVector3D(planeSize, planeSize, 0), planeColor)
					);
		planeVertexData.push_back(
					Vertex(QVector3D(-planeSize, planeSize, 0), planeColor)
					);
		planeVertexData.push_back(
					Vertex(QVector3D(-planeSize, -planeSize, 0), planeColor)
					);
	}
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

	static const float rotSpeed   = 0.01f;

	if(event->buttons() == Qt::RightButton)
	{
		// Handle rotations
		itemRotation.setX(itemRotation.x() + (lastPos.y() - event->y()));
		itemRotation.setY(itemRotation.y() + (lastPos.x() - event->x()));

		float scalar = std::sqrt((event->x() * event->x()) + (event->y() * event->y())) * 0.0001;
		itemRotation.setScalar(itemRotation.scalar() + scalar);

		// redraw
		QOpenGLWidget::update();
	}

	lastPos = event->pos();
}

void View3DWidget::wheelEvent(QWheelEvent *event)
{
	QOpenGLWidget::wheelEvent(event);

	QVector3D translation = camera.forward() * event->delta();
	static const float transSpeed = 0.05f;
	camera.translate(transSpeed * translation);

	// redraw
	QOpenGLWidget::update();
}

void View3DWidget::keyPressEvent(QKeyEvent *event)
{
	QOpenGLWidget::keyPressEvent(event);
}

void View3DWidget::initializeGL()
{
	initializeOpenGLFunctions();
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);


	vertexProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/src/shaders/simple.vert");
	vertexProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/shaders/simple.frag");
	vertexProgram.link();
	vertexProgram.bind();

	itemToSpace = vertexProgram.uniformLocation("itemToSpace");
	worldToCamera = vertexProgram.uniformLocation("worldToCamera");
	cameraToView = vertexProgram.uniformLocation("cameraToView");
	itemToRotate = vertexProgram.uniformLocation("itemToRotate");

	selectedItemColor = vertexProgram.uniformLocation("selectedItemColor");
	itemIsSelected = vertexProgram.uniformLocation("itemIsSelected");
	polygonIsSelected = vertexProgram.uniformLocation("polygonIsSelected");

	transparentColorValue = vertexProgram.uniformLocation("transparentColorValue");

	vertexBuffer.create();
	vertexBuffer.bind();
	vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vertexBuffer.allocate(vertexData.data(), vertexData.size() * sizeof(Vertex));

	vertexBufferObject.create();
	vertexBufferObject.bind();

	vertexProgram.enableAttributeArray(0);
	vertexProgram.enableAttributeArray(1);
	vertexProgram.setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
	vertexProgram.setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

	vertexBufferObject.release();
	vertexBuffer.release();

	vertexProgram.release();
}

void View3DWidget::resizeGL(int width, int height)
{
	projection.setToIdentity();
	projection.perspective(45.0f, width / float(height), 0.0f, 1000.0f);
}

void View3DWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vertexProgram.bind();
	vertexProgram.setUniformValue(worldToCamera, camera.toMatrix());
	vertexProgram.setUniformValue(cameraToView, projection);
	vertexProgram.setUniformValue(selectedItemColor, Settings::selectedFaceColor);

	QMatrix4x4 mtr;
	mtr.rotate(itemRotation.normalized());
	vertexProgram.setUniformValue(itemToRotate, mtr);

	vertexBufferObject.bind();
	//glClear(GL_DEPTH_BUFFER_BIT);

	//draw planes

	vertexProgram.setUniformValue(itemIsSelected, false);
	vertexProgram.setUniformValue(polygonIsSelected, false);
	vertexProgram.setUniformValue(transparentColorValue, 0.3f);

	int planeIndex = 0;
	for(uint32_t i = 0; i < planes.size(); i++)
	{
		vertexProgram.setUniformValue(itemToSpace, planes.at(i)->toMatrix());
		glDrawArrays(GL_TRIANGLES, planeIndex, 6);
		planeIndex += 6;
	}

	vertexProgram.setUniformValue(transparentColorValue, 1.0f);
	foreach (Item *item, objectsInSpace)
	{
		int currentIndex = item->getItemIndex() + planeIndex;
		vertexProgram.setUniformValue(itemToSpace, item->toMatrix());
		vertexProgram.setUniformValue(itemIsSelected, item->isSelected());

		for(uint32_t i = 0; i < item->getPolygons()->size(); i++)
		{
			vertexProgram.setUniformValue(polygonIsSelected, item->getPolygons()->at(i)->isSelected());
			glDrawArrays(GL_TRIANGLES, currentIndex, item->getPolygons()->at(i)->size());
			currentIndex += item->getPolygons()->at(i)->size();
		}
	}
	vertexBufferObject.release();

	vertexProgram.release();
}

void View3DWidget::addItem(std::vector<QPolygonF> polygons, QString sketch)
{
	if(Space::Plane *plane = dynamic_cast<Space::Plane*>(targetItem))
	{
		Item *item = new Item(plane, polygons, sketch);

		item->setText(0, "object " + QString::number(objectsInSpace.size()));

		objectsInSpace.append(item);
		connect(item, &Item::updateData,
				this, &View3DWidget::reallocateMemory
				);

		allocateNewItem(item);
	}
	else if(Item *item = dynamic_cast<Item*>(targetItem))
	{
		item->setPolygons(polygons);
	}
}

void View3DWidget::recieveTargetItem(QTreeWidgetItem *item)
{
	targetItem = item;
}

void View3DWidget::reallocateMemory()
{
	vertexData.clear();

	foreach(Item *item, objectsInSpace)
	{
		allocateNewItem(item);
	}
}
