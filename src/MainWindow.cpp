#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	this->ui->setupUi(this);

	this->settingsDialog = new SettingsDialog();
	this->extrusionDialog = new ExtrusionDialog();
	MessagesManager::init();

	this->setupUi();
	this->setupConnections();

	loadSettings();
}

MainWindow::~MainWindow()
{
	delete this->ui;
}

void MainWindow::show(int mode)
{
	QMainWindow::show();

	if(mode == Global::Draw)
	{
		this->setMode(Global::Mode::Draw);
		emit setTargetItem(this->ui->objectsTree->topLevelItem(0));
		this->ui->objectsTree->setCurrentItem(this->ui->objectsTree->topLevelItem(0));
	}
	else if(mode == Global::Object)
	{
		this->setMode(Global::Mode::Object);
		on_openObjectFile_clicked();
	}
}

void MainWindow::setupUi()
{
    //object menu
	this->ui->objectsTree->setHeaderHidden(true);
    ui->objectsTree->setContextMenuPolicy(Qt::CustomContextMenu);

	//base plane
	Plane *basePlane = new Plane();
	basePlane->setPosition(QVector3D(0,0,0));
	basePlane->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1,0,0), 0));
	basePlane->setText(0, "origin plane");
	basePlane->setCheckState(0,Qt::CheckState::Checked);
    this->ui->objectsTree->addTopLevelItem(basePlane);
	this->ui->spaceView->setTopPlane(basePlane);

    //context menus
	drawAction.setText(tr("Draw"));
    planeContextMenu.addAction(&drawAction);
	extrusionAction.setText(tr("Extrude"));
	objectContextMenu.addAction(&extrusionAction);
	redrawAction.setText(tr("Redraw"));
	objectContextMenu.addAction(&redrawAction);
	deleteAction.setText(tr("Delete"));
	objectContextMenu.addAction(&deleteAction);

	this->setWindowState(Qt::WindowMaximized);
}

void MainWindow::setupConnections()
{
	//forward keypresses to ViewWidget
	connect(this->ui->planeView, &PlaneWidget::keyPressed,
			this, &MainWindow::viewKeyPress
			);
	//forwards statusBar messages from ViewWidget
	connect(this->ui->planeView, &PlaneWidget::showStatusBarMessage,
			this->ui->statusBar, &QStatusBar::showMessage
			);
	//forwards statusBar messages from Tool
	connect(LinesAngleConstraintTool::getInstance(), &LinesAngleConstraintTool::showStatusBarMessage,
			this->ui->statusBar, &QStatusBar::showMessage
			);

	//tools
	connect(this, &MainWindow::setTool,
			this->ui->planeView, &PlaneWidget::setTool
			);
	connect(this, &MainWindow::resetTool,
			this->ui->planeView, &PlaneWidget::resetTool
			);

	//sketch file
	connect(this->ui->newSketchButton, &QToolButton::clicked,
			this->ui->planeView, &PlaneWidget::newSketchButtonClicked
			);
	connect(this->ui->closeSketchButton, &QToolButton::clicked,
			this->ui->planeView, &PlaneWidget::closeSketchButtonClicked
			);

	//Plane to Space connection
	//set item on which to draw
	connect(this, &MainWindow::setTargetItem,
			SpaceFactory::getInstance(), &SpaceFactory::recieveTargetItem
			);
	connect(this->ui->planeView, &PlaneWidget::closeDrawing,
			this, &MainWindow::closeDrawing
			);

	//connect extrusionDialog to screen update
	connect(extrusionDialog, &ExtrusionDialog::selectionChanged,
			this->ui->spaceView, &SpaceWidget::update
			);

	//generating drawables for loading items
	connect(SpaceFactory::getInstance(), &SpaceFactory::generatePolygons,
			this, &MainWindow::getPolygonsForItem
			);

	connect(this->settingsDialog, &SettingsDialog::saveSettings,
			this, &MainWindow::saveSettings
			);
}

