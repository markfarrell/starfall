#pragma once

namespace Starfall {
	/** 
	 * Read a block of data of const size from the packet first.
	 */
	class Head {
		public:

			/**
			 * Values will be read from the received bytes and stored in this variables for each instance of the header
			 */
			int begin;
			int opcode;
			int bodysize;
			int end; 

			/**
			 * This size represents how many bytes will be extracted from the packet to read the header.
			 * Declare the static member inline => The compiler will copy its contents
			 */
			static int const size = 16;


			/**
			 * Construct - Set default values for all integers in the head.
			 */
			Head();
	};

}
