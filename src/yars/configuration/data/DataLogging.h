#ifndef __DATA_LOGGING_H__
#define __DATA_LOGGING_H__

#include <yars/configuration/data/DataNode.h>

#include <yars/configuration/data/DataLoggingObject.h>
#include <yars/configuration/data/DataLoggingSensor.h>
#include <yars/configuration/data/DataLoggingActuator.h>
#include <yars/configuration/data/DataLoggingController.h>

#include <yars/configuration/data/DataLoggingConsole.h>
#include <yars/configuration/data/DataLoggingSelforg.h>
#include <yars/configuration/data/DataLoggingGnuplot.h>
#include <yars/configuration/data/DataLoggingFile.h>
#include <yars/configuration/data/DataLoggingCSV.h>
#include <yars/configuration/data/DataLoggingBlender.h>

#include <yars/configuration/data/DataObjects.h>
#include <yars/configuration/data/DataSensors.h>
#include <yars/configuration/data/DataActuators.h>
#include <yars/configuration/data/DataControllers.h>

#include <yars/util/stl_macros.h>

#include <vector>

#define YARS_STRING_LOGGING            (char*)"logging"
#define YARS_STRING_LOGGING_DEFINITION (char*)"logging_definition"


class DataLogging : public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLogging(DataNode *parent);

    /**
     * @brief Destructor.
     */
    virtual ~DataLogging();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLogging* copy();

    void checkGeoms(DataObjects *objects);
    void checkSensors(DataSensors *sensors);
    void checkActuators(DataActuators *actuators);
    void checkControllers(DataControllers *controllers);
    void finish();

    static void createXsd(XsdSpecification *spec);

    // std::vector<DataLoggingSelforg*>::iterator selforg_begin();
    // std::vector<DataLoggingSelforg*>::iterator selforg_end();
    // int                                   selforg_size();

    std::vector<DataLoggingObject*>::iterator     lo_begin();
    std::vector<DataLoggingObject*>::iterator     lo_end();
    int                                           lo_size();

    std::vector<DataLoggingSensor*>::iterator     ls_begin();
    std::vector<DataLoggingSensor*>::iterator     ls_end();
    int                                           ls_size();

    std::vector<DataLoggingActuator*>::iterator   la_begin();
    std::vector<DataLoggingActuator*>::iterator   la_end();
    int                                           la_size();

    std::vector<DataLoggingController*>::iterator lc_begin();
    std::vector<DataLoggingController*>::iterator lc_end();
    int                                           lc_size();

    std::vector<DataLoggingConsole*>::iterator    c_begin();
    std::vector<DataLoggingConsole*>::iterator    c_end();
    int                                           c_size();

    std::vector<DataLoggingFile*>::iterator       f_begin();
    std::vector<DataLoggingFile*>::iterator       f_end();
    int                                           f_size();

    std::vector<DataLoggingCSV*>::iterator        csv_begin();
    std::vector<DataLoggingCSV*>::iterator        csv_end();
    int                                           csv_size();

    std::vector<DataLoggingBlender*>::iterator    b_begin();
    std::vector<DataLoggingBlender*>::iterator    b_end();
    int                                           b_size();

    std::vector<DataLoggingGnuplot*>::iterator    g_begin();
    std::vector<DataLoggingGnuplot*>::iterator    g_end();
    int                                           g_size();

    std::vector<DataLoggingSelforg*>::iterator    so_begin();
    std::vector<DataLoggingSelforg*>::iterator    so_end();
    int                                           so_size();

  private:
    std::vector<DataLoggingObject*>     _objects;
    std::vector<DataLoggingSensor*>     _sensors;
    std::vector<DataLoggingActuator*>   _actuators;
    std::vector<DataLoggingController*> _controllers;

    std::vector<DataLoggingConsole*>    _consolelogger;
    std::vector<DataLoggingFile*>       _filelogger;
    std::vector<DataLoggingCSV*>        _csvlogger;
    std::vector<DataLoggingBlender*>    _blenderlogger;
    std::vector<DataLoggingGnuplot*>    _gnuplotlogger;
    std::vector<DataLoggingSelforg*>    _selforglogger;
};

#endif // __DATA_LOGGING_H__
