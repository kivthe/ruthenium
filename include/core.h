#ifndef RUTHEN_CORE_H
#define RUTHEN_CORE_H

#define RUTHEN_VERSION_MAJOR 1
#define RUTHEN_VERSION_MINOR 0
#define RUTHEN_VERSION_PATCH 0
#define RUTHEN_VERSION_TWEAK 0

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