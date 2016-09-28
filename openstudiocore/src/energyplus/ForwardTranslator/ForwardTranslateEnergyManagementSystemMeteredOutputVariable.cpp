/**********************************************************************
 *  Copyright (c) 2008-2016, Alliance for Sustainable Energy.
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#include "../ForwardTranslator.hpp"

#include "../../model/Model.hpp"
#include "../../model/OutputVariable.hpp"
#include "../../model/OutputVariable_Impl.hpp"
#include "../../model/OutputMeter.hpp"
#include "../../model/OutputMeter_Impl.hpp"
#include "../../model/Schedule.hpp"
#include "../../model/Schedule_Impl.hpp"
#include "../../model/EnergyManagementSystemMeteredOutputVariable.hpp"
#include "../../model/EnergyManagementSystemMeteredOutputVariable_Impl.hpp"

#include <utilities/idd/EnergyManagementSystem_MeteredOutputVariable_FieldEnums.hxx>
#include "../../utilities/idd/IddEnums.hpp"
#include <utilities/idd/IddEnums.hxx>

using namespace openstudio::model;

using namespace std;

namespace openstudio {

namespace energyplus {

boost::optional<IdfObject> ForwardTranslator::translateEnergyManagementSystemMeteredOutputVariable(EnergyManagementSystemMeteredOutputVariable & modelObject)
{
  boost::optional<std::string> s;

  IdfObject idfObject(openstudio::IddObjectType::EnergyManagementSystem_MeteredOutputVariable);
  m_idfObjects.push_back(idfObject);
  //Name
  s = modelObject.name();
  if (s) {
    idfObject.setName(*s);
  }
 
  idfObject.setString(EnergyManagementSystem_MeteredOutputVariableFields::EMSVariableName, modelObject.eMSVariableName());

  idfObject.setString(EnergyManagementSystem_MeteredOutputVariableFields::UpdateFrequency, modelObject.updateFrequency());
  
  s = modelObject.eMSProgramorSubroutineName();
  if (s.is_initialized()) {
    idfObject.setString(EnergyManagementSystem_MeteredOutputVariableFields::EMSProgramorSubroutineName, s.get());
  }
  idfObject.setString(EnergyManagementSystem_MeteredOutputVariableFields::ResourceType, modelObject.resourceType());

  idfObject.setString(EnergyManagementSystem_MeteredOutputVariableFields::GroupType, modelObject.groupType());

  idfObject.setString(EnergyManagementSystem_MeteredOutputVariableFields::EndUseCategory, modelObject.endUseCategory());
  
  s = modelObject.endUseSubcategory();
  if (s.is_initialized()) {
    idfObject.setString(EnergyManagementSystem_MeteredOutputVariableFields::EndUseSubcategory, s.get());
  }

  s = modelObject.units();
  if (s.is_initialized()) {
    idfObject.setString(EnergyManagementSystem_MeteredOutputVariableFields::Units, s.get());
  }
  return idfObject;
}

} // energyplus

} // openstudio