//----------    Ui handeling    ---------

void MainWindow::refreshTools(int tool)
{
	if(tool != Global::Tools::LineTool)
		this->ui->lineButton->setChecked(false);
	if(tool != Global::Tools::CircleTool)
		this->ui->circleButton->setChecked(false);
	if(tool != Global::Tools::RectangleTool)
		this->ui->rectangleButton->setChecked(false);
	if(tool != Global::Tools::LabelTool)
		this->ui->labelButton->setChecked(false);
	if(tool != Global::Tools::ArcTool)
		this->ui->arcButton->setChecked(false);
	if(tool != Global::Tools::PointPositionTool)
		this->ui->pointPosButton->setChecked(false);
	if(tool != Global::Tools::LockPointTool)
		this->ui->lockPointButton->setChecked(false);
	if(tool != Global::Tools::PointDistanceConstrainTool)
		this->ui->pointDistanceConstraintButton->setChecked(false);
	if(tool != Global::Tools::CircleRadiusConstraintTool)
		this->ui->circleRadiusConstraintButton->setChecked(false);
	if(tool != Global::Tools::ExpandPolgyonTool)
		this->ui->expandPolygonButton->setChecked(false);
	if(tool != Global::Tools::LineCenterTool)
		this->ui->lineCenterButton->setChecked(false);
	if(tool != Global::Tools::ParaelLinesConstraintTool)
		this->ui->paraelLineButton->setChecked(false);
	if(tool != Global::Tools::LinesAngleConstraintTool)
		this->ui->linesAngleButton->setChecked(false);

	emit setTool(tool);
}

//----------	settings    ----------

void MainWindow::loadSettings()
{
	QString settingsFilePath = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation).at(0);
	QFile settingsFile(settingsFilePath + "/Settings.json");

	if (settingsFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		Settings::fromJson(settingsFile.readAll());
	}
}

void MainWindow::saveSettings()
{
	QString settingsFilePath = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation).at(0);
	QDir settingsFolder(settingsFilePath);
	if(!settingsFolder.exists()) settingsFolder.mkdir(settingsFolder.path());

	QFile settingsFile(settingsFilePath + "/Settings.json");
	if(settingsFile.open(QIODevice::WriteOnly))
	{
		settingsFile.write(Settings::toJson().toUtf8());
	}
}

//-----    file tab    -----


void MainWindow::on_newObjectFile_clicked()
{
	this->ui->spaceView->reset();
}

void MainWindow::on_openObjectFile_clicked()
{
	this->ui->spaceView->reset();
	QString fileName = QFileDialog::getOpenFileName(
			this,
			Global::openFile,
			Settings::userProjectRoot,
			Global::konstructorProject + ";;" + Global::allFiles
			);

	QFile file(fileName);
	if(file.exists())
	{
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
			this->ui->spaceView->loadFromFile(file.readAll());
	}
}

void MainWindow::on_saveObjectButton_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(
			this,
			Global::saveFile,
			Settings::userProjectRoot,
			Global::konstructorProject + ";;" + Global::allFiles
			);

	QStringList splitName = fileName.split('.');
	if(splitName.size() > 1)
	{
		if(splitName.at(splitName.size() -1) != "kopr")
			fileName += ".kopr";
	}
	else fileName += ".kopr";

	QFile targetFile(fileName);
	if(targetFile.open(QIODevice::WriteOnly))
	{
		targetFile.write(this->ui->spaceView->saveToFile().toUtf8());
	}
}

