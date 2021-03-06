#pragma once

#include "oqpi/scheduling/task_handle.hpp"
#include "oqpi/scheduling/task_notifier.hpp"
#include "oqpi/scheduling/task_group_base.hpp"


namespace oqpi {

    //----------------------------------------------------------------------------------------------
    template<typename _Scheduler, task_type _TaskType, typename _GroupContext>
    class task_group
        : public task_group_base
        , public notifier<_TaskType>
        , public _GroupContext
    {
        //------------------------------------------------------------------------------------------
        using self_type     = task_group<_Scheduler, _TaskType, _GroupContext>;
        using notifier_type = notifier<_TaskType>;

    public:
        //------------------------------------------------------------------------------------------
        task_group(_Scheduler &sc, const std::string &name, task_priority priority)
            : task_group_base(priority)
            , notifier_type(task_base::getUID())
            , _GroupContext(this, name)
            , scheduler_(sc)
        {}

        //------------------------------------------------------------------------------------------
        virtual ~task_group()
        {}

    public:
        //------------------------------------------------------------------------------------------
        // task_group_base implemented interface
        virtual void addTask(task_handle hTask) override final
        {
            if(hTask.isValid())
            {
                if (oqpi_ensuref(hTask.getParentGroup() == nullptr,
                    "This task (%d) is already bound to a group: %d", hTask.getUID(), hTask.getParentGroup()->getUID()))
                {
                    hTask.setParentGroup(shared_from_this());
                    addTaskImpl(hTask);
                    _GroupContext::group_onTaskAdded(hTask);
                }
            }
        }

        //------------------------------------------------------------------------------------------
        virtual void execute() override final
        {
            _GroupContext::group_onPreExecute();
            executeImpl();
        }

        //------------------------------------------------------------------------------------------
        virtual void executeSingleThreaded() override final
        {
            _GroupContext::group_onPreExecute();
            executeSingleThreadedImpl();
            _GroupContext::group_onPostExecute();

            task_base::setDone();
            notifier_type::notify();
        }

        //------------------------------------------------------------------------------------------
        virtual void wait() const override final
        {
            notifier_type::wait();
        }

        //------------------------------------------------------------------------------------------
        virtual void activeWait() override
        {
            oqpi_checkf(false, "Not supported, fall back to wait");
            wait();
        }

    protected:
        //------------------------------------------------------------------------------------------
        // Implementation details interface
        virtual void addTaskImpl(const task_handle &hTask)  = 0;
        virtual void executeImpl()                          = 0;
        virtual void executeSingleThreadedImpl()            = 0;

    protected:
        //------------------------------------------------------------------------------------------
        // Called once all tasks of a group are done
        void notifyGroupDone()
        {
            task_base::setDone();
            _GroupContext::group_onPostExecute();
            notifier<_TaskType>::notify();
            task_base::notifyParent();
        }

    protected:
        // We need a reference to the scheduler so that the groups can add their tasks
        _Scheduler &scheduler_;
    };
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    template<template<typename, task_type, typename> class _TaskGroupType, task_type _TaskType, typename _GroupContext, typename _Scheduler, typename... _Args>
    inline auto make_task_group(_Scheduler &sc, const std::string &name, _Args &&...args)
    {
        return std::make_shared<_TaskGroupType<_Scheduler, _TaskType, _GroupContext>>(sc, name, std::forward<_Args>(args)...);
    }
    //----------------------------------------------------------------------------------------------

} /*oqpi*/
