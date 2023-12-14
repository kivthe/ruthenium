#ifndef RUTHEN_RUTHENIUM_H
#define RUTHEN_RUTHENIUM_H

#include <memory>

#include "subsys/subsystems.h"

namespace ruthen
{
class RutheniumEngine
{
public:
    RutheniumEngine();
    ~RutheniumEngine();

private:
    std::unique_ptr<subsys::LogManager> log_manager_;
};

} 

#endif