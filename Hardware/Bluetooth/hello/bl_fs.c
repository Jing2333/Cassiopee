/*
 * bl_fs.c
 *
 *  Created on: Apr 10, 2018
 *      Author: chenm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/ustdlib.c"
#include "fatfs/src/diskio.h"
#include "fatfs/src/ff.h"
#include "fatfs/src/ff.c"
#include "bl_fs.h"




//*****************************************************************************
//
// Initialize the file system.
//
//*****************************************************************************
void
BL_fsinit(void)
{
    //
    // Initialize and mount the Fat File System.
    //
    static FATFS g_sFatFs;
    f_mount(0, &g_sFatFs);
    //f_mount(0, &g_sFatFs);
}

//*****************************************************************************
//
// File System tick handler.
//
//*****************************************************************************
void
BL_fstick(uint32_t ui32TickMS)
{
    static uint32_t ui32TickCounter = 0;
    //
    // Increment the tick counter.
    //
    ui32TickCounter += ui32TickMS;

    //
    // Check to see if the FAT FS tick needs to run.
    //
    if(ui32TickCounter >= 10)
    {
        ui32TickCounter = 0;
        disk_timerproc();
    }
}


struct fs_file * BL_fopen(int id){
    struct fs_file *psFile = NULL;
    FIL *FatFile = NULL;
    FRESULT fresult = FR_OK;

    //
    // Allocate memory for the file system structure.
    //
    psFile = malloc(sizeof(struct fs_file));
    if(psFile == NULL)
    {
        return(NULL);
    }

        //
        // Allocate memory for the Fat File system handle.
        //
        FatFile = malloc(sizeof(FIL));
        if(FatFile == NULL)
        {
            free(psFile);
            return(NULL);
        }

        //
        // Attempt to open the file on the Fat File System.
        //
        char filename[10];
        sprintf(filename,"%d.txt",id);
        fresult = f_open(FatFile, filename, FA_WRITE);
        if(fresult == FR_OK)
        {
            psFile->id = id;
            psFile->pextension = FatFile;
            return(psFile);
        }
        //
        // If we get here, we failed to find the file on the Fat File System,
        // so free up the Fat File system handle/object.
        //
        free(FatFile);
        free(psFile);
        return(NULL);
}

void
fs_close(struct fs_file *psFile)
{
    if(psFile->pextension)
    {
       f_close (psFile->pextension);
       free(psFile->pextension);
    }
   free(psFile);
}


