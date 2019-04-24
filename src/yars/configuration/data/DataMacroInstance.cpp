#include "DataMacroInstance.h"
#include "DataMacro.h"
#include "DataRobot.h"
#include "DataPoseFactory.h"

# define YARS_STRING_POSE              (char*)"pose"
# define YARS_STRING_TARGET            (char*)"target"
# define YARS_STRING_NAME              (char*)"name"
# define YARS_STRING_OBJECT            (char*)"object"
# define YARS_STRING_REF               (char*)"ref"
# define YARS_STRING_POSE_DEFINITION   (char*)"pose_definition"
# define YARS_STRING_TARGET_DEFINITION (char*)"macro_instance_target_definition"

# define YARS_STRING_FIRST_TEXTURE     (char*)"first"
# define YARS_STRING_SECOND_TEXTURE    (char*)"second"
# define YARS_STRING_THIRD_TEXTURE     (char*)"third"
# define YARS_STRING_FOURTH_TEXTURE    (char*)"fourth"
# define YARS_STRING_FIFTH_TEXTURE     (char*)"fifth"
# define YARS_STRING_SIXTH_TEXTURE     (char*)"sixth"
# define YARS_STRING_TOP_TEXTURE       (char*)"top"
# define YARS_STRING_BOTTOM_TEXTURE    (char*)"bottom"
# define YARS_STRING_BODY_TEXTURE      (char*)"body"
# define YARS_STRING_TEXTURE           (char*)"texture"
# define YARS_STRING_NAME_DEFINITION   (char*)"name_definition"

# define OPTION_1                      (char*)"option1"
# define OPTION_2                      (char*)"option2"
# define OPTION_3                      (char*)"option3"


DataMacroInstance::DataMacroInstance(DataNode *parent, DataMacros *macros)
  : DataNode(parent)
{
  _macros       = macros;
  _target       = "";
}

void DataMacroInstance::add(DataParseElement *element)
{
  if(element->opening(YARS_STRING_OBJECT_MACRO))
  {
    string ref;
    element->set(YARS_STRING_NAME, _prefix);
    element->set(YARS_STRING_REF,  ref);

    if(_macros != NULL)
    {
      for(std::vector<DataMacro*>::iterator i = _macros->begin(); i != _macros->end(); i++)
      {
        if((*i)->name() == ref)
        {
          for(DataObjects::iterator j = (*i)->begin(); j != (*i)->end(); j++)
          {
            DataObject *copy = (*j)->copy();
            string name = _prefix + (*j)->name();
            copy->setName(name);
            if(copy->type() == DATA_OBJECT_COMPOSITE)
            {
              DataComposite *composite = (DataComposite*)copy;
              for(DataObjects::iterator o = composite->g_begin(); o != composite->g_end(); o++)
              {
                string geom_name = _prefix + (*o)->name();
                (*o)->setName(geom_name);
              }
            }
            push_back(copy);
          }
        }
      }
    }
  }

  if(element->closing(YARS_STRING_OBJECT_MACRO))
  {
    for(DataObjects::iterator i = begin(); i != end(); i++)
    {
      if((*i)->type() == DATA_OBJECT_COMPOSITE)
      {
        ((DataComposite*)(*i))->applyOffset(_pose);
      }
      else
      {
        (*i)->applyOffset(_pose);
      }
    }
    if(_textures.size() > 0)
    {
      for(DataObjects::iterator i = begin(); i != end(); i++)
      {
        if((*i)->type() == DATA_OBJECT_COMPOSITE)
        {
          DataComposite *composite = (DataComposite*)(*i);
          for(DataObjects::iterator o = composite->g_begin(); o != composite->g_end(); o++)
          {
            if((*o)->name() == _target) __setTexture(*o);
          }
        }
        else
        {
            if((*i)->name() == _target) __setTexture(*i);
        }
      }
    }
    current = parent;
  }

  if(element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
  }

  if(element->opening(YARS_STRING_TARGET))
  {
    element->set(YARS_STRING_OBJECT, _target);
    _target = _prefix + _target;
  }

  if(element->opening(YARS_STRING_FIRST_TEXTURE))  _textures.push_back(element->attribute(YARS_STRING_NAME)->value());
  if(element->opening(YARS_STRING_SECOND_TEXTURE)) _textures.push_back(element->attribute(YARS_STRING_NAME)->value());
  if(element->opening(YARS_STRING_THIRD_TEXTURE))  _textures.push_back(element->attribute(YARS_STRING_NAME)->value());
  if(element->opening(YARS_STRING_FOURTH_TEXTURE)) _textures.push_back(element->attribute(YARS_STRING_NAME)->value());
  if(element->opening(YARS_STRING_FIFTH_TEXTURE))  _textures.push_back(element->attribute(YARS_STRING_NAME)->value());
  if(element->opening(YARS_STRING_SIXTH_TEXTURE))  _textures.push_back(element->attribute(YARS_STRING_NAME)->value());

  if(element->opening(YARS_STRING_TOP_TEXTURE))    _textures.push_back(element->attribute(YARS_STRING_NAME)->value());
  if(element->opening(YARS_STRING_BOTTOM_TEXTURE)) _textures.push_back(element->attribute(YARS_STRING_NAME)->value());
  if(element->opening(YARS_STRING_BODY_TEXTURE))   _textures.push_back(element->attribute(YARS_STRING_NAME)->value());

  if(element->opening(YARS_STRING_TEXTURE))        _textures.push_back(element->attribute(YARS_STRING_NAME)->value());

}

