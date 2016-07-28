#include "DataSoftBodyParameters.h"

#define YARS_STRING_VCF        (char*)"velocities_correction_factor"
#define YARS_STRING_DP         (char*)"damping_coefficient"
#define YARS_STRING_DG         (char*)"drag_coefficient"
#define YARS_STRING_LF         (char*)"lift_coefficient"
#define YARS_STRING_PR         (char*)"pressure_coefficient"
#define YARS_STRING_VC         (char*)"volume_conversation_coefficient"
#define YARS_STRING_DF         (char*)"dynamic_friction_coefficient"
#define YARS_STRING_MT         (char*)"pose_matching_coefficient"
#define YARS_STRING_CHR        (char*)"rigid_contacts_hardness"
#define YARS_STRING_KHR        (char*)"kinetic_contacts_hardness"
#define YARS_STRING_SHR        (char*)"soft_contacts_hardness"
#define YARS_STRING_AHR        (char*)"anchors_hardness"
#define YARS_STRING_SRHR_CL    (char*)"soft_vs_rigid_hardness"
#define YARS_STRING_SKHR_CL    (char*)"soft_vs_kinetic_hardness"
#define YARS_STRING_SSHR_CL    (char*)"soft_vs_soft_hardness"
#define YARS_STRING_SR_SPLT_CL (char*)"soft_vs_rigid_impulse_split"
#define YARS_STRING_SK_SPLT_CL (char*)"soft_vs_rigid_impulse_split"
#define YARS_STRING_SS_SPLT_CL (char*)"soft_vs_rigid_impulse_split"
#define YARS_STRING_MV         (char*)"maximum_volume_ratio_for_pose"
#define YARS_STRING_TS         (char*)"time_scale"
#define YARS_STRING_VI         (char*)"velocities_solver_iterations"
#define YARS_STRING_PI         (char*)"positions_solver_iterations"
#define YARS_STRING_DI         (char*)"drift_solver_iterations"
#define YARS_STRING_CI         (char*)"cluster_solver_iterations"
#define YARS_STRING_MATCHING   (char*)"matching"
#define YARS_STRING_DAMPING    (char*)"damping"
#define YARS_STRING_LST        (char*)"linear_stiffness"
#define YARS_STRING_AST        (char*)"angular_stiffness"
#define YARS_STRING_VST        (char*)"volume_stiffness"


DataSoftBodyParameters::DataSoftBodyParameters(DataNode *parent)
  : DataNode(parent)
{
  kLST        = 1.0; // Linear stiffness
  kAST        = 1.0; // Angular stiffness
  kVST        = 1.0; // Volume stiffness
  kVCF        = 1;   // Velocities correction factor (Baumgarte)
  kDP         = 0;   // Damping coefficient [0,1]
  kDG         = 0;   // Drag coefficient [0,+inf]
  kLF         = 0;   // Lift coefficient [0,+inf]
  kPR         = 0;   // Pressure coefficient [-inf,+inf]
  kVC         = 0;   // Volume conversation coefficient [0,+inf]
  kDF         = 0.2; // Dynamic friction coefficient [0,1]
  kMT         = 0;   // Pose matching coefficient [0,1]
  kCHR        = 1.0; // Rigid contacts hardness [0,1]
  kKHR        = 0.1; // Kinetic contacts hardness [0,1]
  kSHR        = 1.0; // Soft contacts hardness [0,1]
  kAHR        = 0.7; // Anchors hardness [0,1]
  kSRHR_CL    = 0.1; // Soft vs rigid hardness [0,1] (cluster only)
  kSKHR_CL    = 1.0; // Soft vs kinetic hardness [0,1] (cluster only)
  kSSHR_CL    = 0.5; // Soft vs soft hardness [0,1] (cluster only)
  kSR_SPLT_CL = 0.5; // Soft vs rigid impulse split [0,1] (cluster only)
  kSK_SPLT_CL = 0.5; // Soft vs rigid impulse split [0,1] (cluster only)
  kSS_SPLT_CL = 0.5; // Soft vs rigid impulse split [0,1] (cluster only)
  maxvolume   = 1.0; // Maximum volume ratio for pose
  timescale   = 1.0; // Time scale
  viterations = 0;   // Velocities solver iterations
  piterations = 1;   // Positions solver iterations
  diterations = 0;   // Drift solver iterations
  citerations = 4;   // Cluster solver iterations

  matching    = 0.05;
  ndamping    = 0.15;
}

