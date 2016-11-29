#pragma once

/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "threads/CriticalSection.h"

#include <memory>
#include <stdint.h>

#define DVD_TIME_BASE 1000000
#define DVD_NOPTS_VALUE 0xFFF0000000000000

#define DVD_TIME_TO_MSEC(x) ((int)((double)(x) * 1000 / DVD_TIME_BASE))
#define DVD_SEC_TO_TIME(x)  ((double)(x) * DVD_TIME_BASE)
#define DVD_MSEC_TO_TIME(x) ((double)(x) * DVD_TIME_BASE / 1000)

#define DVD_PLAYSPEED_PAUSE       0       // frame stepping
#define DVD_PLAYSPEED_NORMAL      1000

class CVideoReferenceClock;

#ifdef HAS_DS_PLAYER
//Time base from directshow is a 100 nanosec unit
#define DS_TIME_BASE 1E7

#define DS_TIME_TO_SEC(x)  ((double)(x / DS_TIME_BASE))
#define DS_TIME_TO_MSEC(x) ((double)(x * 1000 / DS_TIME_BASE))
#define SEC_TO_DS_TIME(x)  ((__int64)(x * DS_TIME_BASE))
#define MSEC_TO_DS_TIME(x) ((__int64)(x * DS_TIME_BASE / 1000))
#define SEC_TO_MSEC(x)     ((double)(x * 1E3))
#endif

class CDVDClock
{
public:

  CDVDClock();
  ~CDVDClock();

  double GetClock(bool interpolated = true);
  double GetClock(double& absolute, bool interpolated = true);

  double ErrorAdjust(double error, const char* log);
  void Discontinuity(double clock, double absolute);
  void Discontinuity(double clock = 0LL)
  {
    Discontinuity(clock, GetAbsoluteClock());
  }

  void Reset() { m_bReset = true; }
  void SetSpeed(int iSpeed);
  void SetSpeedAdjust(double adjust);
  double GetSpeedAdjust();

  double GetClockSpeed(); /**< get the current speed of the clock relative normal system time */

  /* tells clock at what framerate video is, to  *
   * allow it to adjust speed for a better match */
  int UpdateFramerate(double fps, double* interval = NULL);

  void SetMaxSpeedAdjust(double speed);

  double GetAbsoluteClock(bool interpolated = true);
  double GetFrequency() { return (double)m_systemFrequency ; }

  bool GetClockInfo(int& MissedVblanks, double& ClockSpeed, double& RefreshRate) const;
  void SetVsyncAdjust(double adjustment);
  double GetVsyncAdjust();

#ifdef HAS_DS_PLAYER
  // Allow a different time base (DirectShow for example use a 100 ns time base)
  void SetTimeBase(int64_t timeBase) { m_timeBase = timeBase; }
  int64_t GetTimeBase() { return m_timeBase; }
#endif

  void Pause(bool pause);

protected:
  double SystemToAbsolute(int64_t system);
  int64_t AbsoluteToSystem(double absolute);
  double SystemToPlaying(int64_t system);

  CCriticalSection m_critSection;
  int64_t m_systemUsed;
  int64_t m_startClock;
  int64_t m_pauseClock;
  double m_iDisc;
  bool m_bReset;
  bool m_paused;
  int m_speedAfterPause;
  std::unique_ptr<CVideoReferenceClock> m_videoRefClock;

  int64_t m_systemFrequency;
  int64_t m_systemOffset;
  CCriticalSection m_systemsection;
#ifdef HAS_DS_PLAYER
  int64_t m_timeBase;
#endif

  int64_t m_systemAdjust;
  int64_t m_lastSystemTime;
  double m_speedAdjust;
  double m_vSyncAdjust;
  double m_frameTime;

  double m_maxspeedadjust;
  CCriticalSection m_speedsection;
};
