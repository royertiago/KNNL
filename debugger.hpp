/*
 * Copyright (c) 2005, Seweryn Habdank-Wojewodzki
 *
 * All rights reserved.
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 * This software has generative abilities, but does not claim any rights
 * nor guarantees with regard to the generated code.
 */

/*
 * e-mail: habdank{AT}megapolis{dot}pl
 *
 * File created: Sat 05 Nov 2005 14:20:28 CET
 * Last modified: Tue 23 May 2006 23:49:49 CEST
 */

/**
 * \defgroup trivial_debugger Trivial Debugger
 */

/**
 * \file debugger.hpp
 * \ingroup trivial_debugger
 * \brief Defines macro for Trivial Debugger.
 * When compiler send to program one of flags: TDEBUG, ETDEBUG or FTDEBUG debugger functionality is set.
 */

/**
 * \def D
 * \brief Macro D will send to proper stream value of the variable.
 * Stream depends on set flag. Information which is send looks like:
 *
 * __FILE__ [__LINE__] : variable name = variable value
 *
 * \param name is name of debugged variable.
 * \par Usage:
 *
 * D ( my_variable_name );
 *
 * Remember to put semicolon at the end.
 *
 * \par Flags:
 *
 * - TDEBUG - Trivial Debugger sends debugging information to the stdout.
 *
 * - ETDEBUG - Trivial Debugger sends debugging information to the stderr.
 *
 * - FTDEBUG - Trivial Debugger sends debugging information to the file
 * (generally declared and defined in main() function of the program).
 */

#ifndef DEBUGGER_HPP_INCLUDED
#define DEBUGGER_HPP_INCLUDED

#ifdef FTDEBUG

	#include <memory>
	#include <fstream>

	#define D(name) *DEBUGGER_STREAM << __FILE__ << " [" << __LINE__ << "] : " << #name << " = " << name << std::endl
	extern std::auto_ptr < std::ofstream > DEBUGGER_STREAM;

#elif defined(TDEBUG)

	#include <iostream>
	#define D(name) std::cout << __FILE__ << " [" << __LINE__ << "] : " << #name << " = " << name << std::endl

#elif defined(ETDEBUG)

	#include <iostream>
	#define D(name) std::cerr << __FILE__ << " [" << __LINE__ << "] : " << #name << " = " << name << std::endl

#else

	#define D(name) {}

#endif // ..TDEBUG

#endif // DEBUGGER_HPP_INCLUDED

