/*
 * bl_fs.h
 *
 *  Created on: Apr 10, 2018
 *      Author: chenm
 */

#ifndef BL_FS_H_
#define BL_FS_H_

#include <stdint.h>
#include "fatfs/src/ff.h"

struct fs_file {
  int id;
  void *pextension;
};

void BL_fsinit(void);
void BL_fstick(uint32_t ui32TickMS);

struct fs_file * BL_fopen(int id);

void BL_fclose();

//void BL_fread();
//
//void BL_fwrite();


#endif /* BL_FS_H_ */
