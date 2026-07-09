#ifndef __DATA_LOGGING_GNUPLOT_H__
#define __DATA_LOGGING_GNUPLOT_H__

#include "DataNode.h"
#include "DataLoggingModule.h"

#include <string>
#include <vector>

using namespace std;

#define YARS_STRING_GNUPLOT_LOGGER            (char*)"gnuplot"
#define YARS_STRING_GNUPLOT_LOGGER_DEFINITION (char*)"gnuplot_logging_definition"

class DataLoggingGnuplot : public DataLoggingModule, public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLoggingGnuplot(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataLoggingGnuplot();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLoggingGnuplot* copy();

    int size();
    void setSize(int size);
    int delay();
    void setDelay(int delay);
    bool pairwise();
    void setPairwise(bool pairwise);
    string name();
    string term();
    void setTerm(string term);

  private:
    int _size;
    int _delay;
    bool _pairwise;
    string _name;
    string _term;
};

#endif // __DATA_LOGGING_GNUPLOT_H__




