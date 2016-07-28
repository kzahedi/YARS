#ifndef __WINDOW_CONFIGURATIONS_DIALOG_H__
#define __WINDOW_CONFIGURATIONS_DIALOG_H__

#include "WindowConfiguration.h"

#include <QtGui>

class WindowConfigurationDialog : public QFrame
{
  Q_OBJECT
  public:
    WindowConfigurationDialog(WindowConfiguration *windowConfiguration, QWidget *parent = 0);

  public slots:
    void windowNameChanged();
    void captureNameChanged();
    void tracesChanged(int status);
    void texturesChanged(int status);
    void followModeChanged(int status);
    void visualiseAxesChanged(int status);
    void followModeSelected(int index);
    void widthChanged();
    void heightChanged();
    void cameraPoseChanged();
    void maxTraceLinesChanged();
    void maxTracePointsChanged();

  signals:
    void dataChanged();

  private:
    WindowConfiguration *_windowConfiguration;
    QLineEdit *_windowName;
    QLineEdit *_captureName;
    QCheckBox *_useTraces;
    QCheckBox *_useTextures;
    QCheckBox *_useFollowMode;
    QCheckBox *_useVisualeAxes;
    QComboBox *_followModes;
    QLineEdit *_width;
    QLineEdit *_height;
    QLineEdit *_xyz_x;
    QLineEdit *_xyz_y;
    QLineEdit *_xyz_z;
    QLineEdit *_lookAt_x;
    QLineEdit *_lookAt_y;
    QLineEdit *_lookAt_z;
    QLineEdit *_maxTraceLines;
    QLineEdit *_maxTracePoints;

};

#endif // __WINDOW_CONFIGURATIONS_DIALOG_H__
