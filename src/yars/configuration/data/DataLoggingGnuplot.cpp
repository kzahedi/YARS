#include <yars/configuration/data/DataLoggingGnuplot.h>

#include <yars/util/YarsErrorHandler.h>
#include <yars/util/FileSystemOperations.h>


#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_MODULE                    (char*)"module"
#define YARS_STRING_SIZE                      (char*)"size"
#define YARS_STRING_DELAY                     (char*)"delay"
#define YARS_STRING_PAIRWISE                  (char*)"pairwise"
#define YARS_STRING_TERM                      (char*)"term"

#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_TARGET_DEFINITION         (char*)"target"                    DIVIDER "definition"
#define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer" DIVIDER DEFINITION
#define YARS_STRING_TRUE_FALSE_DEFINITION     (char*)"true"                      DIVIDER "false"      DIVIDER DEFINITION

#define YARS_STRING_GNUPLOT_TERM_DEFINITION   (char*)"gnuplot_term_definition"
#define YARS_STRING_X11                       (char*)"x11"
#define YARS_STRING_AQUA                      (char*)"aqua"
#define YARS_STRING_WXT                       (char*)"wxt"


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
    element->set(YARS_STRING_SIZE,     _size);
    element->set(YARS_STRING_DELAY,    _delay);
    element->set(YARS_STRING_TERM,     _term);
    element->set(YARS_STRING_PAIRWISE, _pairwise);
  }
  if(element->opening(YARS_STRING_TARGET))
  {

    if(getenv("DISPLAY") == NULL)
    {
      YarsErrorHandler::push("Can't open display");
      exit(-1);
    }
    if (!FileSystemOperations::doesExecutableExist("gnuplot"))
    {
      YarsErrorHandler::push("Can't find gnuplot in your PATH");
      exit(-1);
    }

    string target;
    element->set(YARS_STRING_NAME, target);
    _targets.push_back(target);
  }
}

void DataLoggingGnuplot::createXsd(XsdSpecification *spec)
{
  XsdSequence *gnuplotDefinition = new XsdSequence(YARS_STRING_GNUPLOT_LOGGER_DEFINITION);
  gnuplotDefinition->add(NA(YARS_STRING_SIZE,     YARS_STRING_POSITIVE_NON_ZERO_INTEGER, true));
  gnuplotDefinition->add(NA(YARS_STRING_DELAY,    YARS_STRING_POSITIVE_NON_ZERO_INTEGER, true));
  gnuplotDefinition->add(NA(YARS_STRING_NAME,     YARS_STRING_XSD_STRING,                false));
  gnuplotDefinition->add(NA(YARS_STRING_PAIRWISE, YARS_STRING_TRUE_FALSE_DEFINITION,     false));
  gnuplotDefinition->add(NA(YARS_STRING_TERM,     YARS_STRING_GNUPLOT_TERM_DEFINITION,   false));
  gnuplotDefinition->add(NE(YARS_STRING_TARGET,   YARS_STRING_TARGET_DEFINITION, 0));
  spec->add(gnuplotDefinition);

  XsdSequence *targetDefinition = new XsdSequence(YARS_STRING_TARGET_DEFINITION);
  targetDefinition->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING, true));
  spec->add(targetDefinition);

  XsdEnumeration *gnuplotTermDefinition = new XsdEnumeration(YARS_STRING_GNUPLOT_TERM_DEFINITION,
      YARS_STRING_XSD_STRING);
  gnuplotTermDefinition->add(YARS_STRING_X11);
  gnuplotTermDefinition->add(YARS_STRING_AQUA);
  gnuplotTermDefinition->add(YARS_STRING_WXT);
  spec->add(gnuplotTermDefinition);
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
