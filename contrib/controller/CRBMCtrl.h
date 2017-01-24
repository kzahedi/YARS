#include <yars/rc/RobotController.hpp>

#include <crbm/CRBMController.h>
#include <entropy++/Container.h>

#include <string>
#include <iostream>

using namespace std;

class CRBMCtrl: public RobotController
{
  public:
    void init();
    void close();
    void reset();
    void update();

  private:

    libcrbm::binary::CRBMController* _crbm;

    double* S;
    double* A;

    int     t;
    int     T;

    entropy::DContainer* Adata;

};
