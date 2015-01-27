
// (C) Copyright 2015, Linh Ngo (linhnv [at] soict.hust.edu.vn)

// This file is part of fCTM.

// fCTM is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.

// fCTM is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA

#include "vmf-est-inf.h"

void main(int argc, char* argv[])
{
    corpus *corp;
	if (strcmp(argv[1], "est")==0)
	{	// Eg: ./fCTM est gro10 train-data.txt 10
		read_settings("fCTM-settings.txt");
		corp = read_data(argv[3], 1);
        NTOPICS = atol(argv[4]);
        printf("\nNumber of topics: %d \n", NTOPICS);
        make_directory(argv[2]);
        VMF_Learn(argv[2], corp);
	}
}

