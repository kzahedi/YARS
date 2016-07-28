#ifndef __YARS_XSD_SAX_HANDLER_H__
#define __YARS_XSD_SAX_HANDLER_H__

#include <yars/configuration/data/Data.h>

#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

XERCES_CPP_NAMESPACE_USE

/**
 * @brief This is the SAX2 parser of YARS. It should not be changed. This parser
 * creates entities of type DataParseElement, which also contain
 * DataParseAttribute. This elements are passed to the data package (see
 * Data)
 *
 *
 * @sa DataParseElement
 * @sa DataParseAttribute
 * @sa Data
 */
class YarsXSDSaxHandler : public DefaultHandler
{
  public:
    YarsXSDSaxHandler();

    void startElement(
        const   XMLCh* const    uri,
        const   XMLCh* const    localname,
        const   XMLCh* const    qname,
        const   Attributes&     attrs);

    void endElement(
        const XMLCh *const uri,
        const XMLCh *const localname,
        const XMLCh *const qname);

    void startDocument();
    void endDocument();

    void error(const SAXParseException& e);
    void fatalError(const SAXParseException& e);
    void warning(const SAXParseException& e);

    /**
     * @brief Returns a list of strings with error messages.
     *
     * @return list of strings
     */
    std::vector<string> errors();

    /**
     * @brief Returns a list of strings with warnings.
     *
     * @return list of strings
     */
    std::vector<string> warnings();

    /**
     * @brief Returns a list of strings with fatal messages.
     *
     * @return list of strings
     */
    std::vector<string> fatals();

  private:
    DataRobotSimulationDescription *_spec;

    std::vector<string> _errors;
    std::vector<string> _warnings;
    std::vector<string> _fatals;
};

#endif // __YARS_XSD_SAX_HANDLER_H__
