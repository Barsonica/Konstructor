#include "Circle.h"

Circle::Circle() : DrawableObject (Type_Circle){}

Circle::Circle(Point *center_point) : DrawableObject (Type_Circle)
{
	this->centerPoint = center_point;
}

void Circle::resolveTies()
{
	if(this->liesOn != nullptr)
	{
		if(this->liesOn->getType() == Type_Point)
		{
			Point *liesOn = dynamic_cast<Point*>(this->liesOn);

			this->radius = this->centerPoint->distanceFrom(liesOn->getLocation());
		}
	}
}

//----------	file handling    ----------

void Circle::loadVariables(QString input)
{
	QStringList varNames = {
		"radius"
    };

	QVector<QVariant> variables = fetchVariables(input, varNames);

	this->radius = variables[0].toDouble();
}

QString Circle::toFileString()
{
	DrawableObject::toFileString();
	this->fileAddVar("centerPoint", this->centerPoint->getId());
	this->fileAddVar("radius", this->radius);
	return this->fileFinish();
}

void Circle::loadRelations(QVector<DrawableObject*> *list)
{
	QStringList varNames = {
		"centerPoint"
    };

	QVector<DrawableObject*> values = fetchRelations(list, varNames);

	this->centerPoint = dynamic_cast<Point*>(values[0]);
}

//----------	getters and setters    ----------

Point *Circle::getCenterPoint()
{
	return this->centerPoint;
}

double *Circle::getRadius()
{
	return &this->radius;
}

Circle *Circle::setRadius(double value)
{
	this->radius = value;

	return this;
}

Circle *Circle::clone()
{
	Circle *circle = new Circle(this->centerPoint);
	circle->setRadius(this->radius);
	circle->setRelationLiesOn(this->liesOn);

	return circle;
}

//----------	Relations    ----------

void Circle::setRelationLiesOn(DrawableObject *object)
{
	this->liesOn = object;
}

//----------	QGraphicsItem overrides    ----------

QRectF Circle::boundingRect() const
{
	QPointF top_left_point(
				this->centerPoint->getLocation() -
				QPointF(radius, radius)
				);

	return QRectF(top_left_point,
			  QSizeF(this->radius + radius,
					 this->radius + radius
					 )
			  );
}

QPainterPath Circle::shape() const
{
	QPainterPath path;
	path.addEllipse(boundingRect() + QMargins(1,1,1,1));

	QPainterPath smaller;
	smaller.addEllipse(boundingRect() - QMargins(2,2,2,2));

	return path.subtracted(smaller);
}

void Circle::paint(QPainter *painter,
				   const QStyleOptionGraphicsItem *option,
				   QWidget *widget)
{
	if(this->isHidden())
		return;

	DrawableObject::paint(painter, option, widget);

	painter->drawEllipse(this->boundingRect());
}