void DataMacroInstance::createXsd(XsdSpecification *spec)
{
  XsdSequence *macroDefinition = new XsdSequence(YARS_STRING_OBJECT_MACRO_DEFINTION);
  macroDefinition->add(NA(YARS_STRING_NAME,   YARS_STRING_XSD_STRING,      true));
  macroDefinition->add(NA(YARS_STRING_REF,    YARS_STRING_XSD_STRING,      true));
  macroDefinition->add(XE(YARS_STRING_POSE,   YARS_STRING_POSE_DEFINITION, 1, 1));
  macroDefinition->add(XE(YARS_STRING_TARGET, YARS_STRING_TARGET_DEFINITION, 0));
  spec->add(macroDefinition);

  XsdChoice *targetDefinition = new XsdChoice(YARS_STRING_TARGET_DEFINITION);
  targetDefinition->add(NA(YARS_STRING_OBJECT, YARS_STRING_XSD_STRING, true));
  spec->add(targetDefinition);

  XsdSequence *option1 = new XsdSequence(OPTION_1);
  option1->add(XE(YARS_STRING_FIRST_TEXTURE,  YARS_STRING_NAME_DEFINITION, 1, 1));
  option1->add(XE(YARS_STRING_SECOND_TEXTURE, YARS_STRING_NAME_DEFINITION, 1, 1));
  option1->add(XE(YARS_STRING_THIRD_TEXTURE,  YARS_STRING_NAME_DEFINITION, 1, 1));
  option1->add(XE(YARS_STRING_FOURTH_TEXTURE, YARS_STRING_NAME_DEFINITION, 1, 1));
  option1->add(XE(YARS_STRING_FIFTH_TEXTURE,  YARS_STRING_NAME_DEFINITION, 1, 1));
  option1->add(XE(YARS_STRING_SIXTH_TEXTURE,  YARS_STRING_NAME_DEFINITION, 1, 1));
  targetDefinition->add(option1);

  XsdSequence *option2 = new XsdSequence(OPTION_2);
  option2->add(XE(YARS_STRING_TOP_TEXTURE,    YARS_STRING_NAME_DEFINITION, 1, 1));
  option2->add(XE(YARS_STRING_BOTTOM_TEXTURE, YARS_STRING_NAME_DEFINITION, 1, 1));
  option2->add(XE(YARS_STRING_BODY_TEXTURE,   YARS_STRING_NAME_DEFINITION, 1, 1));
  targetDefinition->add(option2);

  XsdSequence *option3 = new XsdSequence(OPTION_3);
  option3->add(XE(YARS_STRING_TEXTURE,  YARS_STRING_NAME_DEFINITION, 1, 1));
  targetDefinition->add(option3);

}


DataMacroInstance* DataMacroInstance::copy(DataNode *parent)
{
  DataMacroInstance *copy = new DataMacroInstance(parent, NULL);
  copy->_macros = _macros->copy();
  copy->_pose = _pose;
  copy->_target = _target;
  copy->_prefix = _prefix;
  for(int i = 0; i < (int)_textures.size(); i++)
  {
    copy->_textures.push_back(_textures[i]);
  }
  return copy;
}


string DataMacroInstance::texture(int index)
{
  return _textures[index];
}

void DataMacroInstance::__setTexture(DataObject *o)
{
  DataBox            *box;
  DataSphere         *sphere;
  DataCapsule *capped;
  DataCylinder       *cylinder;
  switch(o->type())
  {
    case DATA_OBJECT_BOX:
      box = (DataBox*)o;
      for(int i = 0; i < 6; i++) box->setTexture(i, _textures[i % _textures.size()]);
      break;
    case DATA_OBJECT_SPHERE:
      sphere = (DataSphere*)o;
      sphere->setTexture(_textures[0]);
      break;
    case DATA_OBJECT_CAPPED_CYLINDER:
      capped = (DataCapsule*)o;
      for(int i = 0; i < (int)_textures.size(); i++) capped->setTexture(i, _textures[i]);
      break;
    case DATA_OBJECT_CYLINDER:
      cylinder = (DataCylinder*)o;
      for(int i = 0; i < (int)_textures.size(); i++) cylinder->setTexture(i, _textures[i]);
      break;
  }

}
