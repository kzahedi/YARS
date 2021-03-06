#ifndef __YARS_XSD_SAX_PARSER_H__
#define __YARS_XSD_SAX_PARSER_H__

#include <vector>
#include <string>

using namespace std;

/**
 * @brief This class reads an XML file, validates it against the XML Schema
 * Grammar generated by YarsXSDGenerator, which was specified in
 * XsdSpecification, and stores the content of the XML file in
 * DataRobotSimulationDescription (singleton)
 * @sa YarsXSDGenerator
 * @sa XsdSpecification
 * @sa DataRobotSimulationDescription
 */
class YarsXSDSaxParser
{
  public:
    /**
     * @brief Read xml file.
     *
     * @param filename
     *
     * @return true/false if parse has failed or was successful
     */
    bool read(string filename);

    bool parse(string xml);

    std::vector<string>::iterator e_begin();
    std::vector<string>::iterator e_end();
    int e_size();

    std::vector<string>::iterator f_begin();
    std::vector<string>::iterator f_end();
    int f_size();

    std::vector<string>::iterator w_begin();
    std::vector<string>::iterator w_end();
    int w_size();

    int errors();

  private:
    std::vector<string> _errors;
    std::vector<string> _warnings;
    std::vector<string> _fatals;
};

#endif // __YARS_XSD_SAX_PARSER_H__