void MainWindow::on_exportObjectButton_clicked()
{
	QFileDialog saveDialog(this);
	saveDialog.setWindowTitle(Global::saveFile);
	saveDialog.setDirectory(Settings::userProjectRoot);
	saveDialog.setNameFilter("STL (*.stl);;OFF (*.off)");
	saveDialog.setAcceptMode(QFileDialog::AcceptSave);

	saveDialog.exec();

	QStringList files = saveDialog.selectedFiles();
	if(files.size() > 0)
	{
		QString fileName = files.at(0);


		QStringList splitName = fileName.split('.');
		if(saveDialog.selectedNameFilter().contains(".off"))
		{
			if(splitName.size() > 1)
			{
				if(splitName.at(splitName.size() -1) != "off")
					fileName += ".off";
			}
			else fileName += ".off";
		}

		if(saveDialog.selectedNameFilter().contains(".stl"))
		{
			if(splitName.size() > 1)
			{
				if(splitName.at(splitName.size() -1) != "stl")
					fileName += ".stl";
			}
			else fileName += ".stl";
		}

		this->ui->spaceView->exportToFile(fileName);
	}
}

void MainWindow::on_settingsButton_clicked()
{
	this->settingsDialog->exec();
}

void MainWindow::on_quitButton_clicked()
{
	this->close();
}

//-----    sketch files    -----

void MainWindow::on_closeSketchButton_clicked()
{
	setMode(Global::Mode::Object);
	PlaneFactory::getInstance()->deleteAll();
}

void MainWindow::on_saveSketchButton_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(
			this,
			Global::saveFile,
			Settings::userProjectRoot,
			Global::konstructorSketch + ";;" + Global::allFiles
			);

	QStringList splitName = fileName.split('.');
	if(splitName.size() > 1)
	{
		if(splitName.at(splitName.size() -1) != "kosk")
			fileName += ".kosk";
	}
	else fileName += ".kosk";

	QFile targetFile(fileName);
	if(targetFile.open(QIODevice::WriteOnly))
	{
		targetFile.write(this->ui->planeView->toFile().toUtf8());
	}
}

void MainWindow::on_openSketchButton_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(
			this,
					Global::openFile,
			Settings::userProjectRoot,
			Global::konstructorSketch + ";;" + Global::allFiles
			);

	QFile file(fileName);
	if(file.exists())
	{
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
			this->ui->planeView->loadFromFile(file.readAll());
	}
}

//-----    sketch tools    -----

