#ifndef PSYCHO3_H
#define PSYCHO3_H

void psycho_3 (short int buffer[2][1152], FLOAT scale[2][32], FLOAT ltmin[2][32], frame_info *, toolame_options *glopts);

psycho_3_mem *psycho_3_init(toolame_options *glopts, frame_header *header);

void psycho_3_deinit(psycho_3_mem *mem);


#endif
