#include "QXOutlineDialog.h"
#include "ui_QXOutlineDialog.h"

QXOutlineDialog::QXOutlineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QXOutlineDialog)
{
    ui->setupUi(this);

    connect(ui->contoursCheckBox, &QCheckBox::stateChanged, this, &QXOutlineDialog::updateComponents);
    connect(ui->pointsCheckBox, &QCheckBox::stateChanged, this, &QXOutlineDialog::updateComponents);
    connect(ui->sketchCheckBox, &QCheckBox::stateChanged, this, &QXOutlineDialog::updateComponents);
    connect(ui->gridsCheckBox, &QCheckBox::stateChanged, this, &QXOutlineDialog::updateComponents);
    connect(ui->emSquareCheckBox, &QCheckBox::stateChanged, this, &QXOutlineDialog::updateComponents);

}

QXOutlineDialog::~QXOutlineDialog()
{
    delete ui;
}

QXOutlineWidget*
QXOutlineDialog::outlineWidget() const {
    return ui->widget;
}

void
QXOutlineDialog::updateComponents() {
    QXOutlineWidget::Components comps{};
    if (ui->contoursCheckBox->isChecked())
        comps |= QXOutlineWidget::kContours;
    if (ui->pointsCheckBox->isChecked())
        comps |= QXOutlineWidget::kPoints;
    if (ui->sketchCheckBox->isChecked())
        comps |= QXOutlineWidget::kSketch;
    if (ui->gridsCheckBox->isChecked())
        comps |= QXOutlineWidget::kGrids;
    if (ui->emSquareCheckBox->isChecked())
        comps |= QXOutlineWidget::kEmSquare;

    outlineWidget()->setComponents(comps);

}