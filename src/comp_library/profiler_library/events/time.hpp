// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Peter Staar (taa@zurich.ibm.com)
//
// Description

#ifndef COMP_LIBRARY_PROFILER_LIBRARY_EVENTS_TIME_H
#define COMP_LIBRARY_PROFILER_LIBRARY_EVENTS_TIME_H

#include <sys/resource.h>
#include <sys/time.h>

#include <stdexcept>
#include <sstream>

namespace PROFILER {

class TimeBase {
public:
  long sec;
  long usec;
  TimeBase(long s, long u) : sec(s), usec(u) {}

  static long oneMillion() {
    static long value(1000000);
    return value;
  }
};

class Time : public TimeBase {
public:
  typedef struct timeval TimeValueType;

  Time(long s, long u) : TimeBase(s, u) {}
  Time(TimeValueType& tim) : TimeBase(tim.tv_sec, tim.tv_usec) {}

  Time& operator=(const TimeValueType& tim) {
    sec = tim.tv_sec;
    usec = tim.tv_usec;
    return *this;
  }
};

class WallTime : public Time {
public:
  typedef struct timeval TimeValueType;

  WallTime() : Time(0, 0) {
    TimeValueType tim;
    gettimeofday(&tim, NULL);
    this->sec = tim.tv_sec;
    this->usec = tim.tv_usec;
  }
};

class ResourceTime : public Time {
public:
  typedef struct rusage UsageType;

  UsageType usage;
  int status;

  int getResourceUsage() {
    return getrusage(RUSAGE_SELF, &usage);
  }

  ResourceTime(long s, long u) : Time(s, u) {}

  ResourceTime() : Time(0, 0), status(this->getResourceUsage()) {
    if (status != 0) {
      std::ostringstream msg;
      msg << "ResourceTime could not retreive the time status = " << status << "\n";
      throw std::logic_error(msg.str());
    }
  }

  ResourceTime(const UsageType& usg) : Time(0, 0), usage(usg), status(1) {}
};

class UserTime : public ResourceTime {
public:
  typedef struct rusage UsageType;
  UserTime(long s, long u) : ResourceTime(s, u) {}
  UserTime() : ResourceTime() {
    Time& tim(*this);
    tim = this->usage.ru_utime;
  }
  UserTime(const UsageType& usge) : ResourceTime(usge) {
    Time& tim(*this);
    tim = this->usage.ru_utime;
  }
};

class SystemTime : public ResourceTime {
public:
  typedef struct rusage UsageType;
  SystemTime(long s, long u) : ResourceTime(s, u) {}
  SystemTime() : ResourceTime() {
    Time& tim(*this);
    tim = this->usage.ru_stime;
  }
  SystemTime(const UsageType& usge) : ResourceTime(usge) {
    Time& tim(*this);
    tim = this->usage.ru_stime;
  }
};

class Duration : public TimeBase {
public:
  Duration(long s, long u) : TimeBase(s, u) {
    normalize();
  }

  Duration(const Time& time1, const Time& time2) : TimeBase(0, 0) {
    this->setFrom(time1, time2);
  }

  Duration(const Duration& duration1, const Duration& duration2) : TimeBase(0, 0) {
    this->setFrom(duration1, duration2);
  }

  void setFrom(const Time& time, const Time& earlierTime) {
    this->sec = time.sec - earlierTime.sec;
    this->usec = time.usec - earlierTime.usec;
    if (this->usec < 0) {
      this->sec = this->sec - 1;
      this->usec = oneMillion() + this->usec;
    }
  }

  void normalize() {
    this->sec += this->usec / oneMillion();
    this->usec = this->usec % oneMillion();
  }

  void setFrom(const Duration& duration1, const Duration& duration2) {
    this->sec = duration1.sec + duration2.sec;
    this->usec = duration1.usec + duration2.usec;
    normalize();
  }

  Duration operator+(const Duration& other) {
    return Duration(*this, other);
  }
};

Duration operator-(const Time& time, const Time& earlierTime) {
  return Duration(time, earlierTime);
}

}  // PROFILER

#endif  // COMP_LIBRARY_PROFILER_LIBRARY_EVENTS_TIME_H
