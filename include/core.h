#ifndef RUTHEN_CORE_H
#define RUTHEN_CORE_H

namespace ruthen
{

void InitializeAPIs();
void TerminateAPIs();

}

#ifdef RUTHEN_NOTHROW
#define THROW(exception)
#else
#define THROW(exception) throw exception
#endif

#endif