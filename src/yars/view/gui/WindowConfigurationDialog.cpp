#include "WindowConfigurationDialog.h"

WindowConfigurationDialog::WindowConfigurationDialog(WindowConfiguration *windowConfiguration, QWidget *parent)
  : QFrame()
{
  _windowConfiguration = windowConfiguration;

  QString w;
  QString h;

  w.setNum(_windowConfiguration->geometry.width());
  h.setNum(_windowConfiguration->geometry.height());

  QString xyz_x;
  QString xyz_y;
  QString xyz_z;

  QString lookAt_x;
  QString lookAt_y;
  QString lookAt_z;

  QString maxTraceLines;
  QString maxTracePoints;

  xyz_x.setNum(_windowConfiguration->cameraPosition.x);
  xyz_y.setNum(_windowConfiguration->cameraPosition.y);
  xyz_z.setNum(_windowConfiguration->cameraPosition.z);

  lookAt_x.setNum(_windowConfiguration->cameraLookAt.x);
  lookAt_y.setNum(_windowConfiguration->cameraLookAt.y);
  lookAt_z.setNum(_windowConfiguration->cameraLookAt.z);

  maxTraceLines.setNum(_windowConfiguration->maxTraceLines);
  maxTracePoints.setNum(_windowConfiguration->maxTracePoints);

  QScrollArea *scrollArea = new QScrollArea();

  QWidget     *content    = new QWidget();
  QGridLayout *layout     = new QGridLayout();
  QBoxLayout  *box        = new QBoxLayout(QHBoxLayout::LeftToRight);

  _windowName             = new QLineEdit(QString(_windowConfiguration->name.c_str()));
  _captureName            = new QLineEdit(QString(_windowConfiguration->captureName.c_str()));
  _useTraces              = new QCheckBox();
  _useTextures            = new QCheckBox();
  _useFollowMode          = new QCheckBox();
  _useVisualeAxes         = new QCheckBox();
  _followModes            = new QComboBox();
  _width                  = new QLineEdit(w);
  _height                 = new QLineEdit(h);
  _xyz_x                  = new QLineEdit(xyz_x);
  _xyz_y                  = new QLineEdit(xyz_y);
  _xyz_z                  = new QLineEdit(xyz_z);
  _lookAt_x               = new QLineEdit(lookAt_x);
  _lookAt_y               = new QLineEdit(lookAt_y);
  _lookAt_z               = new QLineEdit(lookAt_z);
  _maxTraceLines          = new QLineEdit(maxTraceLines);
  _maxTracePoints         = new QLineEdit(maxTracePoints);

  for(std::vector<string>::iterator i = _windowConfiguration->camNames.begin();
      i != _windowConfiguration->camNames.end(); i++)
  {
    _followModes->addItem(QString::fromStdString(*i));
  }
  _followModes->setCurrentIndex(_windowConfiguration->followMode);

  if(_windowConfiguration->useTraces)
  {
    _useTraces->setCheckState(Qt::Checked);
  }
  else
  {
    _useTraces->setCheckState(Qt::Unchecked);
  }

  if(_windowConfiguration->useTextures)
  {
    _useTextures->setCheckState(Qt::Checked);
  }
  else
  {
    _useTextures->setCheckState(Qt::Unchecked);
  }

  if(_windowConfiguration->useFollow)
  {
    _useFollowMode->setCheckState(Qt::Checked);
  }
  else
  {
    _useFollowMode->setCheckState(Qt::Unchecked);
  }

  if(_windowConfiguration->visualiseAxes)
  {
    _useVisualeAxes->setCheckState(Qt::Checked);
  }
  else
  {
    _useVisualeAxes->setCheckState(Qt::Unchecked);
  }


  int row = 0;
  layout->addWidget(new QLabel("Window name"),        row, 0, 1, 1);
  layout->addWidget(_windowName,                      row, 1, 1, 3);

  row++;
  layout->addWidget(new QLabel("Capture name"),       row, 0, 1, 1);
  layout->addWidget(_captureName,                     row, 1, 1, 3);

  row++;
  layout->addWidget(new QLabel("Window dimension"),   row, 0, 1, 1);
  layout->addWidget(_width,                           row, 1, 1, 1);
  layout->addWidget(new QLabel("x"),                  row, 2, 1, 1);
  layout->addWidget(_height,                          row, 3, 1, 1);

  row++;
  layout->addWidget(new QLabel("Camera position"),    row, 0, 1, 1);
  layout->addWidget(_xyz_x,                           row, 1, 1, 1);
  layout->addWidget(_xyz_y,                           row, 2, 1, 1);
  layout->addWidget(_xyz_z,                           row, 3, 1, 1);

  row++;
  layout->addWidget(new QLabel("Camera look at"),     row, 0, 1, 1);
  layout->addWidget(_lookAt_x,                        row, 1, 1, 1);
  layout->addWidget(_lookAt_y,                        row, 2, 1, 1);
  layout->addWidget(_lookAt_z,                        row, 3, 1, 1);

  row++;
  layout->addWidget(new QLabel("Visualise axes"),     row, 0, 1, 1);
  layout->addWidget(_useVisualeAxes,                  row, 1, 1, 3);

  row++;
  layout->addWidget(new QLabel("Use textures"),       row, 0, 1, 1);
  layout->addWidget(_useTextures,                     row, 1, 1, 3);

  row++;
  layout->addWidget(new QLabel("Use traces"),         row, 0, 1, 1);
  layout->addWidget(_useTraces,                       row, 1, 1, 3);

  row++;
  layout->addWidget(new QLabel("Use Follow mode"),    row, 0, 1, 1);
  layout->addWidget(_useFollowMode,                   row, 1, 1, 3);

  row++;
  layout->addWidget(new QLabel("Follow mode"),        row, 0, 1, 1);
  layout->addWidget(_followModes,                     row, 1, 1, 3);

  row++;
  layout->addWidget(new QLabel("Max Trace Lines"),    row, 0, 1, 1);
  layout->addWidget(_maxTraceLines,                   row, 1, 1, 3);

  row++;
  layout->addWidget(new QLabel("Max Trace Points"),    row, 0, 1, 1);
  layout->addWidget(_maxTracePoints,                   row, 1, 1, 3);

  content->setLayout(layout);
  scrollArea->setWidget(content);
  box->addWidget(scrollArea);
  setLayout(box);

  connect(_windowName,     SIGNAL(editingFinished()),        this,   SLOT(windowNameChanged()));
  connect(_captureName,    SIGNAL(editingFinished()),        this,   SLOT(captureNameChanged()));
  connect(_width,          SIGNAL(editingFinished()),        this,   SLOT(widthChanged()));
  connect(_height,         SIGNAL(editingFinished()),        this,   SLOT(heightChanged()));
  connect(_useTraces,      SIGNAL(stateChanged(int)),        this,   SLOT(tracesChanged(int)));
  connect(_useTextures,    SIGNAL(stateChanged(int)),        this,   SLOT(texturesChanged(int)));
  connect(_useFollowMode,  SIGNAL(stateChanged(int)),        this,   SLOT(followModeChanged(int)));
  connect(_useVisualeAxes, SIGNAL(stateChanged(int)),        this,   SLOT(visualiseAxesChanged(int)));
  connect(_followModes,    SIGNAL(currentIndexChanged(int)), this,   SLOT(followModeSelected(int)));
  connect(_xyz_x,          SIGNAL(editingFinished()),        this,   SLOT(cameraPoseChanged()));
  connect(_xyz_y,          SIGNAL(editingFinished()),        this,   SLOT(cameraPoseChanged()));
  connect(_xyz_z,          SIGNAL(editingFinished()),        this,   SLOT(cameraPoseChanged()));
  connect(_lookAt_x,          SIGNAL(editingFinished()),        this,   SLOT(cameraPoseChanged()));
  connect(_lookAt_y,          SIGNAL(editingFinished()),        this,   SLOT(cameraPoseChanged()));
  connect(_lookAt_z,          SIGNAL(editingFinished()),        this,   SLOT(cameraPoseChanged()));
  connect(_maxTraceLines,  SIGNAL(editingFinished()),        this,   SLOT(maxTraceLinesChanged()));
  connect(_maxTracePoints, SIGNAL(editingFinished()),        this,   SLOT(maxTracePointsChanged()));
  connect(this,            SIGNAL(dataChanged()),            parent, SLOT(windowConfigurationChanged()));
}

