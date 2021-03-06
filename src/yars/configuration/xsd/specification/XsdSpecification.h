#ifndef __YARS_XSD_SPECIFICATION_H__
#define __YARS_XSD_SPECIFICATION_H__

#include "XsdSequence.h"
#include "XsdEnumeration.h"
#include "XsdChoice.h"
#include "XsdInterval.h"
#include "XsdRegularExpression.h"


# define XE new XsdElement
# define NA new XsdAttribute

# define DEFINITION "definition"

#include <vector>
using namespace std;

# define YARS_STRING_XSD_DECIMAL               (char*)"xs:decimal"
# define YARS_STRING_XSD_INTEGER               (char*)"xs:integer"
# define YARS_STRING_XSD_STRING                (char*)"xs:string"

# define YARS_STRING_PID_DEFINITION            (char*)"pid_definition"

# define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer_definition"
# define YARS_STRING_POSITIVE_INTEGER          (char*)"positive_integer_definition"
# define YARS_STRING_UNIT_INTERVAL             (char*)"unit_interval_definition"
# define YARS_STRING_POSITIVE_NON_ZERO_DECIMAL (char*)"positive_non_zero_decimal_definition"
# define YARS_STRING_POSITIVE_DECIMAL          (char*)"positive_decimal_definition"
# define YARS_STRING_NAME_DEFINITION           (char*)"name_definition"

# define YARS_STRING_TRUE_FALSE_DEFINITION     (char*)"true_false_definition"

/** \brief Singleton.
 *
 * This class store the structure of the XSD grammar.
 */
class XsdSpecification
{

  public:
    XsdSpecification();
    ~XsdSpecification();

    void setRoot(XsdSequence *root);
    void add(XsdSequence          *sequence);
    void add(XsdEnumeration       *enumeration);
    void add(XsdChoice            *choice);
    void add(XsdInterval          *interval);
    void add(XsdRegularExpression *regExp);

    XsdSequence *root();

    std::vector<XsdSequence*>::iterator          s_begin();
    std::vector<XsdSequence*>::iterator          s_end();
    int                                          s_size();

    std::vector<XsdEnumeration*>::iterator       e_begin();
    std::vector<XsdEnumeration*>::iterator       e_end();
    int                                          e_size();

    std::vector<XsdChoice*>::iterator            c_begin();
    std::vector<XsdChoice*>::iterator            c_end();
    int                                          c_size();

    std::vector<XsdInterval*>::iterator          i_begin();
    std::vector<XsdInterval*>::iterator          i_end();
    int                                          i_size();

    std::vector<XsdRegularExpression*>::iterator r_begin();
    std::vector<XsdRegularExpression*>::iterator r_end();
    int                                          r_size();

    std::vector<XsdNode*>::iterator              n_begin();
    std::vector<XsdNode*>::iterator              n_end();
    int                                          n_size();

  private:

    XsdSequence *_root;

    std::vector<XsdSequence*>          _sequences;
    std::vector<XsdEnumeration*>       _enumerations;
    std::vector<XsdChoice*>            _choices;
    std::vector<XsdInterval*>          _intervals;
    std::vector<XsdRegularExpression*> _regExps;
    std::vector<XsdNode*>              _nodes;
};

#endif // __YARS_XSD_SPECIFICATION_H__
