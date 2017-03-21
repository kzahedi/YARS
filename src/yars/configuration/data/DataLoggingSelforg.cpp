#include <yars/configuration/data/DataLoggingSelforg.h>

#include <yars/util/YarsErrorHandler.h>
#include <yars/util/FileSystemOperations.h>

#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_MODULE                    (char*)"module"
#define YARS_STRING_FILENAME                  (char*)"filename"

#define YARS_STRING_GUILOGGER                 (char*)"guilogger"
#define YARS_STRING_MATRIXVIZ                 (char*)"matrixviz"
#define YARS_STRING_FILE                      (char*)"file"
#define YARS_STRING_STEP_SIZE                 (char*)"stepsize"

#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_TARGET_DEFINITION         (char*)"target"                    DIVIDER "definition"
#define YARS_STRING_TRUE_FALSE_DEFINITION     (char*)"true"                      DIVIDER "false"      DIVIDER DEFINITION
#define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer" DIVIDER DEFINITION

#define YARS_STRING_MATRIXVIZ_DEFINITION      (char*)"matrixviz" DIVIDER DEFINITION
#define YARS_STRING_GUILOGGER_DEFINITION      (char*)"guilogger" DIVIDER DEFINITION

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
    element->set(YARS_STRING_NAME,      _name);
    element->set(YARS_STRING_MATRIXVIZ, _useMatrixviz);
    element->set(YARS_STRING_GUILOGGER, _useGuilogger);
    element->set(YARS_STRING_FILE,      _useFile);
    element->set(YARS_STRING_FILENAME,  _filename);

    if(_useMatrixviz || _useGuilogger)
    {
      if(getenv("DISPLAY") == NULL)
      {
        YarsErrorHandler::push("Can't open display");
        exit(-1);
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

void DataLoggingSelforg::createXsd(XsdSpecification *spec)
{
  // TODO: define filename type
  XsdSequence *selforgDefinition = new XsdSequence(YARS_STRING_SELFORG_LOGGER_DEFINITION);
  selforgDefinition->add(NA(YARS_STRING_NAME,      YARS_STRING_XSD_STRING,                true));
  selforgDefinition->add(NA(YARS_STRING_FILENAME,  YARS_STRING_XSD_STRING,                false));
  selforgDefinition->add(NA(YARS_STRING_GUILOGGER, YARS_STRING_TRUE_FALSE_DEFINITION,     false));
  selforgDefinition->add(NA(YARS_STRING_MATRIXVIZ, YARS_STRING_TRUE_FALSE_DEFINITION,     false));
  selforgDefinition->add(NA(YARS_STRING_FILE,      YARS_STRING_TRUE_FALSE_DEFINITION,     false));
  selforgDefinition->add(NE(YARS_STRING_GUILOGGER, YARS_STRING_GUILOGGER_DEFINITION,      0));
  selforgDefinition->add(NE(YARS_STRING_MATRIXVIZ, YARS_STRING_MATRIXVIZ_DEFINITION,      0));
  selforgDefinition->add(NE(YARS_STRING_TARGET,    YARS_STRING_TARGET_DEFINITION,         0));
  spec->add(selforgDefinition);

  XsdSequence *targetDefinition = new XsdSequence(YARS_STRING_TARGET_DEFINITION);
  targetDefinition->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING, true));
  spec->add(targetDefinition);

  XsdSequence *guiloggerDefinition = new XsdSequence(YARS_STRING_GUILOGGER_DEFINITION);
  guiloggerDefinition->add(NA(YARS_STRING_STEP_SIZE, YARS_STRING_POSITIVE_NON_ZERO_INTEGER, true));
  spec->add(guiloggerDefinition);

  XsdSequence *matrixvizDefinition = new XsdSequence(YARS_STRING_MATRIXVIZ_DEFINITION);
  matrixvizDefinition->add(NA(YARS_STRING_STEP_SIZE, YARS_STRING_POSITIVE_NON_ZERO_INTEGER, true));
  spec->add(matrixvizDefinition);

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