void MainWindow::on_lineButton_clicked()
{
	if(this->ui->lineButton->isChecked())
		refreshTools(Global::Tools::LineTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_circleButton_clicked()
{

	if(this->ui->circleButton->isChecked())
		refreshTools(Global::Tools::CircleTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_rectangleButton_clicked()
{
	if(this->ui->rectangleButton->isChecked())
		refreshTools(Global::Tools::RectangleTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_labelButton_clicked()
{
	if(this->ui->labelButton->isChecked())
		refreshTools(Global::Tools::LabelTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_arcButton_clicked()
{
	if(this->ui->arcButton->isChecked())
		refreshTools(Global::Tools::ArcTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_expandPolygonButton_clicked()
{
	if(this->ui->expandPolygonButton->isChecked())
		refreshTools(Global::Tools::ExpandPolgyonTool);
	else
		refreshTools(Global::Tools::NoTool);
}

//-----    constrains    -----

void MainWindow::on_pointPosButton_clicked()
{
	if(this->ui->pointPosButton->isChecked())
		refreshTools(Global::Tools::PointPositionTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_lockPointButton_clicked()
{
	if(this->ui->lockPointButton->isChecked())
		refreshTools(Global::Tools::LockPointTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_pointDistanceConstraintButton_clicked()
{
	if(this->ui->pointDistanceConstraintButton->isChecked())
		refreshTools(Global::Tools::PointDistanceConstrainTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_lineCenterButton_clicked()
{
	if(this->ui->lineCenterButton->isChecked())
		refreshTools(Global::Tools::LineCenterTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_circleRadiusConstraintButton_clicked()
{
	if(this->ui->circleRadiusConstraintButton->isChecked())
		refreshTools(Global::Tools::CircleRadiusConstraintTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_paraelLineButton_clicked()
{
	if(this->ui->paraelLineButton->isChecked())
		refreshTools(Global::Tools::ParaelLinesConstraintTool);
	else
		refreshTools(Global::Tools::NoTool);
}

void MainWindow::on_linesAngleButton_clicked()
{
	if(this->ui->linesAngleButton->isChecked())
		refreshTools(Global::Tools::LinesAngleConstraintTool);
	else
		refreshTools(Global::Tools::NoTool);
}

//-----    finish drawing    -----

void MainWindow::on_finishDrawingButton_clicked()
{
	if(this->ui->objectsTree->currentItem() == nullptr)
	{
		if(MessagesManager::showOkCancelForm("No plane selected") == QDialog::Rejected)
			return;
	}

	setMode(Global::Mode::Object);

	QString sketch = this->ui->planeView->toFile();
	std::vector<QPolygonF> polygons = Polygonator::getInstance()->generatePolygonsFromDrawing(this->ui->planeView->finishDrawing());
	PlaneFactory::getInstance()->deleteAll();

	SpaceFactory::getInstance()->addNewItem(polygons, sketch);
}

//-----    misc slots    -----

void MainWindow::on_objectsTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	setMode(Global::Mode::Draw);
    emit setTargetItem(item);

    if(Item *existingItem = dynamic_cast<Item*>(item))
    {
		this->ui->planeView->loadProjected(existingItem->getParentPlane()->getProjectedPolygon());
		this->ui->planeView->loadFromFile(existingItem->getSketch());
	}
	else if(Plane *plane = dynamic_cast<Plane*>(item))
	{
		this->ui->planeView->loadProjected(plane->getProjectedPolygon());
	}
}

void MainWindow::viewKeyPress(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key::Key_Escape:
			refreshTools(Global::Tools::NoTool);
			break;
		case Qt::Key::Key_Enter:
			emit resetTool();
			break;
	}
}

void MainWindow::setMode(int index)
{
	this->ui->stackedWidget->setCurrentIndex(index);
	this->ui->statusBar->clearMessage();
}

void MainWindow::on_objectsTree_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->objectsTree->indexAt(pos);
    if (index.isValid()) {
		if(Item *item = dynamic_cast<Item*>(ui->objectsTree->itemAt(pos)))
        {
			if(item->isExtruded()) extrusionAction.setText("Change Extrusion");
			else extrusionAction.setText("Extrude");

			QAction *selectedAction = objectContextMenu.exec(ui->objectsTree->viewport()->mapToGlobal(pos));

			if(selectedAction == &extrusionAction)
			{
				item->setSelected(false);
				this->extrusionDialog->show(item);
			}
			else if(selectedAction == &redrawAction)
			{
				this->ui->planeView->loadProjected(item->getParentPlane()->getProjectedPolygon());
				setMode(Global::Mode::Draw);
				emit setTargetItem(item);

				this->ui->planeView->loadFromFile(item->getSketch());
			}
			else if(selectedAction == &deleteAction)
			{
				SpaceFactory::getInstance()->deleteItem(item);
			}
        }
		else if(Plane *plane = dynamic_cast<Plane*>(ui->objectsTree->itemAt(pos)))
        {
			QAction *selectedAction = planeContextMenu.exec(ui->objectsTree->viewport()->mapToGlobal(pos));
			if(selectedAction == &drawAction)
			{
				this->ui->planeView->loadProjected(plane->getProjectedPolygon());

				setMode(Global::Mode::Draw);
				emit setTargetItem(plane);
			}
        }
    }

	this->ui->spaceView->update();
}

void MainWindow::on_objectsTree_itemClicked(QTreeWidgetItem *item, int column)
{
	this->ui->spaceView->update();
}

std::vector<QPolygonF> MainWindow::getPolygonsForItem(QString sketch)
{
	QVector<DrawableObject*> loadedObjects = PlaneFactory::getInstance()->generateListFromSketch(sketch);

	return Polygonator::getInstance()->generatePolygonsFromDrawing(loadedObjects);
}

void MainWindow::closeDrawing()
{
	setMode(Global::Mode::Object);
}