void WindowConfigurationDialog::windowNameChanged()
{
  _windowConfiguration->name = _windowName->text().toStdString();
  emit dataChanged();
}

void WindowConfigurationDialog::captureNameChanged()
{
  _windowConfiguration->captureName = _captureName->text().toStdString();
}

void WindowConfigurationDialog::tracesChanged(int status)
{
  _windowConfiguration->useTraces = (status == Qt::Checked);
}

void WindowConfigurationDialog::texturesChanged(int status)
{
  _windowConfiguration->useTextures = (status == Qt::Checked);
}

void WindowConfigurationDialog::followModeChanged(int status)
{
  _windowConfiguration->useFollow = (status == Qt::Checked);
}

void WindowConfigurationDialog::visualiseAxesChanged(int status)
{
  _windowConfiguration->visualiseAxes = (status == Qt::Checked);
}

void WindowConfigurationDialog::followModeSelected(int index)
{
  _windowConfiguration->followMode = index;
  _windowConfiguration->followModeChanged = true;
}

void WindowConfigurationDialog::widthChanged()
{
  _windowConfiguration->geometry.setWidth(_width->text().toInt());
  emit dataChanged();
}

void WindowConfigurationDialog::heightChanged()
{
  _windowConfiguration->geometry.setHeight(_height->text().toInt());
  emit dataChanged();
}

void WindowConfigurationDialog::cameraPoseChanged()
{
  _windowConfiguration->cameraPosition.x = _xyz_x->text().toDouble();
  _windowConfiguration->cameraPosition.y = _xyz_y->text().toDouble();
  _windowConfiguration->cameraPosition.z = _xyz_z->text().toDouble();
  _windowConfiguration->cameraLookAt.x   = _lookAt_x->text().toDouble();
  _windowConfiguration->cameraLookAt.y   = _lookAt_y->text().toDouble();
  _windowConfiguration->cameraLookAt.z   = _lookAt_z->text().toDouble();
  emit dataChanged();
}

void WindowConfigurationDialog::maxTraceLinesChanged()
{
  _windowConfiguration->maxTraceLines = _maxTraceLines->text().toInt();
}

void WindowConfigurationDialog::maxTracePointsChanged()
{
  _windowConfiguration->maxTracePoints = _maxTracePoints->text().toInt();
}


