#include "ExtrusionDialog.h"
#include "ui_ExtrusionDialog.h"

ExtrusionDialog::ExtrusionDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ExtrusionDialog)
{
	ui->setupUi(this);

	connect(this->ui->polygonsList, &QListWidget::itemSelectionChanged,
			this, &ExtrusionDialog::selectionChanged
			);
	connect(this, &QDialog::finished,
			this, &ExtrusionDialog::finished
			);
}

ExtrusionDialog::~ExtrusionDialog()
{
	delete ui;
}

void ExtrusionDialog::show(Item *item)
{
	referencedItem = item;
	for(int i = 0; i < referencedItem->getPolygons()->size(); i++)
	{
		this->ui->polygonsList->addItem(referencedItem->getPolygons()->at(i));
	}

	if(item->isExtruded())
	{
		ui->lengthInput->setValue(item->getExtrusion().length * Settings::planeToSpaceRatio);
		ui->additiveButton->setChecked(item->getExtrusion().additive);

		ExtrusionDirection direction = item->getExtrusion().direction;
		if(direction == ExtrusionDirection::Front) ui->frontButton->setChecked(true);
		if(direction == ExtrusionDirection::FrontAndBack) ui->frontAndBackButton->setChecked(true);
		if(direction == ExtrusionDirection::Back) ui->backButton->setChecked(true);
	}

	QDialog::show();
}

void ExtrusionDialog::accept()
{
	ExtrusionDirection direction = ExtrusionDirection::Front;
	if(ui->frontAndBackButton->isChecked()) direction = ExtrusionDirection::FrontAndBack;
	if(ui->backButton->isChecked()) direction = ExtrusionDirection::Back;

	std::vector<Polygon*> polygons;
	for(int i = 0; i < referencedItem->getPolygons()->size(); i++)
	{
		Polygon *polygon = referencedItem->getPolygons()->at(i);
		if(polygon->isSelected())
		{
			Extrusion extrusion;
			extrusion.length = ui->lengthInput->value() / Settings::planeToSpaceRatio;
			extrusion.additive = ui->additiveButton->isChecked();
			extrusion.direction = direction;

			referencedItem->setExtrusion(extrusion, polygon);
		}
	}

	QDialog::accept();
}

void ExtrusionDialog::finished(int result)
{
	for(int i = 0; i < referencedItem->getPolygons()->size(); i++)
	{
		Polygon *polygon = referencedItem->getPolygons()->at(i);
		polygon->setSelected(false);
		this->ui->polygonsList->takeItem(ui->polygonsList->row(polygon));
	}
}
