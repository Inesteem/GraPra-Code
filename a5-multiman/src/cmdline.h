/* $Id$ */
#ifndef __CMDLINE_H__ 
#define __CMDLINE_H__ 

#include <string>
#include <unistd.h>

//! \brief Translated command line options
struct Cmdline
{
	bool verbose;	//!< wheather or not to be verbose
	int res_x, res_y;

	Cmdline() : verbose(false), res_x(1000), res_y(1000)
	{
	}
};

extern Cmdline cmdline;

int parse_cmdline(int argc, char **argv);

#endif

