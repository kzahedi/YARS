#include <yars/configuration/data/DataLoggingGnuplot.h>
#include "DataBinding.h"

#include <yars/util/YarsErrorHandler.h>
#include <yars/util/FileSystemOperations.h>
#include <yars/configuration/YarsConfiguration.h>

#include <cstdlib>


#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_MODULE                    (char*)"module"
#define YARS_STRING_NAME                      (char*)"name"
#define YARS_STRING_SIZE                      (char*)"size"
#define YARS_STRING_DELAY                     (char*)"delay"
#define YARS_STRING_PAIRWISE                  (char*)"pairwise"
#define YARS_STRING_TERM                      (char*)"term"

#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_TARGET_DEFINITION         (char*)"target_definition"
#define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer_definition"
#define YARS_STRING_TRUE_FALSE_DEFINITION     (char*)"true_false_definition"

#define YARS_STRING_GNUPLOT_TERM_DEFINITION   (char*)"gnuplot_term_definition"
#define YARS_STRING_X11                       (char*)"x11"
#define YARS_STRING_AQUA                      (char*)"aqua"
#define YARS_STRING_WXT                       (char*)"wxt"


namespace
{
// Attribute binding table for the gnuplot logger's own opening tag.
// Child-element dispatch (target) stays hand-written below.
const std::vector<yars::AttributeBinding> &loggingGnuplotAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_SIZE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingGnuplot *>(self)->setSize(atoi(value.c_str())); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_DELAY,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingGnuplot *>(self)->setDelay(atoi(value.c_str())); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_TERM,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingGnuplot *>(self)->setTerm(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_PAIRWISE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingGnuplot *>(self)->setPairwise(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataLoggingGnuplot::DataLoggingGnuplot(DataNode *parent)
  : DataNode(parent)
{
  _size = 100;
  _delay = 50;
  _pairwise = false;
#ifdef __APPLE__
  _term     = YARS_STRING_AQUA;
#else
  _term     = YARS_STRING_X11;
#endif // __APPLE__
}

DataLoggingGnuplot::~DataLoggingGnuplot()
{
  // nothing to be done
}

void DataLoggingGnuplot::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GNUPLOT_LOGGER))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_GNUPLOT_LOGGER))
  {
    yars::applyAttributes(this, element, loggingGnuplotAttributeBindings());
  }
  if(element->opening(YARS_STRING_TARGET))
  {
    // In headless mode (--nogui) skip display-bound gnuplot setup entirely.
    // The logger requires a live X display + gnuplot process to plot to;
    // without --gui that infrastructure isn't running, so erroring out on
    // DISPLAY is a false positive for batch / CI / headless audit runs.
    if (!__YARS_GET_USE_VISUALISATION)
    {
      return;
    }

#ifndef __APPLE__
    if(getenv("DISPLAY") == NULL)
    {
      YarsErrorHandler::push("Can't open display");
      exit(-1);
    }
#endif
    if (!FileSystemOperations::doesExecutableExist("gnuplot"))
    {
      std::cerr << "Warning: gnuplot not found in PATH - gnuplot logging disabled." << std::endl;
      return;
    }

    string target;
    element->set(YARS_STRING_NAME, target);
    _targets.push_back(target);
  }
}

int DataLoggingGnuplot::size()
{
  return _size;
}

int DataLoggingGnuplot::delay()
{
  return _delay;
}

bool DataLoggingGnuplot::pairwise()
{
  return _pairwise;
}

string DataLoggingGnuplot::name()
{
  return _name;
}

string DataLoggingGnuplot::term()
{
  return _term;
}

void DataLoggingGnuplot::setSize(int size)
{
  _size = size;
}

void DataLoggingGnuplot::setDelay(int delay)
{
  _delay = delay;
}

void DataLoggingGnuplot::setTerm(string term)
{
  _term = term;
}

void DataLoggingGnuplot::setPairwise(bool pairwise)
{
  _pairwise = pairwise;
}

DataLoggingGnuplot* DataLoggingGnuplot::copy()
{
  DataLoggingGnuplot *copy = new DataLoggingGnuplot(NULL);
  for(std::vector<string>::iterator i = _targets.begin(); i != _targets.end(); i++)
  {
    copy->_targets.push_back(*i);
  }
  copy->_size = _size;
  copy->_delay = _delay;
  copy->_pairwise = _pairwise;
  copy->_name = _name;
  copy->_term = _term;
  return copy;
}
