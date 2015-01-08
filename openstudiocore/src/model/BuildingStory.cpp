/**********************************************************************
 *  Copyright (c) 2008-2015, Alliance for Sustainable Energy.
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

#include "BuildingStory.hpp"
#include "BuildingStory_Impl.hpp"

#include "Model.hpp"
#include "Model_Impl.hpp"
#include "SpaceType.hpp"
#include "SpaceType_Impl.hpp"
#include "Space.hpp"
#include "Space_Impl.hpp"
#include "DefaultConstructionSet.hpp"
#include "DefaultConstructionSet_Impl.hpp"
#include "DefaultScheduleSet.hpp"
#include "DefaultScheduleSet_Impl.hpp"
#include "RenderingColor.hpp"
#include "RenderingColor_Impl.hpp"

#include <utilities/idd/OS_BuildingStory_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

#include "../utilities/core/Assert.hpp"

namespace openstudio {
namespace model {

namespace detail {

  BuildingStory_Impl::BuildingStory_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle)
    : ModelObject_Impl(idfObject,model,keepHandle)
  {
    OS_ASSERT(idfObject.iddObject().type() == BuildingStory::iddObjectType());
  }

  BuildingStory_Impl::BuildingStory_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                                         Model_Impl* model,
                                         bool keepHandle)
    : ModelObject_Impl(other,model,keepHandle)
  {
    OS_ASSERT(other.iddObject().type() == BuildingStory::iddObjectType());
  }

  BuildingStory_Impl::BuildingStory_Impl(const BuildingStory_Impl& other,
                                         Model_Impl* model,
                                         bool keepHandle)
    : ModelObject_Impl(other,model,keepHandle)
  {}

  const std::vector<std::string>& BuildingStory_Impl::outputVariableNames() const
  {
    static std::vector<std::string> result;
    if (result.empty()){
    }
    return result;
  }

  IddObjectType BuildingStory_Impl::iddObjectType() const {
    return BuildingStory::iddObjectType();
  }

  boost::optional<double> BuildingStory_Impl::nominalZCoordinate() const {
    return getDouble(OS_BuildingStoryFields::NominalZCoordinate,true);
  }

  boost::optional<double> BuildingStory_Impl::nominalFloortoFloorHeight() const {
    boost::optional<double> value = getDouble(OS_BuildingStoryFields::NominalFloortoFloorHeight,true);
    return value;
  }

  boost::optional<double> BuildingStory_Impl::nominalFloortoCeilingHeight() const {
    boost::optional<double> value = getDouble(OS_BuildingStoryFields::NominalFloortoCeilingHeight, true);
    return value;
  }

  void BuildingStory_Impl::setNominalZCoordinate(double nominalZCoordinate) {
    bool result = setDouble(OS_BuildingStoryFields::NominalZCoordinate, nominalZCoordinate);
    OS_ASSERT(result);
  }
  
  void BuildingStory_Impl::resetNominalZCoordinate() {
    bool result = setString(OS_BuildingStoryFields::NominalZCoordinate, "");
    OS_ASSERT(result);
  }

  bool BuildingStory_Impl::setNominalFloortoFloorHeight(double nominalFloortoFloorHeight) {
    bool result = setDouble(OS_BuildingStoryFields::NominalFloortoFloorHeight, nominalFloortoFloorHeight);
    return result;
  }

  void BuildingStory_Impl::resetNominalFloortoFloorHeight() {
    bool result = setString(OS_BuildingStoryFields::NominalFloortoFloorHeight, "");
    OS_ASSERT(result);
  }

  bool BuildingStory_Impl::setNominalFloortoCeilingHeight(double nominalFloortoCeilingHeight) {
    bool result = setDouble(OS_BuildingStoryFields::NominalFloortoCeilingHeight, nominalFloortoCeilingHeight);
    return result;
  }

  void BuildingStory_Impl::resetNominalFloortoCeilingHeight() {
    bool result = setString(OS_BuildingStoryFields::NominalFloortoCeilingHeight, "");
    OS_ASSERT(result);
  }

  boost::optional<DefaultConstructionSet> BuildingStory_Impl::defaultConstructionSet() const
  {
    return getObject<ModelObject>().getModelObjectTarget<DefaultConstructionSet>(OS_BuildingStoryFields::DefaultConstructionSetName);
  }

  bool BuildingStory_Impl::setDefaultConstructionSet(const DefaultConstructionSet& defaultConstructionSet)
  {
    return setPointer(OS_BuildingStoryFields::DefaultConstructionSetName, defaultConstructionSet.handle());
  }

  void BuildingStory_Impl::resetDefaultConstructionSet()
  {
    setString(OS_BuildingStoryFields::DefaultConstructionSetName, "");
  }

  boost::optional<DefaultScheduleSet> BuildingStory_Impl::defaultScheduleSet() const
  {
    return getObject<ModelObject>().getModelObjectTarget<DefaultScheduleSet>(OS_BuildingStoryFields::DefaultScheduleSetName);
  }

  bool BuildingStory_Impl::setDefaultScheduleSet(const DefaultScheduleSet& defaultScheduleSet)
  {
    return setPointer(OS_BuildingStoryFields::DefaultScheduleSetName, defaultScheduleSet.handle());
  }

  void BuildingStory_Impl::resetDefaultScheduleSet()
  {
    setString(OS_BuildingStoryFields::DefaultScheduleSetName, "");
  }
 
  boost::optional<RenderingColor> BuildingStory_Impl::renderingColor() const
  {
    return getObject<ModelObject>().getModelObjectTarget<RenderingColor>(OS_BuildingStoryFields::GroupRenderingName);
  }

  bool BuildingStory_Impl::setRenderingColor(const RenderingColor& renderingColor)
  {
    return setPointer(OS_BuildingStoryFields::GroupRenderingName, renderingColor.handle());
  }

  void BuildingStory_Impl::resetRenderingColor()
  {
    setString(OS_BuildingStoryFields::GroupRenderingName, "");
  }

  std::vector<Space> BuildingStory_Impl::spaces() const
  {
    return getObject<ModelObject>().getModelObjectSources<Space>(
        Space::iddObjectType());
  }
  
  boost::optional<ModelObject> BuildingStory_Impl::defaultConstructionSetAsModelObject() const {
    OptionalModelObject result;
    OptionalDefaultConstructionSet object = defaultConstructionSet();
    if (object) {
      result = *object;
    }
    return result;
  }

  boost::optional<ModelObject> BuildingStory_Impl::defaultScheduleSetAsModelObject() const {
    OptionalModelObject result;
    OptionalDefaultScheduleSet object = defaultScheduleSet();
    if (object) {
      result = *object;
    }
    return result;
  }

  boost::optional<ModelObject> BuildingStory_Impl::renderingColorAsModelObject() const {
    OptionalModelObject result;
    OptionalRenderingColor object = renderingColor();
    if (object) {
      result = *object;
    }
    return result;
  }

  bool BuildingStory_Impl::setDefaultConstructionSetAsModelObject(
      boost::optional<ModelObject> modelObject)
  {
    if (modelObject) {
      OptionalDefaultConstructionSet object = modelObject->optionalCast<DefaultConstructionSet>();
      if (object) {
        return setDefaultConstructionSet(*object);
      }
      return false;
    }
    else {
      resetDefaultConstructionSet();
    }
    return true;
  }

  bool BuildingStory_Impl::setDefaultScheduleSetAsModelObject(
      boost::optional<ModelObject> modelObject)
  {
    if (modelObject) {
      OptionalDefaultScheduleSet object = modelObject->optionalCast<DefaultScheduleSet>();
      if (object) {
        return setDefaultScheduleSet(*object);
      }
      return false;
    }
    else {
      resetDefaultScheduleSet();
    }
    return true;
  }

  bool BuildingStory_Impl::setRenderingColorAsModelObject(boost::optional<ModelObject> modelObject)
  {
    if (modelObject) {
      OptionalRenderingColor object = modelObject->optionalCast<RenderingColor>();
      if (object) {
        return setRenderingColor(*object);
      }
      return false;
    }
    else {
      resetRenderingColor();
    }
    return true;
  }

} // detail

BuildingStory::BuildingStory(const Model& model)
  : ModelObject(BuildingStory::iddObjectType(), model)
{
  OS_ASSERT(getImpl<detail::BuildingStory_Impl>());
}

IddObjectType BuildingStory::iddObjectType() {
  IddObjectType result(IddObjectType::OS_BuildingStory);
  return result;
}

boost::optional<double> BuildingStory::nominalZCoordinate() const {
  return getImpl<detail::BuildingStory_Impl>()->nominalZCoordinate();
}

boost::optional<double> BuildingStory::nominalFloortoFloorHeight() const {
  return getImpl<detail::BuildingStory_Impl>()->nominalFloortoFloorHeight();
}

boost::optional<double> BuildingStory::nominalFloortoCeilingHeight() const {
  return getImpl<detail::BuildingStory_Impl>()->nominalFloortoCeilingHeight();
}

void BuildingStory::setNominalZCoordinate(double nominalZCoordinate) {
  getImpl<detail::BuildingStory_Impl>()->setNominalZCoordinate(nominalZCoordinate);
}

void BuildingStory::resetNominalZCoordinate() {
  getImpl<detail::BuildingStory_Impl>()->resetNominalZCoordinate();
}

bool BuildingStory::setNominalFloortoFloorHeight(double nominalFloortoFloorHeight) {
  return getImpl<detail::BuildingStory_Impl>()->setNominalFloortoFloorHeight(nominalFloortoFloorHeight);
}

void BuildingStory::resetNominalFloortoFloorHeight() {
  getImpl<detail::BuildingStory_Impl>()->resetNominalFloortoFloorHeight();
}

bool BuildingStory::setNominalFloortoCeilingHeight(double nominalFloortoCeilingHeight) {
  return getImpl<detail::BuildingStory_Impl>()->setNominalFloortoCeilingHeight(nominalFloortoCeilingHeight);
}

void BuildingStory::resetNominalFloortoCeilingHeight() {
  getImpl<detail::BuildingStory_Impl>()->resetNominalFloortoCeilingHeight();
}

boost::optional<DefaultConstructionSet> BuildingStory::defaultConstructionSet() const
{
  return getImpl<detail::BuildingStory_Impl>()->defaultConstructionSet();
}

bool BuildingStory::setDefaultConstructionSet(const DefaultConstructionSet& defaultConstructionSet)
{
  return getImpl<detail::BuildingStory_Impl>()->setDefaultConstructionSet(defaultConstructionSet);
}

void BuildingStory::resetDefaultConstructionSet()
{
  getImpl<detail::BuildingStory_Impl>()->resetDefaultConstructionSet();
}

boost::optional<DefaultScheduleSet> BuildingStory::defaultScheduleSet() const
{
  return getImpl<detail::BuildingStory_Impl>()->defaultScheduleSet();
}

bool BuildingStory::setDefaultScheduleSet(const DefaultScheduleSet& defaultScheduleSet)
{
  return getImpl<detail::BuildingStory_Impl>()->setDefaultScheduleSet(defaultScheduleSet);
}

void BuildingStory::resetDefaultScheduleSet()
{
  getImpl<detail::BuildingStory_Impl>()->resetDefaultScheduleSet();
}

boost::optional<RenderingColor> BuildingStory::renderingColor() const
{
  return getImpl<detail::BuildingStory_Impl>()->renderingColor();
}

bool BuildingStory::setRenderingColor(const RenderingColor& renderingColor)
{
  return getImpl<detail::BuildingStory_Impl>()->setRenderingColor(renderingColor);
}

void BuildingStory::resetRenderingColor()
{
  getImpl<detail::BuildingStory_Impl>()->resetRenderingColor();
}

std::vector<Space> BuildingStory::spaces() const
{
  return getImpl<detail::BuildingStory_Impl>()->spaces();
}

/// @cond
BuildingStory::BuildingStory(std::shared_ptr<detail::BuildingStory_Impl> impl)
  : ModelObject(impl)
{}
/// @endcond


} // model
} // openstudio

