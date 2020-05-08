/*

Pencil - Traditional Animation Software
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "displayoptionwidget.h"
#include "ui_displayoption.h"

#include <QToolButton>
#include <QGridLayout>

#include "preferencemanager.h"
#include "viewmanager.h"
#include "scribblearea.h"
#include "editor.h"
#include "util.h"
#include "flowlayout.h"


DisplayOptionWidget::DisplayOptionWidget(QWidget *parent) :
    BaseDockWidget(parent),
    ui(new Ui::DisplayOption)
{
    ui->setupUi(this);
}

DisplayOptionWidget::~DisplayOptionWidget()
{
    delete ui;
}

void DisplayOptionWidget::initUI()
{
    updateUI();
    anglePreferences();
    makeConnections();

    delete ui->innerWidget->layout();

    FlowLayout *layout = new FlowLayout;
    layout->setAlignment(Qt::AlignHCenter);
    layout->addWidget(ui->mirrorButton);
    layout->addWidget(ui->mirrorVButton);
    layout->addWidget(ui->thinLinesButton);
    layout->addWidget(ui->outLinesButton);
    layout->addWidget(ui->overlayCenterButton);
    layout->addWidget(ui->overlayThirdsButton);
    layout->addWidget(ui->overlayGoldenRatioButton);
    layout->addWidget(ui->overlaySafeAreaButton);

    ui->innerWidget->setLayout(layout);

#ifdef __APPLE__
    // Mac only style. ToolButtons are naturally borderless on Win/Linux.
    QString stylesheet =
        "QToolButton { border: 0px; } "
        "QToolButton:pressed{ border: 1px solid #FFADAD; border-radius: 2px; background-color: #D5D5D5; }"
        "QToolButton:checked{ border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }";

    ui->mirrorButton->setStyleSheet(stylesheet);
    ui->mirrorVButton->setStyleSheet(stylesheet);
    ui->thinLinesButton->setStyleSheet(stylesheet);
    ui->outLinesButton->setStyleSheet(stylesheet);
    ui->overlayCenterButton->setStyleSheet(stylesheet);
    ui->overlayThirdsButton->setStyleSheet(stylesheet);
    ui->overlayGoldenRatioButton->setStyleSheet(stylesheet);
    ui->overlaySafeAreaButton->setStyleSheet(stylesheet);
#endif
}

void DisplayOptionWidget::makeConnections()
{    
    connect(ui->mirrorButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleMirror);
    connect(ui->mirrorVButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleMirrorV);
    connect(ui->overlayCenterButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayCenter);
    connect(ui->overlayThirdsButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayThirds);
    connect(ui->overlayGoldenRatioButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayGoldenRatio);
    connect(ui->overlaySafeAreaButton, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlaySafeAreas);
    connect(ui->overlayPerspective1Button, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayPerspective1);
    connect(ui->overlayPerspective2Button, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayPerspective2);
    connect(ui->overlayPerspective3Button, &QToolButton::clicked, this, &DisplayOptionWidget::toggleOverlayPerspective3);

    PreferenceManager* prefs = editor()->preference();
    ScribbleArea* pScriArea = editor()->getScribbleArea();

    connect(ui->thinLinesButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleThinLines);
    connect(ui->outLinesButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOutlines);
    connect(prefs, &PreferenceManager::optionChanged, this, &DisplayOptionWidget::updateUI);

    ViewManager* view = editor()->view();
    connect(view, &ViewManager::viewFlipped, this, &DisplayOptionWidget::updateUI);
}

void DisplayOptionWidget::clearPreviousAngle(int angle)
{
    switch (angle)
    {
    case  5:  ui->action5_degrees->setChecked(false); break;
    case 10: ui->action10_degrees->setChecked(false); break;
    case 15: ui->action15_degrees->setChecked(false); break;
    case 20: ui->action20_degrees->setChecked(false); break;
    case 30: ui->action30_degrees->setChecked(false); break;
    case 45: ui->action45_degrees->setChecked(false); break;
    default: ui->action15_degrees->setChecked(false);
    }
}

void DisplayOptionWidget::changeAngle(int angle)
{
    clearPreviousAngle(mOverlayAngle);
    mOverlayAngle = angle;
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("OverlayAngle", angle);
    emit editor()->view()->viewChanged();
}

void DisplayOptionWidget::updateUI()
{
    PreferenceManager* prefs = editor()->preference();

    SignalBlocker b1(ui->thinLinesButton);
    ui->thinLinesButton->setChecked(prefs->isOn(SETTING::INVISIBLE_LINES));

    SignalBlocker b2(ui->outLinesButton);
    ui->outLinesButton->setChecked(prefs->isOn(SETTING::OUTLINES));

    SignalBlocker b9(ui->overlayCenterButton);
    ui->overlayCenterButton->setChecked(prefs->isOn(SETTING::OVERLAY_CENTER));

    SignalBlocker b10(ui->overlayThirdsButton);
    ui->overlayThirdsButton->setChecked(prefs->isOn(SETTING::OVERLAY_THIRDS));

    SignalBlocker b11(ui->overlayGoldenRatioButton);
    ui->overlayGoldenRatioButton->setChecked(prefs->isOn(SETTING::OVERLAY_GOLDEN));

    SignalBlocker b12(ui->overlaySafeAreaButton);
    ui->overlaySafeAreaButton->setChecked(prefs->isOn(SETTING::OVERLAY_SAFE));

    SignalBlocker b13(ui->overlayPerspective1Button);
    ui->overlayPerspective1Button->setChecked(prefs->isOn(SETTING::OVERLAY_PERSPECTIVE1));

    SignalBlocker b14(ui->overlayPerspective2Button);
    ui->overlayPerspective2Button->setChecked(prefs->isOn(SETTING::OVERLAY_PERSPECTIVE2));

    SignalBlocker b15(ui->overlayPerspective3Button);
    ui->overlayPerspective3Button->setChecked(prefs->isOn(SETTING::OVERLAY_PERSPECTIVE3));
    if (prefs->isOn(SETTING::ACTION_SAFE_ON) || prefs->isOn(SETTING::TITLE_SAFE_ON))
    {
        ui->overlaySafeAreaButton->setEnabled(true);
    } else {
        ui->overlaySafeAreaButton->setEnabled(false);
    }

    ViewManager* view = editor()->view();

    SignalBlocker b3(ui->mirrorButton);
    ui->mirrorButton->setChecked(view->isFlipHorizontal());

    SignalBlocker b4(ui->mirrorVButton);
    ui->mirrorVButton->setChecked(view->isFlipVertical());
}

int DisplayOptionWidget::getMinHeightForWidth(int width)
{
    return ui->innerWidget->layout()->heightForWidth(width);
}

void DisplayOptionWidget::toggleMirror(bool isOn)
{
    editor()->view()->flipHorizontal(isOn);
}

void DisplayOptionWidget::toggleMirrorV(bool isOn)
{
    editor()->view()->flipVertical(isOn);
}

void DisplayOptionWidget::toggleOverlayCenter(bool isOn)
{
    editor()->view()->setOverlayCenter(isOn);
    editor()->preference()->set(SETTING::OVERLAY_CENTER, isOn);
}

void DisplayOptionWidget::toggleOverlayThirds(bool isOn)
{
    editor()->view()->setOverlayThirds(isOn);
    editor()->preference()->set(SETTING::OVERLAY_THIRDS, isOn);
}

void DisplayOptionWidget::toggleOverlayGoldenRatio(bool isOn)
{
    editor()->view()->setOverlayGoldenRatio(isOn);
    editor()->preference()->set(SETTING::OVERLAY_GOLDEN, isOn);
}

void DisplayOptionWidget::toggleOverlaySafeAreas(bool isOn)
{
    editor()->view()->setOverlaySafeAreas(isOn);
    editor()->preference()->set(SETTING::OVERLAY_SAFE, isOn);
}

void DisplayOptionWidget::toggleOverlayPerspective1(bool isOn)
{
    editor()->view()->setOverlayPerspective1(isOn);
    editor()->preference()->set(SETTING::OVERLAY_PERSPECTIVE1, isOn);
}

void DisplayOptionWidget::toggleOverlayPerspective2(bool isOn)
{
    editor()->view()->setOverlayPerspective2(isOn);
    editor()->preference()->set(SETTING::OVERLAY_PERSPECTIVE2, isOn);
}

void DisplayOptionWidget::toggleOverlayPerspective3(bool isOn)
{
    editor()->view()->setOverlayPerspective3(isOn);
    editor()->preference()->set(SETTING::OVERLAY_PERSPECTIVE3, isOn);
}

void DisplayOptionWidget::anglePreferences()
{
    buttonStylesheet = "::menu-indicator{ image: none; }"
        "QToolButton { border: 0px; }"
        "QToolButton:pressed { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }"
        "QToolButton:checked { border: 1px solid #ADADAD; border-radius: 2px; background-color: #D5D5D5; }";

    ui->overlayChooseAngle->setArrowType(Qt::ArrowType::NoArrow);
    ui->overlayChooseAngle->setStyleSheet(buttonStylesheet);

    ui->overlayChooseAngle->addAction(ui->action5_degrees);
    ui->overlayChooseAngle->addAction(ui->action10_degrees);
    ui->overlayChooseAngle->addAction(ui->action15_degrees);
    ui->overlayChooseAngle->addAction(ui->action20_degrees);
    ui->overlayChooseAngle->addAction(ui->action30_degrees);
    ui->overlayChooseAngle->addAction(ui->action45_degrees);

    QSettings settings(PENCIL2D, PENCIL2D);
    mOverlayAngle = settings.value("OverlayAngle").toInt();

    switch (mOverlayAngle)
    {
    case  5:  ui->action5_degrees->setChecked(true); break;
    case 10: ui->action10_degrees->setChecked(true); break;
    case 15: ui->action15_degrees->setChecked(true); break;
    case 20: ui->action20_degrees->setChecked(true); break;
    case 30: ui->action30_degrees->setChecked(true); break;
    case 45: ui->action45_degrees->setChecked(true); break;
    default: ui->action15_degrees->setChecked(true);
    }

    connect( ui->action5_degrees, &QAction::triggered, this,  &DisplayOptionWidget::angle5degrees);
    connect(ui->action10_degrees, &QAction::triggered, this, &DisplayOptionWidget::angle10degrees);
    connect(ui->action15_degrees, &QAction::triggered, this, &DisplayOptionWidget::angle15degrees);
    connect(ui->action20_degrees, &QAction::triggered, this, &DisplayOptionWidget::angle20degrees);
    connect(ui->action30_degrees, &QAction::triggered, this, &DisplayOptionWidget::angle30degrees);
    connect(ui->action45_degrees, &QAction::triggered, this, &DisplayOptionWidget::angle45degrees);
}

void DisplayOptionWidget::angle5degrees(bool)
{
    changeAngle(5);
}

void DisplayOptionWidget::angle10degrees(bool)
{
    changeAngle(10);
}

void DisplayOptionWidget::angle15degrees(bool)
{
    changeAngle(15);
}

void DisplayOptionWidget::angle20degrees(bool)
{
    changeAngle(20);
}

void DisplayOptionWidget::angle30degrees(bool)
{
    changeAngle(30);
}

void DisplayOptionWidget::angle45degrees(bool)
{
    changeAngle(45);
}
