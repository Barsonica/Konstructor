#include "DrawablesFactory.h"

DrawablesFactory *DrawablesFactory::instance = nullptr;

void DrawablesFactory::initialise(QBrush *defaultBrush,
									QPen *defaultPen,
									QVector<DrawableObject*> *objectList,
									QGraphicsScene *scene
									)
{
	if(DrawablesFactory::instance == nullptr)
	{
		DrawablesFactory::instance = new DrawablesFactory(defaultBrush,
														  defaultPen,
														  objectList,
														  scene
														  );
	}
}

DrawablesFactory *DrawablesFactory::getInstance()
{
	return DrawablesFactory::instance;
}

//----------     object creation     ---------

Point *DrawablesFactory::makePoint()
{
	Point *point = new Point();
	point->setBrush(this->defaultBrush)
			->setPen(this->defaultPen);

	return point;
}

Point *DrawablesFactory::makePoint(double x, double y)
{
	Point *point = makePoint();
	point->setLocation(x, y);

	return point;
}

Line *DrawablesFactory::makeLine(Point *startPoint, Point *endPoint)
{
	Line *line = new Line(startPoint, endPoint);
	line->setBrush(this->defaultBrush)
			->setPen(this->defaultPen);

	return line;
}

Circle *DrawablesFactory::makeCircle(Point *centerPoint)
{
	Circle *circle = new Circle(centerPoint);
	circle->setBrush(this->defaultBrush)
			->setPen(this->defaultPen);

	return circle;
}


Circle *DrawablesFactory::makeCircle(Point *centerPoint, double radius)
{
	Circle *circle = makeCircle(centerPoint);
	circle->setRadius(radius);

	return circle;
}

Circle *DrawablesFactory::makeCircle(Point *centerPoint, DrawableObject *liesOn)
{
	Circle *circle = makeCircle(centerPoint);
	circle->setRelationLiesOn(liesOn);

	return circle;
}

//----------     dimension creation     ---------

LineLengthDimension *DrawablesFactory::makeLineLengthDimension(Line *line)
{
	LineLengthDimension *dimension = new LineLengthDimension(line);
	dimension->setPen(this->defaultPen)
			->setBrush(this->defaultBrush);

	QObject::connect(dimension, &UserInputRequester::requestDouble,
					 this->userInput, &QGraphicsViewUserInput::requestDouble
					 );

	return dimension;
}

LineLengthDimension *DrawablesFactory::makeLineLengthDimension(Line *line, double length)
{
	LineLengthDimension *dimension = makeLineLengthDimension(line);
	dimension->setLineLength(length);

	return dimension;
}

LineLengthDimension *DrawablesFactory::makeLineLengthDimension(Line *line, double length, double distanceFromLine)
{
	LineLengthDimension *dimension = makeLineLengthDimension(line, length);
	dimension->setDistanceFromLine(distanceFromLine);

	return dimension;
}

LinesAngleDimension *DrawablesFactory::makeLinesAngleDimension(Line *lines[])
{
	LinesAngleDimension *dimension = new LinesAngleDimension(lines);
	dimension->setPen(this->defaultPen)
			->setBrush(this->defaultBrush);

	QObject::connect(dimension, &UserInputRequester::requestDouble,
					 this->userInput, &QGraphicsViewUserInput::requestDouble
					 );

	return dimension;
}

LinesAngleDimension *DrawablesFactory::makeLinesAngleDimension(Line *lines[], double distanceFromCenter)
{
	LinesAngleDimension *dimension = makeLinesAngleDimension(lines);
	dimension->setDistanceFromCenter(distanceFromCenter);

	return dimension;
}

//----------     object managment     ---------

void DrawablesFactory::addDrawable(DrawableObject *object)
{
	object->setPen(this->defaultPen)
			->setBrush(this->defaultBrush);

	if(!this->objectList->contains(object))
	{
		object->setFlags(QGraphicsItem::ItemIsSelectable);
		object->setAcceptHoverEvents(true);
		this->objectList->append(object);
		this->scene->addItem(object);

		object->setId(this->idCounter);
		this->idCounter++;
	}
}

void DrawablesFactory::tryDeleteDrawable(DrawableObject *object)
{
	if(!this->objectList->contains(object))
		deleteDrawable(object);
}

void DrawablesFactory::deleteDrawable(DrawableObject *object)
{
	removeDrawable(object);

	if(object != nullptr)
	{
		delete object;
		object = nullptr;
	}
}

void DrawablesFactory::removeDrawable(DrawableObject *object)
{
	if(objectList->contains(object))
		this->objectList->removeAll(object);
	this->scene->removeItem(object);
}


//----------     _     ---------

DrawablesFactory::DrawablesFactory(QBrush *defaultBrush,
								   QPen *defaultPen,
								   QVector<DrawableObject*> *objectList,
								   QGraphicsScene *scene
								   )
{
	this->defaultBrush = defaultBrush;
	this->defaultPen = defaultPen;
	this->objectList = objectList;
	this->scene = scene;

	QGraphicsViewUserInput::initialize(scene);
	this->userInput = QGraphicsViewUserInput::getInstance();
}
