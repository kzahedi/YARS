#ifndef __DATA_SOFT_BODY_PARAMETERS_H__
#define __DATA_SOFT_BODY_PARAMETERS_H__

#include "DataNode.h"

# define YARS_STRING_SOFT_BODY_PARAMETER            (char*)"parameter"
# define YARS_STRING_SOFT_BODY_PARAMETER_DEFINITION (char*)"soft_body_parameter" DIVIDER DEFINITION


class DataSoftBodyParameters : public DataNode
{
  public:
    DataSoftBodyParameters(DataNode *parent);
    // ~DataSoftBodyParameters();

    //DataSoftBodyParameters(const DataSoftBodyParameters);
    //DataSoftBodyParameters operator=(const DataSoftBodyParameters);

    static void createXsd(XsdSpecification *spec);
    void add(DataParseElement *element);

    DataSoftBodyParameters *copy();

    void resetTo(const DataSoftBodyParameters *other);

    double kVCF;
    double kDP;
    double kDG;
    double kLF;
    double kPR;
    double kVC;
    double kDF;
    double kMT;
    double kCHR;
    double kKHR;
    double kSHR;
    double kAHR;
    double kSRHR_CL;
    double kSKHR_CL;
    double kSSHR_CL;
    double kSR_SPLT_CL;
    double kSK_SPLT_CL;
    double kSS_SPLT_CL;
    double maxvolume;
    double timescale;
    double viterations;
    double piterations;
    double diterations;
    double citerations;

    double kLST;
    double kAST;
    double kVST;

    double matching;
    double ndamping;

};


#endif // __DATA_SOFT_BODY_PARAMETERS_H__
