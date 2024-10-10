/*!
 * \file platform.hpp
 *
 * \copyright © 2024 s3mat3 
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Platform detecter
 *
 * \author s3mat3
 */

#pragma once

#ifndef SML_PLATFORM_Hpp
# define  SML_PLATFORM_Hpp

# ifdef _WIN32
#  define SML_WINDOWS
# elif __posix
#  define SML_POSIX
# elif __linux
#  define SML_POSIX
# elif __unix
#  define SML_POSIX
# else
#  define SML_NOT_SUPORTED
# endif


#endif //<-- macro  SML_PLATFORM_Hpp ends here.
