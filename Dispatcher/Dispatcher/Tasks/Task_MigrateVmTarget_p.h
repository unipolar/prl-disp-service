///////////////////////////////////////////////////////////////////////////////
///
/// @file Task_MigrateVmTarget_p.h
//
/// Private part of a target side of Vm migration.
///
/// @author nshirokovskiy@
///
/// Copyright (c) 2010-2015 Parallels IP Holdings GmbH
///
/// This file is part of Virtuozzo Core. Virtuozzo Core is free
/// software; you can redistribute it and/or modify it under the terms
/// of the GNU General Public License as published by the Free Software
/// Foundation; either version 2 of the License, or (at your option) any
/// later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
/// 02110-1301, USA.
///
/// Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
/// Schaffhausen, Switzerland.
///
/////////////////////////////////////////////////////////////////////////////////

#ifndef __TASK_MIGRATEVMTARGET_P_H__
#define __TASK_MIGRATEVMTARGET_P_H__

#include <QLocalSocket>
#include "Task_MigrateVm_p.h"
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <Libraries/VmFileList/CVmFileListCopy.h>

class Task_MigrateVmTarget;

namespace Migrate
{
namespace Vm
{
namespace Target
{
struct Frontend;
typedef boost::msm::back::state_machine<Frontend> Machine_type;
typedef Pump::Event<Parallels::VmMigrateStartCmd> StartCommand_type;
typedef Pump::Event<Parallels::FileCopyRangeStart> CopyCommand_type;

namespace Content
{
///////////////////////////////////////////////////////////////////////////////
// struct Frontend

struct Frontend: Vm::Frontend<Frontend>
{
	typedef Copying initial_state;

	struct Good
	{
	};

	explicit Frontend(Task_MigrateVmTarget& task_): m_task(&task_)
	{
	}
	Frontend(): m_task()
	{
	}

	template <typename Event, typename FSM>
	void on_entry(const Event&, FSM&);

	template <typename Event, typename FSM>
	void on_exit(const Event&, FSM&);

	void copy(const CopyCommand_type& event_);

	// there in no use in copier cancel operation, it would not anyway
	// break blocking file io thus we have no action on cancel
	struct transition_table : boost::mpl::vector<
		a_row<Copying,CopyCommand_type,	Copying,&Frontend::copy>,
		_row<Copying,	Flop::Event,	Flop::State>,
		_row<Copying,	Good,		Success>
	>
	{
	};

private:
	Task_MigrateVmTarget *m_task;
	SmartPtr<CVmFileListCopySender> m_sender;
	SmartPtr<CVmFileListCopyTarget> m_copier;
};

} // namespace Content

namespace Tunnel
{
///////////////////////////////////////////////////////////////////////////////
// struct Connected

struct Connected
{
};

///////////////////////////////////////////////////////////////////////////////
// struct Disconnected

struct Disconnected
{
};

///////////////////////////////////////////////////////////////////////////////
// struct Disconnecting

struct Disconnecting: Trace<Disconnecting>
{
};

///////////////////////////////////////////////////////////////////////////////
// struct Connector

struct Connector: QObject, Vm::Connector::Base<Machine_type>
{
public slots:
	void reactConnected();

	void reactDisconnected();

private:
	Q_OBJECT
};

///////////////////////////////////////////////////////////////////////////////
// struct IO

struct IO: Pump::IO
{
	explicit IO(CDspDispConnection& io_);
	~IO();

	IOSendJob::Handle sendPackage(const SmartPtr<IOPackage>& package_);

signals:
	void disconnected();

private slots:
	void reactReceived(IOSender::Handle handle_, const SmartPtr<IOPackage>& package_);

	void reactDisconnected(IOSender::Handle handle);

	void reactSend(IOServerInterface*, IOSender::Handle handle_,
		    IOSendJob::Result, const SmartPtr<IOPackage>);

private:
	Q_OBJECT

