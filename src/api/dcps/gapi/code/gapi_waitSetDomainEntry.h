#ifndef GAPI_WAITSETDOMAINENTRY_H
#define GAPI_WAITSETDOMAINENTRY_H

#include "gapi_common.h"
#include "gapi_condition.h"

#define _WaitSetDomainEntry(o) ((_WaitSetDomainEntry)(o))

C_STRUCT(_WaitSetDomainEntry) {
    gapi_boolean  busy;
    gapi_boolean  running;
    gapi_boolean  multimode;
    gapi_long     condition_count;
    _WaitSet      waitset;
    u_waitset     uWaitset;
    u_participant uParticipant;
    os_threadId   thread;
};

_WaitSetDomainEntry
_WaitSetDomainEntryNew(
    _WaitSet waitset,
    gapi_domainId_t domain_id);

void
_WaitSetDomainEntryDelete(
    _WaitSetDomainEntry _this);

gapi_returnCode_t
_WaitSetDomainEntryMultiMode(
    _WaitSetDomainEntry _this,
    c_bool multimode);

gapi_returnCode_t
_WaitSetDomainEntryAttachCondition(
    _WaitSetDomainEntry _this,
    _Condition condition);

gapi_returnCode_t
_WaitSetDomainEntryDetachCondition(
    _WaitSetDomainEntry _this,
    _Condition condition);


os_result
_WaitSetDomainEntryWait(
    _WaitSetDomainEntry _this);

os_result
_WaitSetDomainEntryTimedWait(
    _WaitSetDomainEntry _this,
    const os_time t);

gapi_long
_WaitSetDomainEntryConditionCount(
    _WaitSetDomainEntry _this);

#endif