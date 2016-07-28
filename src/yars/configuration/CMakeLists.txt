#########################################################################
#                                                                       #
# This file is part of Yet Another Robot Simulator (YARS).              #
# Copyright (C) 2003-2015 Keyan Ghazi-Zahedi.                           #
# All rights reserved.                                                  #
# Email: keyan.zahedi@googlemail.com                                    #
# Web: https://github.com/kzahedi/YARS                                  #
#                                                                       #
# For a list of contributors see the file AUTHORS.                      #
#                                                                       #
# YARS is free software; you can redistribute it and/or modify it under #
# the terms of the GNU General Public License as published by the Free  #
# Software Foundation; either version 2 of the License, or (at your     #
# option) any later version.                                            #
#                                                                       #
# YARS is distributed in the hope that it will be useful, but WITHOUT   #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or #
# FITNESS FOR A PARTICULAR PURPOSE.                                     #
#                                                                       #
# You should have received a copy of the GNU General Public License     #
# along with YARS in the file COPYING; if not, write to the Free        #
# Software Foundation, Inc., 51 Franklin St, Fifth Floor,               #
# Boston, MA 02110-1301, USA                                            #
#                                                                       #
#########################################################################

# add_library(YarsConfiguration ${LIBRARY_TYPE} YarsConfiguration.cpp
set(CFG_SRC YarsConfiguration.cpp
            container/KeyboardShortcut.cpp
            container/KeyboardShortcuts.cpp
            container/ConfigurationContainer.cpp
            container/ProgramOptionsConfiguration.cpp
            ProgramOptions.cpp

            data/Data.cpp
            data/DataActuator.cpp
            data/DataActuatorFactory.cpp
            data/DataBox.cpp
            data/DataCamera.cpp
            data/DataCameraConfig.cpp
            data/DataCapsule.cpp
            data/DataCenterOrbitCam.cpp
            data/DataComment.cpp
            data/DataComposite.cpp
            data/DataConstraints.cpp
            data/DataController.cpp
            data/DataCylinder.cpp
            data/DataEnvironment.cpp
            data/DataFilter.cpp
            data/DataFixedActuator.cpp
            data/DataFollowables.cpp
            data/DataGenericActuator.cpp
            data/DataGenericActuatorSensor.cpp
            data/DataGenericAmbientLightSensor.cpp
            data/DataGenericBinaryContactSensor.cpp
            data/DataGenericCameraSensor.cpp
            data/DataGenericDeflectionSensor.cpp
            data/DataGenericFeedbackSensor.cpp
            data/DataGenericInternalEnergySensor.cpp
            data/DataGenericLightDependentResistorSensor.cpp
            data/DataGenericOrientationSensor.cpp
            data/DataGenericPositionSensor.cpp
            data/DataGenericProximitySensor.cpp
            data/DataGenericSignalSensor.cpp
            data/DataGenericVelocitySensor.cpp
            data/DataHingeActuator.cpp
            data/DataPointLightSource.cpp
            data/DataAmbientLight.cpp
            data/DataLogging.cpp
            data/DataLoggingActuator.cpp
            data/DataLoggingBlender.cpp
            data/DataLoggingConsole.cpp
            data/DataLoggingController.cpp
            data/DataLoggingFile.cpp
            data/DataLoggingCSV.cpp
            data/DataLoggingGnuplot.cpp
            data/DataLoggingModule.cpp
            data/DataLoggingObject.cpp
            data/DataLoggingSelforg.cpp
            data/DataLoggingSensor.cpp
            data/DataMacro.cpp
            data/DataMacroInstance.cpp
            data/DataMacros.cpp
            data/DataNode.cpp
            data/DataNoise.cpp
            data/DataObject.cpp
            data/DataObjectFactory.cpp
            data/DataObjectAngularVelocitySensor.cpp
            data/DataObjectVelocitySensor.cpp
            data/DataOffsetCam.cpp
            data/DataOrbitCam.cpp
            data/DataParameter.cpp
            data/DataParseAttribute.cpp
            data/DataParseElement.cpp
            data/DataPhysicsParameter.cpp
            data/DataRecording.cpp
            data/DataRobot.cpp
            data/DataRobotSimulationDescription.cpp
            data/DataRobots.cpp
            data/DataScreen.cpp
            data/DataScreens.cpp
            data/DataSensor.cpp
            data/DataSensorFactory.cpp
            data/DataSharpDM2Y3A003K0FSensor.cpp
            data/DataSharpGP2D12_37Sensor.cpp
            data/DataSignalKeyboard.cpp
            data/DataSignalPeriodic.cpp
            data/DataSignalTriggered.cpp
            data/DataSignals.cpp
            data/DataSimulator.cpp
            data/DataSliderActuator.cpp
            data/DataSphere.cpp
            data/DataTrace.cpp
            data/DataTraceLine.cpp
            data/DataTracePoint.cpp
            data/DataTraces.cpp
            data/XmlChangeLog.cpp
            data/XmlChangeLogEntry.cpp
            data/DataPly.cpp
            data/DataMeshVisualisation.cpp
            data/DataSoftBodyParameters.cpp

            xsd/generator/YarsXSDGenerator.cpp
            xsd/generator/XSDString.cpp
            xsd/graphviz/graph/XsdGraph.cpp
            xsd/graphviz/graph/XsdGraphLink.cpp
            xsd/graphviz/graph/XsdSequenceGraphNode.cpp
            xsd/graphviz/graph/XsdChoiceGraphNode.cpp
            xsd/graphviz/graph/XsdRegularExpressionGraphNode.cpp
            xsd/graphviz/graph/XsdEnumerationGraphNode.cpp
            xsd/graphviz/graph/XsdIntervalGraphNode.cpp
            xsd/graphviz/graph/XsdGraphNodeInstance.cpp
            xsd/graphviz/graph/XsdElementGraphNode.cpp
            xsd/graphviz/graph/XsdGraphNode.cpp
            xsd/graphviz/XsdGraphvizGenerator.cpp
            xsd/graphviz/XsdGraphvizExporter.cpp
            xsd/parser/YarsXSDSaxParser.cpp
            xsd/parser/YarsXSDSaxHandler.cpp
            xsd/specification/XsdSpecification.cpp
            xsd/specification/XsdSequence.cpp
            xsd/specification/XsdElement.cpp
            xsd/specification/XsdRegularExpression.cpp
            xsd/specification/XsdChoice.cpp
            xsd/specification/XsdInterval.cpp
            xsd/specification/XsdNode.cpp
            xsd/specification/XsdEnumeration.cpp
            xsd/specification/XsdAttribute.cpp)


add_library(YarsConfiguration STATIC ${CFG_SRC})
target_link_libraries(YarsConfiguration ${XERCESC_LIBRARY_FILE} ${Boost_LIBRARIES} YarsUtil)
