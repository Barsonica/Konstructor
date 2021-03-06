#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <QString>
#include <QDir>

#define POINT_Z_LEVEL 1

namespace Global
{

enum Types{
	Point,
	Line,
	Circle,
	Arc,
	Label,
	CircleRadiusConstraint,
	CirclesRadiusDifferenceConstraint,
	PointDistanceConstraint,
	LineCenterPointConstraint,
	ParaelLinesConstraint,
	LinesAngleConstraint
};

enum Tools{
	NoTool,
	LineTool,
	CircleTool,
	RectangleTool,
	LabelTool,
	ArcTool,
	ExpandPolgyonTool,
	LineCenterTool,
	PointPositionTool,
	LockPointTool,

	PointDistanceConstrainTool,
	CircleRadiusConstraintTool,
	ParaelLinesConstraintTool,
	LinesAngleConstraintTool
};

enum Mode{
	Draw,
	Object
};

	//messages
	const QString openFile = "Open File";
	const QString saveFile = "Save File";

	//file types
	const QString konstructorSketch = "Konstructor sketch (*.kosk)";
	const QString konstructorProject = "Konstructor project (*.kopr)";
	const QString allFiles;
}

#endif // GLOBALVARIABLES_H
