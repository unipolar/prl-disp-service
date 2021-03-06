///////////////////////////////////////////////////////////////////////////////
///
/// @file CDspSystemEventsMonitor.h
///
/// system events monitor thread implementation
///
/// @author sergeyt
///
/// Copyright (c) 2005-2015 Parallels IP Holdings GmbH
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
///////////////////////////////////////////////////////////////////////////////

#ifndef H_CDspSystemEventsMonitor_H
#define H_CDspSystemEventsMonitor_H

#include <QThread>
#include <QMutex>

/**
 * Thread that react on system events
 */
class CDspSystemEventsMonitor : public QThread
{
	Q_OBJECT

public:
	CDspSystemEventsMonitor();

	void startMonitor();
	/** Thread finalization method*/
	void stopMonitor();

protected:
	/** Create appropriate events handler object */
	QObject * createSystemEventsHandler();

	/** Overridden method of thread working body */
	void run();
};


/**
 * Dispatcher system events handler
 */
class CDspSystemEventsHandler : public QObject
{
	Q_OBJECT
public slots:
	void onHostSleep();
	void onHostWakeup();

private:

#ifdef _WIN_
	void onWinHostSleep();
	void onWinHostWakeup();
#endif
};

#endif //H_CDspSystemEventsMonitor_H
