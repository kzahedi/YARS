#include <yars/configuration/data/DataLoggingSelforg.h>
#include "DataBinding.h"

#include <yars/util/YarsErrorHandler.h>
#include <yars/util/FileSystemOperations.h>
#include <yars/configuration/YarsConfiguration.h>

#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_MODULE                    (char*)"module"
#define YARS_STRING_NAME                      (char*)"name"
#define YARS_STRING_FILENAME                  (char*)"filename"

#define YARS_STRING_GUILOGGER                 (char*)"guilogger"
#define YARS_STRING_MATRIXVIZ                 (char*)"matrixviz"
#define YARS_STRING_FILE                      (char*)"file"
#define YARS_STRING_STEP_SIZE                 (char*)"stepsize"

#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_TARGET_DEFINITION         (char*)"target_definition"
#define YARS_STRING_TRUE_FALSE_DEFINITION     (char*)"true_false_definition"
#define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer_definition"

#define YARS_STRING_MATRIXVIZ_DEFINITION      (char*)"matrixviz_definition"
#define YARS_STRING_GUILOGGER_DEFINITION      (char*)"guilogger_definition"

namespace
{
// Attribute binding table for the selforg logger's own opening tag.
// Child-element dispatch (target/guilogger/matrixviz) and the post-parse
// validation stay hand-written below.
const std::vector<yars::AttributeBinding> &loggingSelforgAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingSelforg *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_MATRIXVIZ,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingSelforg *>(self)->setUseMatrixviz(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_GUILOGGER,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingSelforg *>(self)->setUseGuilogger(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_FILE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingSelforg *>(self)->setUseFile(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_FILENAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingSelforg *>(self)->setFilename(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataLoggingSelforg::DataLoggingSelforg(DataNode *parent)
  : DataNode(parent)
{
  _useFile             = false;
  _useMatrixviz        = false;
  _useGuilogger        = false;
  _guiloggerStepSize = 1;
  _matrixvizStepSize = 1;
}

DataLoggingSelforg::~DataLoggingSelforg()
{
  // nothing to be done
}

void DataLoggingSelforg::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_SELFORG_LOGGER))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_SELFORG_LOGGER))
  {
    stringstream oss;
    yars::applyAttributes(this, element, loggingSelforgAttributeBindings());

    if(_useMatrixviz || _useGuilogger)
    {
      // In headless mode (--nogui) skip the display check; matrixviz /
      // guilogger require a live X display + helper processes, neither of
      // which run when YARS is launched headless. Erroring out on DISPLAY
      // would otherwise turn batch / CI / headless audit runs into hard
      // failures for any XML that opted into selforg logging.
      if (__YARS_GET_USE_VISUALISATION)
      {
#ifndef __APPLE__
        if(getenv("DISPLAY") == NULL)
        {
          YarsErrorHandler::push("Can't open display");
          exit(-1);
        }
#endif
      }
    }
    if(_useMatrixviz)
    {
      if (!FileSystemOperations::doesExecutableExist("matrixviz"))
      {
        oss << "Can't find matrixviz in your PATH" << endl;
      }
    }
    if(_useGuilogger)
    {
      if (!FileSystemOperations::doesExecutableExist("guilogger"))
      {
        oss << "Can't find guilogger in your PATH" << endl;
      }
    }

    if(_useFile && _filename.length() == 0)
    {
      oss << "Use file selected in Selforg Logger, but not file name is given." << endl;
    }

    if (oss.str().length() > 0)
    {
      YarsErrorHandler::push(oss.str());
      exit(-1);
    }

  }
  if(element->opening(YARS_STRING_TARGET))
  {
    string target;
    element->set(YARS_STRING_NAME, target);
    _targets.push_back(target);
  }

  if(element->opening(YARS_STRING_GUILOGGER))
  {
    element->set(YARS_STRING_STEP_SIZE, _guiloggerStepSize);
  }
  if(element->opening(YARS_STRING_MATRIXVIZ))
  {
    element->set(YARS_STRING_STEP_SIZE, _matrixvizStepSize);
  }
}

DataLoggingSelforg* DataLoggingSelforg::copy()
{
  DataLoggingSelforg *copy = new DataLoggingSelforg(NULL);
  for(std::vector<string>::iterator i = _targets.begin(); i != _targets.end(); i++)
  {
    copy->_targets.push_back(*i);
  }
  copy->_name              = _name;
  copy->_filename          = _filename;
  copy->_useFile           = _useFile;
  copy->_useMatrixviz      = _useMatrixviz;
  copy->_useGuilogger      = _useGuilogger;
  copy->_guiloggerStepSize = _guiloggerStepSize;
  copy->_matrixvizStepSize = _matrixvizStepSize;
  return copy;
}

bool DataLoggingSelforg::useFile()
{
  return _useFile;
}

bool DataLoggingSelforg::useMatrixviz()
{
  return _useMatrixviz;
}

bool DataLoggingSelforg::useGuilogger()
{
  return _useGuilogger;
}

string DataLoggingSelforg::filename()
{
  return _filename;
}

void DataLoggingSelforg::setFilename(string filename)
{
  _filename = filename;
}

void DataLoggingSelforg::setName(string name)
{
  _name = name;
}

void DataLoggingSelforg::setUseFile(bool useFile)
{
  _useFile = useFile;
}

void DataLoggingSelforg::setUseMatrixviz(bool useMatrixviz)
{
  _useMatrixviz = useMatrixviz;
}

void DataLoggingSelforg::setUseGuilogger(bool useGuilogger)
{
  _useGuilogger = useGuilogger;
}

string DataLoggingSelforg::name()
{
  return _name;
}

int DataLoggingSelforg::glStepSize()
{
  return _guiloggerStepSize;

}

int DataLoggingSelforg::mvStepSize()
{
  return _matrixvizStepSize;
}

