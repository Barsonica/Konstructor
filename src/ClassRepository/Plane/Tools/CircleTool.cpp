#include "CircleTool.h"

CircleTool *CircleTool::instance = nullptr;

CircleTool::CircleTool(Point *mousePoint, QGraphicsScene *scene)
{
	this->objectFactory = Factory::getInstance();

	this->circlePreviewCentre = this->objectFactory->makePoint();
	this->circlePreview = this->objectFactory
			->makeCircle(this->circlePreviewCentre, mousePoint);
	this->circlePreview->setHidden(true);
	this->circlePreview->setAcceptedMouseButtons(Qt::MouseButton::NoButton);
	this->circlePreview->setAcceptHoverEvents(false);

	scene->addItem(this->circlePreview);
	this->previousClickedPoint = nullptr;
}

void CircleTool::initialize(Point *mousePoint,
							QGraphicsScene *scene,
							QBrush *defaultBrush,
							QPen *defaultPen)
{
	if(CircleTool::instance == nullptr)
		CircleTool::instance = new CircleTool(mousePoint, scene);

	CircleTool::instance->setCurrentPen(defaultPen);
	CircleTool::instance->setCurrentBrush(defaultBrush);
}

CircleTool *CircleTool::getInstance()
{
	return CircleTool::instance;
}

void CircleTool::click(DrawableObject *clickedObject, Point *mousePoint)
{
	if(this->previousClickedPoint != nullptr)
	{
		this->objectFactory->addDrawable(this->previousClickedPoint);

		if(clickedObject == nullptr)
		{
			Point *clickedPoint = mousePoint->clone();
			Circle *circle = this->objectFactory
									->makeCircle(this->previousClickedPoint,
												this->previousClickedPoint->distanceFrom(clickedPoint->getLocation())
												);
			this->objectFactory->addDrawable(circle);
		}
		else
		{
			if(clickedObject->getType() == Global::Point)
			{
				Circle *circle = this->objectFactory
										->makeCircle(this->previousClickedPoint,
													 dynamic_cast<Point*>(clickedObject)
													 );
				this->objectFactory->addDrawable(circle);
			}
			else if (clickedObject == this->circlePreview)
			{
				Point *clickedPoint = mousePoint->clone();
				Circle *circle = this->objectFactory
										->makeCircle(this->previousClickedPoint,
													this->previousClickedPoint->distanceFrom(clickedPoint->getLocation())
													);
				this->objectFactory->addDrawable(circle);
			}
		}

		this->circlePreview->setHidden(true);
		this->previousClickedPoint = nullptr;
	}
	else
	{
		if(clickedObject == nullptr)
			clickedObject = mousePoint->clone();

		if(clickedObject->getType() != Global::Point)
		{
			clickedObject = mousePoint->clone();
		}

		Point *clickedPoint = dynamic_cast<Point*>(clickedObject);

		this->previousClickedPoint = clickedPoint;
		this->circlePreviewCentre->setLocation(clickedPoint->getLocation());
		this->circlePreview->setHidden(false);
	}
}

void CircleTool::resetTool()
{
	this->objectFactory->tryDeleteDrawable(this->previousClickedPoint);
	this->previousClickedPoint = nullptr;
	this->circlePreview->setHidden(true);
}
