/*
 *  tooLAME: an optimized mpeg 1/2 layer 2 audio encoder
 *
 *  Copyright (C) 2001-2004 Michael Cheng
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 */


#ifndef	_PSYCHO_3_H_
#define	_PSYCHO_3_H_

void psycho_3 (toolame_options *glopts, short int buffer[2][1152], FLOAT scale[2][32], FLOAT ltmin[2][32]);

psycho_3_mem *psycho_3_init( toolame_options *glopts );

void psycho_3_deinit(psycho_3_mem **mem);


#endif
