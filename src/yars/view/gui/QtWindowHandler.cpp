#include "QtWindowHandler.h"


#include <QtGui>
#include <QtOpenGL>

#include <sstream>
#include <algorithm>

#define NAME_PREFIX "Time Series Logging Window "

QtWindowHandler::QtWindowHandler(int index)
: QFrame()
{
  _closed                     = false;
  _windowConfiguration        = new WindowConfiguration(index);
  _cameraHandler              = new CameraHandler(_windowConfiguration);
  _firstTimeFollowablesCalled = true;
  // _scenarioWindow             = NULL;

  setWindowTitle(_windowConfiguration->name.c_str());

  int x = _windowConfiguration->geometry.x();
  int y = _windowConfiguration->geometry.y();
  int w = _windowConfiguration->geometry.width();
  int h = _windowConfiguration->geometry.height();

  QRect screenRect = QApplication::desktop()->screenGeometry();
  
  if(x == SG_CENTER)
  {
    x = (int)(((yReal)screenRect.width()) / 2.0 - ((yReal)w) / 2.0);
  }
  if(y == SG_CENTER)
  {
    y = (int)(((yReal)screenRect.height()) / 2.0 - ((yReal)h) / 2.0);
  }
  if(x == SG_END)
  {
    x = screenRect.width() - w;
  }
  if(y == SG_END)
  {
    y = screenRect.height() - h;
  }

  setGeometry(x, y, w, h);

  _glPanel = new QtGlPanel(_windowConfiguration, this);

  QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::LeftToRight);
  mainLayout->setContentsMargins(0,0,0,0);
  mainLayout->addWidget(_glPanel);
  setLayout(mainLayout);
  __createMenu();
  show();
  raise();

  _glPanel->initializeGL();

  // must be called after show
  _newWindow      = new ObservableMessage("open new window", -1);
  _windowClosed   = new ObservableMessage("window closed",   -2);
  // if(_windowConfiguration->followObjects)
  // {
    // _windowConfiguration->followObjects = false;
    // toggleFollowingOfObjects();
  // }

#ifdef USE_CAPTURE_VIDEO
  // if(__YARS_GET_USE_CAPTURE)
  // {
    // toggleCaptureMovie();
  // }
#endif // USE_CAPTURE_VIDEO
}

QtWindowHandler::~QtWindowHandler()
{
  delete _cameraHandler;
  delete _glPanel;
  delete _windowConfiguration;
  delete _newWindow;
  delete _windowClosed;
}

void QtWindowHandler::step()
{
  _glPanel->updateGL();

  // if(_windowConfiguration->followObjects)
  // {
    // _cameraHandler->update();
  // }
}

void QtWindowHandler::reset()
{
  _glPanel->reset();
  _cameraHandler->reset();
}

void QtWindowHandler::quit()
{
  hide();
  _glPanel->quit();
  delete _glPanel;
  _glPanel = NULL;
  QCoreApplication::exit(0);
}

void QtWindowHandler::__createMenu()
{
  QMenuBar *menuBar     = new QMenuBar(0);
  QMenu   *_windowsMenu = menuBar->addMenu(tr("&Window"));
  _scenarioWindowAction = _windowsMenu->addAction("Open Scenario Manager");
  connect(_windowsMenu, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
}

void QtWindowHandler::menuTriggered(QAction *action)
{
  if(action == _scenarioWindowAction)
  {
    __openScenarioWindow();
  }
}

void QtWindowHandler::__openScenarioWindow()
{
  // if(_scenarioWindow == NULL)
  // {
    // _scenarioWindow = new ScenarioWindow();
  // }
  // _scenarioWindow->show();
}


void QtWindowHandler::toggleFollowingOfObjects()
{
  // if(YarsContainers::Followables()->size() == 0) return;
  // _windowConfiguration->followObjects = !_windowConfiguration->followObjects;
  // if(_firstTimeFollowablesCalled) // activated -> initialise variables
  // {
    // __initialiseFollowable();
  // }
  // _followModeWasActivated = true;
  // _cameraHandler->follow(*_followedObject);
}

void QtWindowHandler::__initialiseFollowable()
{
  // _followedObject = YarsContainers::Followables()->begin();
  _firstTimeFollowablesCalled = false;
}

void QtWindowHandler::previousFollowMode()
{
  _cameraHandler->previousFollowMode();
  if(_followModeWasActivated)
  {
    // _cameraHandler->follow(*_followedObject);
  }
}

void QtWindowHandler::nextFollowMode()
{
  _cameraHandler->nextFollowMode();
  if(_followModeWasActivated)
  {
    // _cameraHandler->follow(*_followedObject);
  }
}

void QtWindowHandler::previousFollowable()
{
  // if(!_windowConfiguration->followObjects) return;
  // if(YarsContainers::Followables()->size() == 0) return;
  // if(_followedObject == YarsContainers::Followables()->begin())
  {
    // _followedObject = YarsContainers::Followables()->end();
  }
  // _followedObject--;
  // _cameraHandler->follow(*_followedObject);
}

void QtWindowHandler::nextFollowable()
{
  // if(!_windowConfiguration->followObjects) return;
  // if(YarsContainers::Followables()->size() == 0) return;
  // _followedObject++;
  // if(_followedObject == YarsContainers::Followables()->end())
  {
    // _followedObject = YarsContainers::Followables()->begin();
  }
  // _cameraHandler->follow(*_followedObject);
}

#ifdef USE_CAPTURE_VIDEO
void QtWindowHandler::toggleCaptureMovie()
{
  _glPanel->toggleCaptureMovie();
}
#endif // USE_CAPTURE_VIDEO

void QtWindowHandler::toggleWriteFrames()
{
  _glPanel->toggleWriteFrames();
}

void QtWindowHandler::openNewWindow()
{
  notifyObservers(_newWindow);
}

void QtWindowHandler::openWindowTitleDialog()
{
  bool ok;
  QString text = QInputDialog::getText(this, tr("Set Window Title"),
      tr("Window Title"), QLineEdit::Normal,
      QString(_windowConfiguration->name.c_str()), &ok);
  if (ok && !text.isEmpty())
  {
    setWindowTitle(text);
  }
  _windowConfiguration->name = text.toStdString();
}

void QtWindowHandler::setWindowSize()
{
  bool ok;
  QString sizeString;
  QString w;
  QString h;
  w.setNum(_windowConfiguration->geometry.width());
  h.setNum(_windowConfiguration->geometry.height());
  sizeString.append(w);
  sizeString.append("x");
  sizeString.append(h);
  QString text = QInputDialog::getText(this, tr("Set Window Size"),
      tr("Window Size"), QLineEdit::Normal,
      sizeString, &ok);
  if (ok && !text.isEmpty())
  {
    // TODO: catch exception
    string s = text.toStdString();
    string s_w = s.substr(0, s.find_first_of("x"));
    string s_h = s.substr(s.find_first_of("x")+1, s.length()-1);
    //_windowConfiguration->dimension.setWidth(atoi(s_w.c_str()));
    //_windowConfiguration->dimension.setHeight(atoi(s_h.c_str()));
    resize(atoi(s_w.c_str()), atoi(s_h.c_str()));
  }
}

void QtWindowHandler::windowConfigurationChanged()
{
  resize(_windowConfiguration->geometry.width(), _windowConfiguration->geometry.height());
  setWindowTitle(_windowConfiguration->name.c_str());
}

void QtWindowHandler::closeEvent(QCloseEvent*)
{
  _closed = true;
  notifyObservers(_windowClosed);
}

bool QtWindowHandler::closed()
{
  return _closed;
}