	CDspDispConnection *m_io;
};

///////////////////////////////////////////////////////////////////////////////
// struct Frontend

struct Frontend: Vm::Frontend<Frontend>, Vm::Connector::Mixin<Connector>
{
	typedef Pump::Frontend<Machine_type, boost::mpl::false_> pump_type;
	typedef boost::msm::back::state_machine<pump_type> Pumping;
	typedef Vm::Tunnel::Prime initial_state;

	explicit Frontend(IO& service_) : m_service(&service_)
	{
	}
	Frontend(): m_service()
	{
	}

	template <typename Event, typename FSM>
	void on_entry(const Event&, FSM&);

	template <typename Event, typename FSM>
	void on_exit(const Event&, FSM&);

	void connect(const Connected&);

	void disconnect(const msmf::none&);

	struct transition_table : boost::mpl::vector<
		a_row<initial_state,		Connected,	Vm::Tunnel::Ready,&Frontend::connect>,
		msmf::Row<Vm::Tunnel::Ready,	Pump::Launch_type,Pumping>,
		a_row<Pumping::exit_pt<Success>,msmf::none,	Disconnecting,	&Frontend::disconnect>,
		msmf::Row<Pumping,		Disconnected,	msmf::none,	Flop::Action>,
		msmf::Row<Disconnecting,	Disconnected,	Success>,
		msmf::Row<Pumping::exit_pt<Flop::State>,Flop::Event,Flop::State>
	>
	{
	};

private:
	IO* m_service;
	QSharedPointer<QLocalSocket> m_socket;
};

} // namespace Tunnel

///////////////////////////////////////////////////////////////////////////////
// struct Connector

struct Connector: QObject, Vm::Connector::Base<Machine_type>
{
public slots:
	void cancel();

	void disconnected();

	void react(const SmartPtr<IOPackage>& package_);

private:
	Q_OBJECT
};

///////////////////////////////////////////////////////////////////////////////
// struct Frontend

struct Frontend: Vm::Frontend<Frontend>, Vm::Connector::Mixin<Connector>
{
	typedef Pipeline::Frontend<Machine_type, StartCommand_type> starting_type;
	typedef boost::msm::back::state_machine<starting_type> Starting;
	typedef boost::msm::back::state_machine<Content::Frontend> Copying;
	typedef boost::msm::back::state_machine<Tunnel::Frontend> Tunneling;
	typedef Starting initial_state;

	Frontend(Task_MigrateVmTarget& task_, Tunnel::IO& io_):
		m_io(&io_), m_task(&task_)
	{
	}
	Frontend(): m_io(), m_task()
	{
	}

	template <typename Event, typename FSM>
	void on_entry(const Event&, FSM&);

	template <typename Event, typename FSM>
	void on_exit(const Event&, FSM&);

	void setResult(const Flop::Event& value_);

	struct transition_table : boost::mpl::vector<
		// wire error exits to FINISHED immediately
		a_row<Starting::exit_pt<Flop::State>,	Flop::Event,Finished, &Frontend::setResult>,
		a_row<Copying::exit_pt<Flop::State>,	Flop::Event,Finished, &Frontend::setResult>,
		a_row<Tunneling::exit_pt<Flop::State>,	Flop::Event,Finished, &Frontend::setResult>,
		// wire success exits sequentially up to FINISHED
		msmf::Row<Starting::exit_pt<Success>,	msmf::none,Copying>,
		msmf::Row<Copying::exit_pt<Success>,	msmf::none,Tunneling>,
		msmf::Row<Tunneling::exit_pt<Success>,	msmf::none,Finished>,
		// handle asyncronous termination
		a_row<Starting,	Flop::Event, Finished,	&Frontend::setResult>,
		a_row<Copying,	Flop::Event, Finished,	&Frontend::setResult>,
		a_row<Tunneling,Flop::Event, Finished,	&Frontend::setResult>
	>
	{
	};

private:
	Tunnel::IO* m_io;
	Task_MigrateVmTarget *m_task;
};

} // namespace Target
} // namespace Vm
} // namespace Migrate

#endif // __TASK_MIGRATEVMTARGET_P_H__