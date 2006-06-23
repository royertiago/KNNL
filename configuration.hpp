/*
 * Copyright (c) 2006, Seweryn Habdank-Wojewodzki
 * Copyright (c) 2006, Janusz Rybarski
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *
 * Redistributions of source code must retain the above
 * copyright notice, this list of conditions and the
 * following disclaimer.
 *
 * Redistributions in binary form must reproduce the
 * above copyright notice, this list of conditions
 * and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
 * AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * e-mail: habdank AT megapolis DOT pl
 * e-mail: janusz.rybarski AT ae DOT krakow DOT pl
 *
 * File created: Sun 29 Jan 2006 11:35:05 CET
 * Last modified: Mon 19 Jun 2006 14:49:50 CEST
 */

#ifndef CONFIGURATION_HPP_INCLUDED
#define CONFIGURATION_HPP_INCLUDED

#include <iostream>
#include <boost/program_options.hpp>

/**
 * \defgroup config Configuration
 */

/**
 * \file configuration.hpp
 * \ingroup config
 * \brief File contains decrarations of the ::config:: namespace.
 */

/**
 * \namespace config
 * \ingroup config
 * \brief Classes and functions related to the construction of the program configuration.
 */

/** \addtogroup config */
/*\@{*/
namespace config
{
	/**
	 * Base for calculation major number as unsigned int.
	 */
	const unsigned int BASE_MAJOR = 16777216;

	/**
	 * Base for calculation minor number as unsigned int.
	 */
	const unsigned int BASE_MINOR = 16384;

	/**
	 * Width of screen for displaying options.
	 */
	const unsigned int screen_width = 80;
		
	/** Exit status. */
	enum
	{
		/// Run the calculations
		PROCEED = -1,
		/// Exit with succes after displaying e.g. help
		EXIT_SUCCES = 0,
		/// something went bad
		EXIT_FAILLURE = 1
	};
	
	/**
	 * \struct Version_struct
	 * \brief Composition of the numbers describing version number of the software e.g. 1.2.3.
	 */
	struct Version_struct
	{
		unsigned int major;		/**< Number on the first position in the string */
		unsigned int minor;		/**< Number on the second position in the string */
		unsigned int release;	/**< Number on the third position in the string */
		std::string quantifier;	/**< Rest of version number string */
	};

	/**
	 * \class Configuration_container
	 * \brief Contains options dedicated for the program
	 */
	class Configuration_container
	{
	public:
		/**
		 * Verbosity level.
		 */
		int verbosity;

		/**
		 * Input file name.
		 */
		std::string input_file_name;

		/**
		 * Number of rows in kohonen network.
		 */
		size_t no_rows;

		/**
		 * Number of columns in kohonen network.
		 */
		size_t no_columns;

		/**
		 * Number of epochs of the training algorithm.
		 * In this demo program the <b>one</b> epoch is
		 * <b>one</b> pass through the all training data.
		 */
		unsigned long no_epochs;

	};
	
	/**
	 * \class Basic_configuration
	 * \brief Contains all fields and methods for creating configuration template.
	 */
	template < class Internal_configuration_type >
	class Basic_configuration
	{
	public:

		/**
		 * \typedef Config_class
		 * \brief definition that helps to return configuration class as a type.
		 */
		typedef Internal_configuration_type Config_class;

		/**
		 * Set program name.
		 * \param program_name_ containg string e.g. "Pattern Finder".
		 */
		void set_program_name ( std::string program_name_ ) throw()
		{
			program_name = program_name_;
		}
		
		/**
		 * Set version number.
		 * \param version_struct_ contains description of the version.
		 * program_version is concatenated and
		 * version_number is calculated.
		 */
		void set_version_number ( Version_struct & version_struct_ ) throw()
		{
			version_struct = version_struct_;

			{
				// prepare string
				std::stringstream sstr_;
				sstr_ << version_struct.major
					<< "." << version_struct.minor
					<< "." << version_struct.release
					<< version_struct.quantifier;
				
				version_string = sstr_.str();
			}

			// calculate version number
			version_number = calculate_version_number ( version_struct );

			return;
		}
		
		/**
		 * Set version number.
		 * \param version_string_ contains string e.g. "1.2.3".
		 * version_struct and version_number are calculated.
		 */
		void set_version_number ( std::string version_string_ ) throw()
		{
			version_string = version_string_;

			std::string v_s = version_string_;

			// prepare string to parse it in the string stream
			std::transform
			(
				v_s.begin(), v_s.end(),
				v_s.begin(),
				dot_eraser
			);

			// initialize stringstream
			std::stringstream s_v_s ( v_s );
			// and table for temporal data
			int tmp_tab[3];

			// "parse" stringstream
			for ( int i = 0; i < 3; ++i )
			{
				tmp_tab[i] = 0;
				s_v_s >> tmp_tab[i];
			}

			// put proper values
			version_struct.major = tmp_tab[0];
			version_struct.minor = tmp_tab[1];
			version_struct.release = tmp_tab[2];

			version_struct.quantifier = s_v_s.str();

			// calculate version number
			version_number = calculate_version_number ( version_struct );
			
			return;
		}

		/**
		 * Get program name.
		 * \return std::string.
		 */
		inline std::string get_program_name() throw()
		{
			return program_name;
		}

		/**
		 * Get version number as a struct.
		 * \return Version_struct with version.
		 */
		inline Version_struct get_version_struct() throw()
		{
			return version_struct;
		}

		/**
		 * Get version number as a string.
		 * \return std::string e.g. 0.1.2.
		 */
		inline std::string get_version_string() throw()
		{
			return version_string;
		}

		/**
		 * Get version number.
		 * \return unsigned int it is needed by the boost::serialisation package.
		 */
		inline unsigned int get_version_number() throw()
		{
			return version_number;
		}

		/**
		 * Class that contains all important configuration parameters.
		 */
		Internal_configuration_type parameters;
		
	private:
		/** Program name. */
		std::string program_name;

		/** Version number. */
		Version_struct version_struct;

		/** Version number. */
		std::string version_string;

		/** Version number. */
		unsigned int version_number;

		/**
		 * Calculate version number from data in Version_struct.
		 * \param version_struct_ contains version information.
		 * \return unsigned int value of the version.
		 */
		inline unsigned int calculate_version_number ( Version_struct & version_struct_ ) throw()
		{
			// calculate version as a unsigned int number
			return version_struct.major * BASE_MAJOR
				+ version_struct.minor * BASE_MINOR
				+ version_struct.release;
		}

		/**
		 * Eliminate dots '.' from string.
		 * \param ch character for transformation.
		 * \return char transformed character.
		 */
		static char dot_eraser ( char ch ) throw()
		{
			return ( ch == '.' ? ' ' : ch );
		}
	};
	
	/**
	 * \typedef Configuration
	 * \brief Configuration class.
	 */
	typedef Basic_configuration < Configuration_container > Configuration;
	
	/**
	 * \typedef Program_options
	 * \brief Set configuration as Singleton in the program structure. */
	typedef Configuration Program_options;

	/**
	 * Create all options.
	 *
	 * \param argc has standard meanning.
	 * \param argv has standard meanning.
	 * \param conf_ is a configuration object.
	 * \return exit status.
	 */
	int do_program_options ( int argc, char * argv[], Configuration & conf_ ) throw();

} // namespace config
/*\@}*/

#endif // CONFIGURATION_HPP_INCLUDED

