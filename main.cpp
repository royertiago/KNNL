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
 * File created: Thu 10 Apr 2006 11:05:06 CEST
 * Last modified: Thu 08 Jun 2006 00:10:22 CEST
 */

#include "debugger.hpp"

#include "std_headers.hpp"
#include "neural_net_headers.hpp"
#include "configuration.hpp"

/**
 * \mainpage Kohonen Neural Network Library Demo
 *
 * \par Program is designed for making pattern recognition using Kohonen Self-oragnizing Map.
 *
 * \section intro_sec Introduction
 *
 * \section options Options
 *
 * \par Program usage:
 *
 * knnl_demo [OPTIONS] [FILE]
 *
 * If FILE is not given then standard input (stdin) is taken as input stream.
 *
 * \par Generic options:
 *
 * - -h help message
 *
 * - -V version string
 *
 * - -r number of rows in neural matrix
 *
 * - -c number of columns in neural martix
 *
 * \par Hidden options:
 *
 * - -i [ --input-file ] arg input file name
 *
 * \author Seweryn Habdank-Wojewodzki
 * \author Janusz Rybarski
 * \version 0.1.0
 * \date 2006.06.03
 */

/**
 * \file main.cpp
 * \brief Main file of the program contains int main ( int argc, char * argv[] ) function :-)
 */

/**
 * \var DEBUGGER_STREAM
 * \brief Set dynamical pointer to the stream.
 * \code
 * DEBUGGER_STREAM =
 *	std::auto_ptr < std::ofstream > ( new std::ofstream ( "_debugger.out" ) );
 * \endcode
 */
#ifdef FTDEBUG
std::auto_ptr < std::ofstream > DEBUGGER_STREAM;
#endif //FTDEGUG

/**
 * \defgroup data_storages Data storages
 */

/**
 * Configuration.
 * \ingroup data_storages
 */
config::Configuration CONF;

/**
 * Main function of the program.
 */
