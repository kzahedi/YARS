#ifndef __OBSERVABLE_H__
#define __OBSERVABLE_H__

#include <yars/util/Observer.h>
#include <yars/util/ObservableMessage.h>

#include <vector>
#include <algorithm>

class Observable
{
  public:
    void addObserver(Observer* o)
    {
      if (o != nullptr) {
        observers.push_back(o);
      }
    }

    void removeObserver(Observer* o)
    {
      observers.erase(
        std::remove(observers.begin(), observers.end(), o),
        observers.end()
      );
    }

    void notifyObservers(ObservableMessage *message)
    {
      for (auto* observer : observers) {
        if (observer != nullptr) {
          observer->notify(message);
        }
      }
    }

  protected:
    std::vector<Observer*> observers;
};

#endif // __OBSERVABLE_H__