void DataSoftBodyParameters::createXsd(XsdSpecification *spec)
{
  XsdSequence *def = new XsdSequence(YARS_STRING_SOFT_BODY_PARAMETER_DEFINITION);
  def->add(NA(YARS_STRING_VCF,        YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  def->add(NA(YARS_STRING_DP,         YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_DG,         YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  def->add(NA(YARS_STRING_LF,         YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  def->add(NA(YARS_STRING_PR,         YARS_STRING_XSD_DECIMAL,               false));
  def->add(NA(YARS_STRING_VC,         YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  def->add(NA(YARS_STRING_DF,         YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_MT,         YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_CHR,        YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_KHR,        YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_SHR,        YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_AHR,        YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_SRHR_CL,    YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_SKHR_CL,    YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_SSHR_CL,    YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_SR_SPLT_CL, YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_SK_SPLT_CL, YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_SS_SPLT_CL, YARS_STRING_UNIT_INTERVAL,             false));
  def->add(NA(YARS_STRING_MV,         YARS_STRING_XSD_DECIMAL,               false));
  def->add(NA(YARS_STRING_TS,         YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  def->add(NA(YARS_STRING_VI,         YARS_STRING_POSITIVE_INTEGER,          false));
  def->add(NA(YARS_STRING_PI,         YARS_STRING_POSITIVE_INTEGER,          false));
  def->add(NA(YARS_STRING_DI,         YARS_STRING_POSITIVE_INTEGER,          false));
  def->add(NA(YARS_STRING_CI,         YARS_STRING_POSITIVE_INTEGER,          false));
  def->add(NA(YARS_STRING_MATCHING,   YARS_STRING_POSITIVE_DECIMAL,          false));
  def->add(NA(YARS_STRING_DAMPING,    YARS_STRING_POSITIVE_DECIMAL,          false));
  def->add(NA(YARS_STRING_LST,        YARS_STRING_POSITIVE_DECIMAL,          false));
  def->add(NA(YARS_STRING_AST,        YARS_STRING_POSITIVE_DECIMAL,          false));
  def->add(NA(YARS_STRING_VST,        YARS_STRING_POSITIVE_DECIMAL,          false));


  spec->add(def);
}

void DataSoftBodyParameters::add(DataParseElement *element)
{
  if(element->opening(YARS_STRING_SOFT_BODY_PARAMETER))
  {
    element->set(YARS_STRING_VCF,        kVCF);
    element->set(YARS_STRING_DP,         kDP);
    element->set(YARS_STRING_DG,         kDG);
    element->set(YARS_STRING_LF,         kLF);
    element->set(YARS_STRING_PR,         kPR);
    element->set(YARS_STRING_VC,         kVC);
    element->set(YARS_STRING_DF,         kDF);
    element->set(YARS_STRING_MT,         kMT);
    element->set(YARS_STRING_CHR,        kCHR);
    element->set(YARS_STRING_KHR,        kKHR);
    element->set(YARS_STRING_SHR,        kSHR);
    element->set(YARS_STRING_AHR,        kAHR);
    element->set(YARS_STRING_SRHR_CL,    kSRHR_CL);
    element->set(YARS_STRING_SKHR_CL,    kSKHR_CL);
    element->set(YARS_STRING_SSHR_CL,    kSSHR_CL);
    element->set(YARS_STRING_SR_SPLT_CL, kSR_SPLT_CL);
    element->set(YARS_STRING_SK_SPLT_CL, kSK_SPLT_CL);
    element->set(YARS_STRING_SS_SPLT_CL, kSS_SPLT_CL);
    element->set(YARS_STRING_MV,         maxvolume);
    element->set(YARS_STRING_TS,         timescale);
    element->set(YARS_STRING_VI,         viterations);
    element->set(YARS_STRING_PI,         piterations);
    element->set(YARS_STRING_DI,         diterations);
    element->set(YARS_STRING_CI,         citerations);
    element->set(YARS_STRING_MATCHING,   matching);
    element->set(YARS_STRING_DAMPING,    ndamping);
    element->set(YARS_STRING_LST,        kLST);
    element->set(YARS_STRING_AST,        kAST);
    element->set(YARS_STRING_VST,        kVST);

    cout << "piterations: " << piterations << endl;
  }
  if(element->closing(YARS_STRING_SOFT_BODY_PARAMETER))
  {
    current = parent;
  }
}

DataSoftBodyParameters* DataSoftBodyParameters::copy()
{
  DataSoftBodyParameters *copy = new DataSoftBodyParameters(NULL);
  copy->kVCF        = kVCF;
  copy->kDP         = kDP;
  copy->kDG         = kDG;
  copy->kLF         = kLF;
  copy->kPR         = kPR;
  copy->kVC         = kVC;
  copy->kDF         = kDF;
  copy->kMT         = kMT;
  copy->kCHR        = kCHR;
  copy->kKHR        = kKHR;
  copy->kSHR        = kSHR;
  copy->kAHR        = kAHR;
  copy->kSRHR_CL    = kSRHR_CL;
  copy->kSKHR_CL    = kSKHR_CL;
  copy->kSSHR_CL    = kSSHR_CL;
  copy->kSR_SPLT_CL = kSR_SPLT_CL;
  copy->kSK_SPLT_CL = kSK_SPLT_CL;
  copy->kSS_SPLT_CL = kSS_SPLT_CL;
  copy->maxvolume   = maxvolume;
  copy->timescale   = timescale;
  copy->viterations = viterations;
  copy->piterations = piterations;
  copy->diterations = diterations;
  copy->citerations = citerations;
  copy->matching    = matching;
  copy->ndamping    = ndamping;
  copy->kLST        = kLST;
  copy->kAST        = kAST;
  copy->kVST        = kVST;

    cout << "copy piterations: " << piterations << endl;
  return copy;
}

void DataSoftBodyParameters::resetTo(const DataSoftBodyParameters *other)
{
  kVCF        = other->kVCF;
  kDP         = other->kDP;
  kDG         = other->kDG;
  kLF         = other->kLF;
  kPR         = other->kPR;
  kVC         = other->kVC;
  kDF         = other->kDF;
  kMT         = other->kMT;
  kCHR        = other->kCHR;
  kKHR        = other->kKHR;
  kSHR        = other->kSHR;
  kAHR        = other->kAHR;
  kSRHR_CL    = other->kSRHR_CL;
  kSKHR_CL    = other->kSKHR_CL;
  kSSHR_CL    = other->kSSHR_CL;
  kSR_SPLT_CL = other->kSR_SPLT_CL;
  kSK_SPLT_CL = other->kSK_SPLT_CL;
  kSS_SPLT_CL = other->kSS_SPLT_CL;
  maxvolume   = other->maxvolume;
  timescale   = other->timescale;
  viterations = other->viterations;
  piterations = other->piterations;
  diterations = other->diterations;
  citerations = other->citerations;
  matching    = other->matching;
  ndamping    = other->ndamping;
  kLST        = other->kLST;
  kAST        = other->kAST;
  kVST        = other->kVST;
}