int main ( int argc, char * argv[] )
{
#ifdef FTDEBUG
	DEBUGGER_STREAM =
		std::auto_ptr < std::ofstream >
		( new std::ofstream ( "_debugger.out" ) );
#endif //FTDEBUG

	// set program name and version number
	CONF.set_program_name ( "Kohonen Network Library Demo" );
	CONF.set_version_number ( "0.0.1" );

	int return_value = 1;

	// flag used when compilation is prepared for the tests using pmap linux command
#ifdef PMAP
	char key;
	std::cout << "Press any key.";
	::getc (key);
#endif //PMAP

	try
	{
		// try to read command line
		return_value = config::do_program_options ( argc, argv, CONF );

		// if PROCEED was returned then follow the white rabbit
		if ( return_value != config::PROCEED )
		{
			return return_value;
		}

		// set NULL to the istream pointer
		std::istream * file_ptr = static_cast < std::istream * > ( 0 );

		// if input_file_name is empty set standard input as input for the program
		// this is important when program have to work as filter for the stream console
		if ( CONF.parameters.input_file_name == "" )
		{
			file_ptr = new std::istream ( std::cin.rdbuf() ) ;
			// std::cout << "No file name specified." << std::endl;
		}
		else
		{
			// set real file as input stream
			file_ptr = new std::ifstream ( CONF.parameters.input_file_name.c_str() );
			// std::cout << CONF.parameters.input_file_name << std::endl;
			if ( !file_ptr )
			{
				std::cout << "Error in reading file" << std::endl;
				std::exit ( EXIT_FAILURE );
			}
		}

		// read file for very big files if could cause problems,
		// but for files smaller than available memory this is faster solution
		std::stringstream tmp_stream;
		tmp_stream << file_ptr->rdbuf();
		delete file_ptr;

		// start random generator
		std::srand ( time ( NULL ) );

		typedef double data_type;

		// it could cause problems
		try
		{
			// set number of rows for network
			int R = CONF.parameters.no_rows;
			// set number of columns for network
			int C = CONF.parameters.no_columns;

			// type of the single data vector
			// for example if we have pairs of the data like { (1,2), (2,3), (3,4)}
			// V_d describes type of (1,2)
			typedef std::vector < data_type > V_d;
			typedef std::vector < V_d > V_v_d;

			V_v_d data;

			data_parser::Data_parser < V_v_d > data_parser;
			data_parser ( tmp_stream, data );

			// configure Cauchy hat function
			typedef neural_net::Cauchy_function < V_d::value_type, V_d::value_type, int > C_a_f;
			// Activation function for neurons
			C_a_f c_a_f ( 2.0, 2 );

			// configure Gauss hat function
			typedef neural_net::Gauss_function < V_d::value_type, V_d::value_type, int > G_a_f;
			// Activation function for neurons
			G_a_f g_a_f ( 2.0, 2 );

			// prepare euclidean distance function
			typedef distance::Euclidean_distance_function < V_d > E_d_t;
			E_d_t e_d;

			// and weighted euclidean distance function
			typedef distance::Weighted_euclidean_distance_function < V_d, V_d > We_d_t;
			V_d coefs;
			coefs.push_back ( 0.4 );	// weight for first axis
			coefs.push_back ( 1 );		// weight for second axis
			We_d_t we_d ( coefs );

			/* prepare different variants of neurons */

			/* examples how to construct single neuron */
			// here Cauchy activation function and euclidean distance is chosen
			typedef neural_net::Basic_neuron < C_a_f, E_d_t, V_d > Kohonen_neuron;
			Kohonen_neuron my_neuron ( *(data.begin()), c_a_f, e_d );

			// here Gauss activation function and weighted euclidean distance is chosen
			typedef neural_net::Basic_neuron < G_a_f, We_d_t, V_d > Kohonen_neuron_w;
			Kohonen_neuron_w my_neuron_w ( *(data.begin()), g_a_f, we_d );

			/* neural networks parametrized by different neurons */

			// Kohonen_neuron is used for network construction
			typedef neural_net::Rectangular_container < Kohonen_neuron > Kohonen_network;
			Kohonen_network kohonen_network;

			// Other Kohonen neuron (Kohonen_neuron_m) is used
			typedef neural_net::Rectangular_container < Kohonen_neuron_w > Kohonen_network_w;
			Kohonen_network_w kohonen_network_w;

			// example networks
			Kohonen_network kohonen_network_1;
			Kohonen_network kohonen_network_2;
			Kohonen_network kohonen_network_3;
			Kohonen_network kohonen_network_4;

			// prepare randomization policy
			neural_net::Internal_randomize IR;
			neural_net::External_randomize ER;

			// generate networks initialized by data
			neural_net::generate_kohonen_network
			<
				V_v_d,
				Kohonen_network,
				neural_net::Internal_randomize
			> ( R, C, c_a_f, e_d, data, kohonen_network, IR );

			neural_net::generate_kohonen_network
			<
				V_v_d,
				Kohonen_network_w,
				neural_net::External_randomize
			> ( R, C, g_a_f, we_d, data, kohonen_network_w, ER );

			// construct WTA training functional
			typedef neural_net::Wta_proportional_training_functional < V_d, double, int > Wta_train_func;

			// construct WTA training algoritm
			typedef neural_net::Wta_training_algorithm < Kohonen_network, V_d, V_v_d::iterator, Wta_train_func, int > Learning_algorithm;
			typedef neural_net::Wta_training_algorithm < Kohonen_network_w, V_d, V_v_d::iterator, Wta_train_func, int > Learning_algorithm_w;

			// define proper functionals
			// Wta_train_func wta_train_func_0 ( static_cast < double > ( R * C ) / static_cast < double > ( data.size() ), 0 );
			// Wta_train_func wta_train_func_2 ( 0.3, 1.0 / static_cast<double> (data.size()) );
			Wta_train_func wta_train_func ( 0.2 , 0 );
			Wta_train_func wta_train_func_1 ( 0.8 , 0 );

			// make copy of network
			kohonen_network_1 = kohonen_network;
			kohonen_network_2 = kohonen_network;
			kohonen_network_3 = kohonen_network;
			kohonen_network_4 = kohonen_network;

			// print weights before training process
			//std::cout << "kohonen_network" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network );
			//std::cout << std::endl;
			
			//std::cout << "kohonen_network_1" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network_1 );
			//std::cout << std::endl;

			//std::cout << "kohonen_network_m" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network_w );
			//std::cout << std::endl;

			// define training algorithm
			Learning_algorithm training_alg ( wta_train_func, 0, 1 );
			
			// set number of the the training epochs
			// one epoch it is training through all data stored in container
			const int no_epochs = 30;

			// example of using macro for debugging
			D ( no_epochs );

			// train network
			for ( int i = 0; i < no_epochs; ++i )
			{
				training_alg ( data.begin(), data.end(), &kohonen_network );
				// beter results are reached if data are sorted in each cycle
				// in target procedure this functionality could be prepared using
				// proper policy like Shuffle_policy
				// std::random_shuffle ( data.begin(), data.end() );
			}

			//std::cout << "kohonen_network" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network );
			//std::cout << std::endl;

			// define another training algorithm
			Learning_algorithm training_alg_1 ( wta_train_func_1, 0, 1 );

			// train network
			for ( int i = 0; i < no_epochs; ++i )
			{
				training_alg_1 ( data.begin(), data.end(), &kohonen_network_1 );
				// beter results are reached if data are sorted in each cycle
				// in target procedure this functionality could be prepared using
				// proper policy like Shuffle_policy
				// std::random_shuffle ( data.begin(), data.end() );
			}

			//std::cout << "kohonen_network_1" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network_1 );
			//std::cout << std::endl;

			// define another algorithm for another network
			Learning_algorithm_w training_alg_w ( wta_train_func_1, 0, 1 );

			// train network
			for ( int i = 0; i < no_epochs; ++i )
			{
				training_alg_w ( data.begin(), data.end(), &kohonen_network_w );
				// beter results are reached if data are sorted in each cycle
				// in target procedure this functionality could be prepared using
				// proper policy like Shuffle_policy
				// std::random_shuffle ( data.begin(), data.end() );
			}

			//std::cout << "kohonen_network_m" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network_w );
			//std::cout << std::endl;

			// example of using another function for printing results from network
			//for ( V_v_d::iterator pos = data.begin(); pos != data.end(); ++pos )
			//{
			//	neural_net::print_network ( std::cout, kohonen_network, *pos );
			//	std::cout << "---------------------------------------------------------------" << std::endl;
			//}

			// construct topologies for WTM training algorithm
			// city_top and max_top is not used in examples below,
			// but for tests in this file have to be done
			neural_net::City_topology < size_t > city_top();
			neural_net::Max_topology < size_t > max_top();

			// Hexagonal topology will be used in further tests
			typedef neural_net::Hexagonal_topology < size_t > Hex_top;
			Hex_top hex_top ( kohonen_network.get_no_rows() );

			// construct another functions that are need for WTM algorithm
			// functors below are used for weighting training of neurons
			// with respect to the data space
			typedef neural_net::Gauss_function < V_d::value_type, V_d::value_type, int > G_f_space;
			G_f_space g_f_space ( 100, 1 );
			typedef neural_net::Cauchy_function < V_d::value_type, V_d::value_type, int > C_f_space;
			C_f_space c_f_space ( 100, 1 );
			typedef neural_net::Constant_function < V_d::value_type, V_d::value_type > Co_f_space;
			Co_f_space co_f_space ( 1 );

			// functors below are used for weighting training of neurons
			// with respect to the neural network topology
			typedef neural_net::Gauss_function < size_t, V_d::value_type, int > G_f_net;
			G_f_net g_f_net ( 10, 1 );
			typedef neural_net::Cauchy_function < size_t, V_d::value_type, int > C_f_net;
			C_f_net c_f_net ( 10, 1 );

			// typedefs for tests
			typedef G_f_space Space_func;
			typedef G_f_net Net_func;
			typedef Hex_top Net_top;
			//typedef M_d_t Space_top;
			typedef E_d_t Space_top;

			// constructing typical training generalized weight
			typedef neural_net::Classic_training_weight
			<
				V_d,
				int,
				Net_func,
				Space_func,
				Net_top,
				Space_top,
				size_t
			> Classic_weight;

			// define if
			Classic_weight classic_weight ( g_f_net, g_f_space, hex_top, e_d );

			// construct experimental training generalized weight
			// experiment allow user to repel weights of neurons which
			// are separated in network but they have similar weights
			// this functionality is useful in the beginning of the training
			// proces to avoid situation that in different parts of neural
			// network are similar neurons - it is useless redundancy
			typedef neural_net::Experimental_training_weight
			<
				V_d,
				int,
				Net_func,
				Space_func,
				Net_top,
				Space_top,
				size_t,
				double
			> Experimental_weight;

			// define it
			Experimental_weight experimental_weight ( g_f_net, g_f_space, hex_top, e_d, 1, 2 );

			// other experimental training generalized weight
			typedef neural_net::Experimental_training_weight
			<
				V_d,
				int,
				Net_func,
				Co_f_space,
				Net_top,
				Space_top,
				size_t,
				double
			> Experimental_weight_co;

			// definition
			Experimental_weight_co experimental_weight_co ( g_f_net, co_f_space, hex_top, e_d, 0, 1 );

			// construction of the training functional
			typedef neural_net::Wtm_classical_training_functional
			<
				V_d,
				double,
				int,
				size_t,
				Classic_weight
			> Wtm_c_l_f;

			// definition
			Wtm_c_l_f wtm_c_l_f ( classic_weight, 0.3 );

			// construction of another functional
			typedef neural_net::Wtm_classical_training_functional
			<
				V_d,
				double,
				int,
				size_t,
				Experimental_weight
			> Wtm_e_l_f;

			// definition
			Wtm_e_l_f wtm_e_l_f ( experimental_weight, 0.3 );

			// construction of functional
			typedef neural_net::Wtm_classical_training_functional
			<
				V_d,
				double,
				int,
				size_t,
				Experimental_weight_co
			> Wtm_e_l_f_co;

			// definition
			Wtm_e_l_f_co wtm_e_l_f_co ( experimental_weight_co, 0.3 );

			//std::cout << "kohonen_network_2" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network_2 );
			//std::cout << std::endl;

			// construction of algorithm
			typedef neural_net::Wtm_training_algorithm
			<
				Kohonen_network,
				V_d,
				V_v_d::iterator,
				Wtm_c_l_f,
				int,
				size_t
			> Wtm_c_training_alg;

			// definition
			Wtm_c_training_alg wtm_c_train_alg ( wtm_c_l_f, 0, 1 );

			// tricky training
			for ( int i = 0; i < no_epochs; ++i )
			{
				// train network using data
				wtm_c_train_alg ( data.begin(), data.end(), &kohonen_network_2 );

				// shuffle data
				std::random_shuffle ( data.begin(), data.end() );

				// decrease sigma parameter in network will make training proces more sharpen with each epoch,
				// but it have to be done slowly :-)
				wtm_c_train_alg.training_functional.generalized_training_weight.network_function.sigma *= 2.0/3.0;
			}

			//std::cout << "kohonen_network_2" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network_2 );
			//std::cout << std::endl;

			//std::cout << "kohonen_network_3" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network_3 );
			//std::cout << std::endl;

			// construct another algorithm
			typedef neural_net::Wtm_training_algorithm
			<
				Kohonen_network,
				V_d,
				V_v_d::iterator,
				Wtm_e_l_f,
				int,
				size_t
			> Wtm_c_training_alg_e;

			// definition
			Wtm_c_training_alg_e wtm_c_train_alg_e ( wtm_e_l_f, 0, 1 );

			// small helping constant
			const int border = 3;

			for ( int i = 0; i < no_epochs; ++i )
			{
				// repel neurons just at the begining
				if ( i < border )
				{
					// use linear function to modify parameters
					wtm_c_train_alg_e.training_functional.generalized_training_weight.parameter_1
						= - 1.0 / static_cast <double> ( border - 1 ) * static_cast <double> ( i ) + 2.0;
					wtm_c_train_alg_e.training_functional.generalized_training_weight.parameter_0
						= wtm_c_train_alg_e.training_functional.generalized_training_weight.parameter_1
							- 1.0;
				}
				else
				{
					// set "normal" parameters for training functors
					wtm_c_train_alg_e.training_functional.generalized_training_weight.parameter_1
						= 1;
					wtm_c_train_alg_e.training_functional.generalized_training_weight.parameter_0
						= 0;
				}

				// train
				wtm_c_train_alg_e ( data.begin(), data.end(), &kohonen_network_3 );

				// sharpen training proces
				wtm_c_train_alg_e.training_functional.generalized_training_weight.network_function.sigma
					*= 2.0 / 3.0;

				// shuffle data
				std::random_shuffle ( data.begin(), data.end() );
			}

			//std::cout << "kohonen_network_3" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network_3 );
			//std::cout << std::endl;

			//std::cout << "kohonen_network_4" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network_4 );
			//std::cout << std::endl;

			// construct another algorithm
			typedef neural_net::Wtm_training_algorithm
			<
				Kohonen_network,
				V_d,
				V_v_d::iterator,
				Wtm_e_l_f_co,
				int,
				size_t
			> Wtm_c_training_alg_e_co;

			// definition
			Wtm_c_training_alg_e_co wtm_c_train_alg_e_co ( wtm_e_l_f_co, 0, 1 );

			// similar training
			for ( int i = 0; i < no_epochs; ++i )
			{
				if ( i < border )
				{
					wtm_c_train_alg_e_co.training_functional.generalized_training_weight.parameter_1
						= - 1.0 / static_cast<double> (border - 1) * static_cast<double> (i) + 2.0;
					wtm_c_train_alg_e_co.training_functional.generalized_training_weight.parameter_0
						= wtm_c_train_alg_e_co.training_functional.generalized_training_weight.parameter_1
							- 1.0;
				}
				else
				{
					wtm_c_train_alg_e_co.training_functional.generalized_training_weight.parameter_1
						= 1;
					wtm_c_train_alg_e_co.training_functional.generalized_training_weight.parameter_0
						= 0;
				}

				wtm_c_train_alg_e_co ( data.begin(), data.end(), &kohonen_network_4 );

				wtm_c_train_alg_e.training_functional.generalized_training_weight.network_function.sigma *= 2.0/3.0;

				std::random_shuffle ( data.begin(), data.end() );
			}

			//std::cout << "kohonen_network_4" << std::endl;
			neural_net::print_network_weights ( std::cout, kohonen_network_4 );
			//std::cout << std::endl;
		}
		catch ( std::runtime_error & e)
		{
			std::cerr << e.what() << std::endl;
		}

	}
	catch ( std::exception & e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}
	catch (...)
	{
		std::cerr << "Exception of unknown type!\n";
		return 1;
	}

#ifdef PMAP
	std::cout << "Press any key.";
	::getc (key);
#endif //PMAP

	return 0;
}
