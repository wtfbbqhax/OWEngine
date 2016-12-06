#ifndef IDBSP_ABSTRACT
#define IDBSP_ABSTRACT

/* -------------------------------------------------------------------------------

abstracted bsp file

------------------------------------------------------------------------------- */


#define EXTERNAL_LIGHTMAP		"lm_%04d.tga"

#define MAX_LIGHTMAPS			4			/* RBSP */
#define MAX_LIGHT_STYLES		64
#define	MAX_SWITCHED_LIGHTS		32
#define LS_NORMAL				0x00
#define LS_UNUSED				0xFE
#define	LS_NONE					0xFF

#ifndef _PHYSICSLIB
#define MAX_LIGHTMAP_SHADERS	256

/* ok to increase these at the expense of more memory */
#define	MAX_MAP_MODELS			0x400
#define	MAX_MAP_BRUSHES			0x8000
#define	MAX_MAP_ENTITIES		0x1000		//%	0x800	/* ydnar */
#define	MAX_MAP_ENTSTRING		0x80000		//%	0x40000	/* ydnar */
#define	MAX_MAP_SHADERS			0x400

#define	MAX_MAP_AREAS			0x100		/* MAX_MAP_AREA_BYTES in q_shared must match! */
#define	MAX_MAP_FOGS			30			//& 0x100	/* RBSP (32 - world fog - goggles) */
#define	MAX_MAP_PLANES			0x100000	//%	0x20000	/* ydnar for md */
#define	MAX_MAP_NODES			0x20000
#define	MAX_MAP_BRUSHSIDES		0x100000	//%	0x20000	/* ydnar */
#define	MAX_MAP_LEAFS			0x20000
#define	MAX_MAP_LEAFFACES		0x20000
#define	MAX_MAP_LEAFBRUSHES		0x40000
#define	MAX_MAP_PORTALS			0x20000
#define	MAX_MAP_LIGHTING		0x800000
#define	MAX_MAP_LIGHTGRID		0x100000	//%	0x800000 /* ydnar: set to points, not bytes */
#define	MAX_MAP_VISIBILITY		0x200000

#define	MAX_MAP_DRAW_SURFS		0x20000
#define	MAX_MAP_DRAW_VERTS		0x80000
#define	MAX_MAP_DRAW_INDEXES	0x80000

#define MAX_MAP_ADVERTISEMENTS	30

/* key / value pair sizes in the entities lump */
#define	MAX_KEY					32
#define	MAX_VALUE				1024

/* the editor uses these predefined yaw angles to orient entities up or down */
#define	ANGLE_UP				-1
#define	ANGLE_DOWN				-2

#define	LIGHTMAP_WIDTH			128
#define	LIGHTMAP_HEIGHT			128

#define MIN_WORLD_COORD			(-65536)
#define	MAX_WORLD_COORD			(65536)
#define WORLD_SIZE				(MAX_WORLD_COORD - MIN_WORLD_COORD)


typedef void	( *bspFunc )( const char* );
#endif

typedef struct
{
    int		offset, length;
}
bspLump_t;


typedef struct
{
    char		ident[ 4 ];
    int			version;
    
    bspLump_t	lumps[ 100 ];	/* theoretical maximum # of bsp lumps */
}
bspHeader_t;


typedef struct
{
    float		mins[ 3 ], maxs[ 3 ];
    int			firstBSPSurface, numBSPSurfaces;
    int			firstBSPBrush, numBSPBrushes;
}
bspModel_t;


typedef struct
{
    char		shader[ MAX_QPATH ];
    int			surfaceFlags;
    int			contentFlags;
}
bspShader_t;


/* planes x^1 is allways the opposite of plane x */

typedef struct
{
    float		normal[ 3 ];
    float		dist;
}
bspPlane_t;


typedef struct
{
    int			planeNum;
    int			children[ 2 ];		/* negative numbers are -(leafs+1), not nodes */
    int			mins[ 3 ];			/* for frustom culling */
    int			maxs[ 3 ];
}
bspNode_t;


typedef struct
{
    int			cluster;			/* -1 = opaque cluster (do I still store these?) */
    int			area;
    
    int			mins[ 3 ];			/* for frustum culling */
    int			maxs[ 3 ];
    
    int			firstBSPLeafSurface;
    int			numBSPLeafSurfaces;
    
    int			firstBSPLeafBrush;
    int			numBSPLeafBrushes;
}
bspLeaf_t;


typedef struct
{
    int			planeNum;			/* positive plane side faces out of the leaf */
    int			shaderNum;
    int			surfaceNum;			/* RBSP */
}
bspBrushSide_t;


typedef struct
{
    int			firstSide;
    int			numSides;
    int			shaderNum;			/* the shader that determines the content flags */
}
bspBrush_t;


typedef struct
{
    char		shader[ MAX_QPATH ];
    int			brushNum;
    int			visibleSide;		/* the brush side that ray tests need to clip against (-1 == none) */
}
bspFog_t;


typedef struct
{
    vec3_t		xyz;
    float		st[ 2 ];
    float		lightmap[ MAX_LIGHTMAPS ][ 2 ];	/* RBSP */
    vec3_t		normal;
    byte		color[ MAX_LIGHTMAPS ][ 4 ];	/* RBSP */
}
bspDrawVert_t;

#ifndef _PHYSICSLIB
typedef enum
{
    MST_BAD,
    MST_PLANAR,
    MST_PATCH,
    MST_TRIANGLE_SOUP,
    MST_FLARE,
    MST_FOLIAGE
}
bspSurfaceType_t;
#endif

typedef struct bspGridPoint_s
{
    byte		ambient[ MAX_LIGHTMAPS ][ 3 ];
    byte		directed[ MAX_LIGHTMAPS ][ 3 ];
    byte		styles[ MAX_LIGHTMAPS ];
    byte		latLong[ 2 ];
}
bspGridPoint_t;


typedef struct
{
    int			shaderNum;
    int			fogNum;
    int			surfaceType;
    
    int			firstVert;
    int			numVerts;
    
    int			firstIndex;
    int			numIndexes;
    
    byte		lightmapStyles[ MAX_LIGHTMAPS ];						/* RBSP */
    byte		vertexStyles[ MAX_LIGHTMAPS ];							/* RBSP */
    int			lightmapNum[ MAX_LIGHTMAPS ];							/* RBSP */
    int			lightmapX[ MAX_LIGHTMAPS ], lightmapY[ MAX_LIGHTMAPS ];	/* RBSP */
    int			lightmapWidth, lightmapHeight;
    
    vec3_t		lightmapOrigin;
    vec3_t		lightmapVecs[ 3 ];	/* on patches, [ 0 ] and [ 1 ] are lodbounds */
    
    int			patchWidth;
    int			patchHeight;
}
bspDrawSurface_t;


/* advertisements */
typedef struct
{
    int			cellId;
    vec3_t		normal;
    vec3_t		rect[4];
    char		model[ MAX_QPATH ];
} bspAdvertisement_t;

#endif // !IDBSP_ABSTRACT
