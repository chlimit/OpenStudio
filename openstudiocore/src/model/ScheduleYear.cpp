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

#include "ScheduleYear.hpp"
#include "ScheduleYear_Impl.hpp"
#include "ScheduleWeek.hpp"
#include "ScheduleWeek_Impl.hpp"
#include "ScheduleTypeLimits.hpp"
#include "ScheduleTypeLimits_Impl.hpp"
#include "YearDescription.hpp"
#include "YearDescription_Impl.hpp"
#include "Model.hpp"
#include "Model_Impl.hpp"
#include "ModelExtensibleGroup.hpp"

#include <utilities/idd/OS_Schedule_Year_FieldEnums.hxx>
#include <utilities/idd/IddEnums.hxx>

#include "../utilities/core/Assert.hpp"
#include "../utilities/time/Date.hpp"

namespace openstudio {
namespace model {

namespace detail {

  ScheduleYear_Impl::ScheduleYear_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle)
    : Schedule_Impl(idfObject,model,keepHandle)
  {
    OS_ASSERT(idfObject.iddObject().type() == ScheduleYear::iddObjectType());
  }

  ScheduleYear_Impl::ScheduleYear_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                                       Model_Impl* model,
                                       bool keepHandle)
    : Schedule_Impl(other,model,keepHandle)
  {
    OS_ASSERT(other.iddObject().type() == ScheduleYear::iddObjectType());
  }

  ScheduleYear_Impl::ScheduleYear_Impl(const ScheduleYear_Impl& other,
                                       Model_Impl* model,
                                       bool keepHandle)
    : Schedule_Impl(other,model,keepHandle)
  {}

  const std::vector<std::string>& ScheduleYear_Impl::outputVariableNames() const
  {
    static std::vector<std::string> result;
    if (result.empty()){
    }
    return result;
  }

  IddObjectType ScheduleYear_Impl::iddObjectType() const {
    return ScheduleYear::iddObjectType();
  }

  boost::optional<ScheduleTypeLimits> ScheduleYear_Impl::scheduleTypeLimits() const {
    return getObject<ModelObject>().getModelObjectTarget<ScheduleTypeLimits>(OS_Schedule_YearFields::ScheduleTypeLimitsName);
  }

  std::vector<double> ScheduleYear_Impl::values() const {
    return DoubleVector();
  }

  bool ScheduleYear_Impl::setScheduleTypeLimits(const ScheduleTypeLimits& scheduleTypeLimits) {
    if (scheduleTypeLimits.model() != model()) {
      return false;
    }
    if (!candidateIsCompatibleWithCurrentUse(scheduleTypeLimits)) {
      return false;
    }
    return setPointer(OS_Schedule_YearFields::ScheduleTypeLimitsName, scheduleTypeLimits.handle());
  }

  bool ScheduleYear_Impl::resetScheduleTypeLimits() {
    if (okToResetScheduleTypeLimits()) {
      return setString(OS_Schedule_YearFields::ScheduleTypeLimitsName,"");
    }
    return false;
  }

  std::vector<openstudio::Date> ScheduleYear_Impl::dates() const
  {
    std::vector<openstudio::Date> result;

    YearDescription yd = this->model().getUniqueModelObject<YearDescription>();

    for (const ModelExtensibleGroup& group : castVector<ModelExtensibleGroup>(this->extensibleGroups()))
    {
      OptionalUnsigned month = group.getUnsigned(0, true);
      OptionalUnsigned day = group.getUnsigned(1, true);

      if (month && day){
        result.push_back(yd.makeDate(*month, *day));
      }else{
        LOG(Error, "Could not read date " << group.groupIndex() << " in " << briefDescription() << "." );
      }
    }

    return result;
  }

  std::vector<ScheduleWeek> ScheduleYear_Impl::scheduleWeeks() const
  {
    std::vector<ScheduleWeek> result;

    for (const ModelExtensibleGroup& group : castVector<ModelExtensibleGroup>(this->extensibleGroups()))
    {
      OptionalWorkspaceObject object = group.getTarget(2);
      if (object){
        boost::optional<ScheduleWeek> schedule = object->optionalCast<ScheduleWeek>();

        if (schedule){
          result.push_back(*schedule);
        }else{
          LOG(Error, "Could not read schedule " << group.groupIndex() << " in " << briefDescription() << "." );
        }
      }
    }

    return result;
  }

  boost::optional<ScheduleWeek> ScheduleYear_Impl::getScheduleWeek(const openstudio::Date& date) const
  {
    YearDescription yd = this->model().getUniqueModelObject<YearDescription>();

    if (yd.assumedYear() != date.assumedBaseYear()){
      LOG(Error, "Assumed base year " << date.assumedBaseYear() << " of date does not match this model's assumed base year of " << yd.assumedYear());
      return boost::none;
    }

    std::vector<ScheduleWeek> scheduleWeeks = this->scheduleWeeks(); // these are already sorted
    std::vector<openstudio::Date> dates = this->dates(); // these are already sorted
    
    unsigned N = dates.size();
    OS_ASSERT(scheduleWeeks.size() == N);

    if (N == 0){
      return boost::none;
    }

    boost::optional<ScheduleWeek> result;

    for (unsigned i = 0; i < N; ++i){

      // want first date which is greater than or equal to the target date
      if(dates[i] >= date){
        result = scheduleWeeks[i];
        break;
      }
    }

    return result;
  }

