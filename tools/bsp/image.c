/*
Copyright (C) 1999-2007 id Software, Inc. and contributors.
For a list of contributors, see the accompanying CONTRIBUTORS file.

This file is part of GtkRadiant.

GtkRadiant is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GtkRadiant is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GtkRadiant; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

----------------------------------------------------------------------------------

This code has been altered significantly from its original form, to support
several games based on the Quake III Arena engine, in the form of "Q3Map2."

------------------------------------------------------------------------------- */



/* marker */
#define IMAGE_C



/* dependencies */
#include "q3map2.h"
#include "./libs/ddslib/ddslib.h"


/* -------------------------------------------------------------------------------

this file contains image pool management with reference counting. note: it isn't
reentrant, so only call it from init/shutdown code or wrap calls in a mutex

------------------------------------------------------------------------------- */

/*
LoadDDSBuffer()
loads a dxtc (1, 3, 5) dds buffer into a valid rgba image
*/

static void LoadDDSBuffer( byte* buffer, int size, byte** pixels, int* width, int* height )
{
    int		w, h;
    ddsPF_t	pf;
    
    
    /* dummy check */
    if( buffer == NULL || size <= 0 || pixels == NULL || width == NULL || height == NULL )
        return;
        
    /* null out */
    *pixels = 0;
    *width = 0;
    *height = 0;
    
    /* get dds info */
    if( DDSGetInfo( ( ddsBuffer_t* ) buffer, &w, &h, &pf ) )
    {
        Sys_Printf( "WARNING: Invalid DDS texture\n" );
        return;
    }
    
    /* only certain types of dds textures are supported */
    if( pf != DDS_PF_ARGB8888 && pf != DDS_PF_DXT1 && pf != DDS_PF_DXT3 && pf != DDS_PF_DXT5 )
    {
        Sys_Printf( "WARNING: Only DDS texture formats ARGB8888, DXT1, DXT3, and DXT5 are supported (%d)\n", pf );
        return;
    }
    
    /* create image pixel buffer */
    *width = w;
    *height = h;
    *pixels = safe_malloc( w * h * 4 );
    
    /* decompress the dds texture */
    DDSDecompress( ( ddsBuffer_t* ) buffer, *pixels );
}


/*
ImageInit()
implicitly called by every function to set up image list
*/

static void ImageInit( void )
{
    int		i;
    
    
    if( numImages <= 0 )
    {
        /* clear images (fixme: this could theoretically leak) */
        memset( images, 0, sizeof( images ) );
        
        /* generate *bogus image */
        images[ 0 ].name = safe_malloc( strlen( DEFAULT_IMAGE ) + 1 );
        strcpy( images[ 0 ].name, DEFAULT_IMAGE );
        images[ 0 ].filename = safe_malloc( strlen( DEFAULT_IMAGE ) + 1 );
        strcpy( images[ 0 ].filename, DEFAULT_IMAGE );
        images[ 0 ].width = 64;
        images[ 0 ].height = 64;
        images[ 0 ].refCount = 1;
        images[ 0 ].pixels = safe_malloc( 64 * 64 * 4 );
        for( i = 0; i < ( 64 * 64 * 4 ); i++ )
            images[ 0 ].pixels[ i ] = 255;
    }
}



/*
ImageFree()
frees an rgba image
*/

void ImageFree( image_t* image )
{
    /* dummy check */
    if( image == NULL )
        return;
        
    /* decrement refcount */
    image->refCount--;
    
    /* free? */
    if( image->refCount <= 0 )
    {
        if( image->name != NULL )
            free( image->name );
        image->name = NULL;
        if( image->filename != NULL )
            free( image->filename );
        image->filename = NULL;
        free( image->pixels );
        image->width = 0;
        image->height = 0;
        numImages--;
    }
}



/*
ImageFind()
finds an existing rgba image and returns a pointer to the image_t struct or NULL if not found
*/

image_t* ImageFind( const char* filename )
{
    int			i;
    char		name[ 1024 ];
    
    
    /* init */
    ImageInit();
    
    /* dummy check */
    if( filename == NULL || filename[ 0 ] == '\0' )
        return NULL;
        
    /* strip file extension off name */
    strcpy( name, filename );
    StripExtension( name );
    
    /* search list */
    for( i = 0; i < MAX_IMAGES; i++ )
    {
        if( images[ i ].name != NULL && !strcmp( name, images[ i ].name ) )
            return &images[ i ];
    }
    
    /* no matching image found */
    return NULL;
}



/*
ImageLoad()
loads an rgba image and returns a pointer to the image_t struct or NULL if not found
*/

image_t* ImageLoad( const char* filename )
{
    int			i;
    image_t*		image;
    char		name[ 1024 ];
    int			size;
    byte*		buffer = NULL;
    
    
    /* init */
    ImageInit();
    
    /* dummy check */
    if( filename == NULL || filename[ 0 ] == '\0' )
        return NULL;
        
    /* strip file extension off name */
    strcpy( name, filename );
    StripExtension( name );
    
    /* try to find existing image */
    image = ImageFind( name );
    if( image != NULL )
    {
        image->refCount++;
        return image;
    }
    
    /* none found, so find first non-null image */
    image = NULL;
    for( i = 0; i < MAX_IMAGES; i++ )
    {
        if( images[ i ].name == NULL )
        {
            image = &images[ i ];
            break;
        }
    }
    
    /* too many images? */
    if( image == NULL )
        Error( "MAX_IMAGES (%d) exceeded, there are too many image files referenced by the map.", MAX_IMAGES );
        
    /* set it up */
    image->name = safe_malloc( strlen( name ) + 1 );
    strcpy( image->name, name );
    
    /* attempt to load tga */
    StripExtension( name );
    strcat( name, ".tga" );
    size = vfsLoadFile( ( const char* ) name, ( void** ) &buffer, 0 );
    if( size > 0 )
        LoadTGABuffer( buffer, &image->pixels, &image->width, &image->height );
    else
    {
        if( 1 )
        {
            if( 1 )
            {
                /* attempt to load dds */
                StripExtension( name );
                strcat( name, ".dds" );
                size = vfsLoadFile( ( const char* ) name, ( void** ) &buffer, 0 );
                if( size > 0 )
                {
                    LoadDDSBuffer( buffer, size, &image->pixels, &image->width, &image->height );
                    
                    /* debug code */
#if 1
                    {
                        ddsPF_t	pf;
                        DDSGetInfo( ( ddsBuffer_t* ) buffer, NULL, NULL, &pf );
                        Sys_Printf( "pf = %d\n", pf );
                        if( image->width > 0 )
                        {
                            StripExtension( name );
                            strcat( name, "_converted.tga" );
                            WriteTGA( "C:\\games\\quake3\\baseq3\\textures\\rad\\dds_converted.tga", image->pixels, image->width, image->height );
                        }
                    }
#endif
                }
            }
        }
    }
    
    /* free file buffer */
    free( buffer );
    
    /* make sure everything's kosher */
    if( size <= 0 || image->width <= 0 || image->height <= 0 || image->pixels == NULL )
    {
        //%	Sys_Printf( "size = %d  width = %d  height = %d  pixels = 0x%08x (%s)\n",
        //%		size, image->width, image->height, image->pixels, name );
        free( image->name );
        image->name = NULL;
        return NULL;
    }
    
    /* set filename */
    image->filename = safe_malloc( strlen( name ) + 1 );
    strcpy( image->filename, name );
    
    /* set count */
    image->refCount = 1;
    numImages++;
    
    /* return the image */
    return image;
}


