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

    yReal kVCF;
    yReal kDP;
    yReal kDG;
    yReal kLF;
    yReal kPR;
    yReal kVC;
    yReal kDF;
    yReal kMT;
    yReal kCHR;
    yReal kKHR;
    yReal kSHR;
    yReal kAHR;
    yReal kSRHR_CL;
    yReal kSKHR_CL;
    yReal kSSHR_CL;
    yReal kSR_SPLT_CL;
    yReal kSK_SPLT_CL;
    yReal kSS_SPLT_CL;
    yReal maxvolume;
    yReal timescale;
    yReal viterations;
    yReal piterations;
    yReal diterations;
    yReal citerations;

    yReal kLST;
    yReal kAST;
    yReal kVST;

    yReal matching;
    yReal ndamping;

};


#endif // __DATA_SOFT_BODY_PARAMETERS_H__