  bool ScheduleYear_Impl::addScheduleWeek(const openstudio::Date& untilDate, const ScheduleWeek& scheduleWeek)
  {
    YearDescription yd = this->model().getUniqueModelObject<YearDescription>();

    if (yd.assumedYear() != untilDate.assumedBaseYear()){
      LOG(Error, "Assumed base year " << untilDate.assumedBaseYear() << " of untilDate does not match this model's assumed base year of " << yd.assumedYear());
      return false;
    }

    std::vector<ScheduleWeek> scheduleWeeks = this->scheduleWeeks(); // these are already sorted
    std::vector<openstudio::Date> dates = this->dates(); // these are already sorted
    bool inserted = false;
    
    unsigned N = dates.size();
    OS_ASSERT(scheduleWeeks.size() == N);

    this->clearExtensibleGroups();

    for (unsigned i = 0; i < N; ++i){

      if (dates[i] == untilDate){

        bool doEmit = (i == (N-1));

        // push back just this schedule/date pair
        std::vector<std::string> groupValues;
        groupValues.push_back(boost::lexical_cast<std::string>(untilDate.monthOfYear().value()));
        groupValues.push_back(boost::lexical_cast<std::string>(untilDate.dayOfMonth()));
        groupValues.push_back(scheduleWeek.name().get());

        ModelExtensibleGroup group = pushExtensibleGroup(groupValues, doEmit).cast<ModelExtensibleGroup>();
        OS_ASSERT(!group.empty());

        inserted = true;

      }else{
        
        // if we need to insert new schedule/date pair here
        if ((untilDate < dates[i]) && !inserted){

          // push back this schedule/date pair
          std::vector<std::string> groupValues;
          groupValues.push_back(boost::lexical_cast<std::string>(untilDate.monthOfYear().value()));
          groupValues.push_back(boost::lexical_cast<std::string>(untilDate.dayOfMonth()));
          groupValues.push_back(scheduleWeek.name().get());

          ModelExtensibleGroup group = pushExtensibleGroup(groupValues, false).cast<ModelExtensibleGroup>();
          OS_ASSERT(!group.empty());

          inserted = true;
        }

        bool doEmit = (i == (N-1)) && inserted;

        // insert existing schedule/date pair
        std::vector<std::string> groupValues;
        groupValues.push_back(boost::lexical_cast<std::string>(dates[i].monthOfYear().value()));
        groupValues.push_back(boost::lexical_cast<std::string>(dates[i].dayOfMonth()));
        groupValues.push_back(scheduleWeeks[i].name().get());

        ModelExtensibleGroup group = pushExtensibleGroup(groupValues, doEmit).cast<ModelExtensibleGroup>();
        OS_ASSERT(!group.empty());
      }
    }

    if (!inserted){
      // push back this schedule/date pair
      std::vector<std::string> groupValues;
      groupValues.push_back(boost::lexical_cast<std::string>(untilDate.monthOfYear().value()));
      groupValues.push_back(boost::lexical_cast<std::string>(untilDate.dayOfMonth()));
      groupValues.push_back(scheduleWeek.name().get());

      ModelExtensibleGroup group = pushExtensibleGroup(groupValues, true).cast<ModelExtensibleGroup>();
      OS_ASSERT(!group.empty());
    }

    return true;
  }

  void ScheduleYear_Impl::clearScheduleWeeks()
  {
    this->clearExtensibleGroups();
  }

  void ScheduleYear_Impl::ensureNoLeapDays()
  {
    for (IdfExtensibleGroup group : this->extensibleGroups()){
      boost::optional<int> month;
      boost::optional<int> day;

      month = group.getInt(OS_Schedule_YearExtensibleFields::Month);
      if (month && (month.get() == 2)){
        day = group.getInt(OS_Schedule_YearExtensibleFields::Day);
        if (day && (day.get() == 29)){
          this->setInt(OS_Schedule_YearExtensibleFields::Day, 28);
        }
      }
    }
  }

} // detail

ScheduleYear::ScheduleYear(const Model& model)
  : Schedule(ScheduleYear::iddObjectType(),model)
{
  OS_ASSERT(getImpl<detail::ScheduleYear_Impl>());
}

IddObjectType ScheduleYear::iddObjectType() {
  IddObjectType result(IddObjectType::OS_Schedule_Year);
  return result;
}

/// @cond
ScheduleYear::ScheduleYear(std::shared_ptr<detail::ScheduleYear_Impl> impl)
  : Schedule(impl)
{}
/// @endcond

std::vector<openstudio::Date> ScheduleYear::dates() const
{
  return getImpl<detail::ScheduleYear_Impl>()->dates();
}

std::vector<ScheduleWeek> ScheduleYear::scheduleWeeks() const
{
  return getImpl<detail::ScheduleYear_Impl>()->scheduleWeeks();
}

boost::optional<ScheduleWeek> ScheduleYear::getScheduleWeek(const openstudio::Date& date) const
{
  return getImpl<detail::ScheduleYear_Impl>()->getScheduleWeek(date);
}

bool ScheduleYear::addScheduleWeek(const openstudio::Date& untilDate, const ScheduleWeek& scheduleWeek)
{
  return getImpl<detail::ScheduleYear_Impl>()->addScheduleWeek(untilDate, scheduleWeek);
}

void ScheduleYear::clearScheduleWeeks()
{
  getImpl<detail::ScheduleYear_Impl>()->clearScheduleWeeks();
}

} // model
} // openstudio

