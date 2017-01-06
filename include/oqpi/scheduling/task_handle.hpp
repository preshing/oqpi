#pragma once

#include <string>

#include "oqpi/error_handling.hpp"
#include "oqpi/scheduling/task.hpp"
#include "oqpi/scheduling/task_base.hpp"
#include "oqpi/scheduling/task_type.hpp"


namespace oqpi {

    //----------------------------------------------------------------------------------------------
    class task_handle
    {
    public:
        //------------------------------------------------------------------------------------------
        // Constructs an invalid handle
        task_handle()
            : spTask_(nullptr)
        {}

        //------------------------------------------------------------------------------------------
        // Construct a valid handle by taking a shared pointer to a task_base
        task_handle(task_sptr spTask)
            : spTask_(std::move(spTask))
        {}
        
        //------------------------------------------------------------------------------------------
        // Construct a valid handle by taking a shared pointer to a task
        template<task_type _TaskType, typename _TaskContext, typename _Tuple, typename _ReturnType>
        task_handle(std::shared_ptr<task<_TaskType, _TaskContext, _Tuple, _ReturnType>> spTask)
            : spTask_(std::move(spTask))
        {}

        //------------------------------------------------------------------------------------------
        // Movable
        task_handle(task_handle &&rhs)
            : spTask_(std::move(rhs.spTask_))
        {}

        task_handle& operator =(task_handle &&rhs)
        {
            if (this != &rhs)
            {
                spTask_ = std::move(rhs.spTask_);
            }
            return (*this);
        }

        //------------------------------------------------------------------------------------------
        // Copyable as well
        task_handle(const task_handle &rhs)
            : spTask_(rhs.spTask_)
        {}

        task_handle& operator =(const task_handle &rhs)
        {
            if (this != &rhs)
            {
                spTask_ = rhs.spTask_;
            }
            return (*this);
        }

    public:
        //------------------------------------------------------------------------------------------
        bool isValid() const
        {
            return spTask_ != nullptr;
        }

        //------------------------------------------------------------------------------------------
        void reset()
        {
            spTask_.reset();
        }

        //------------------------------------------------------------------------------------------
        void execute()
        {
            validate();
            oqpi_checkf(spTask_->isGrabbed(), "Trying to execute an ungrabbed task: %s", getName().c_str());
            spTask_->execute();
        }

        //------------------------------------------------------------------------------------------
        void executeSingleThreaded()
        {
            validate();
            spTask_->executeSingleThreaded();
        }

        //------------------------------------------------------------------------------------------
        void wait() const
        {
            validate();
            spTask_->wait();
        }

        //------------------------------------------------------------------------------------------
        void activeWait()
        {
            validate();
            spTask_->activeWait();
        }

        //------------------------------------------------------------------------------------------
        bool isDone() const
        {
            validate();
            return spTask_->isDone();
        }

        //------------------------------------------------------------------------------------------
        bool tryGrab()
        {
            validate();
            return spTask_->tryGrab();
        }

        //------------------------------------------------------------------------------------------
        bool isGrabbed() const
        {
            validate();
            return spTask_->isGrabbed();
        }

        //------------------------------------------------------------------------------------------
        task_priority getPriority() const
        {
            validate();
            return spTask_->getPriority();
        }

        //------------------------------------------------------------------------------------------
        void setParentGroup(const task_group_sptr &spParentGroup)
        {
            validate();
            spTask_->setParentGroup(spParentGroup); 
        }

        //------------------------------------------------------------------------------------------
        const task_group_sptr& getParentGroup() const
        {
            validate();
            return spTask_->getParentGroup();
        }

        //------------------------------------------------------------------------------------------
        const std::string& getName() const
        {
            validate();
            return spTask_->getName();
        }

    private:
        //------------------------------------------------------------------------------------------
        void validate() const
        {
            oqpi_checkf(isValid(), "Invalid task handle.");
        }

    private:
        task_sptr spTask_;
    };
    //----------------------------------------------------------------------------------------------

} /*oqpi*/
