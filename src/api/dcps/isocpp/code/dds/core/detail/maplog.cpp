/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to 2012 PrismTech
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $OSPL_HOME/LICENSE
 *
 *   for full copyright notice and license terms.
 *
 */

/**
 * @file
 */

#include <dds/core/detail/maplog.hpp>

namespace dds
{
namespace core
{
namespace detail
{

os_reportType maplog(std::string kind)
{
    if(kind == "WARNING")
    {
        return OS_WARNING;
    }
    else
    {
        return OS_DEBUG;
    }
}

}
}
}
