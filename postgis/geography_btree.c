/**********************************************************************
 * $Id: geography_gist.c 4778 2009-11-10 19:30:43Z pramsey $
 *
 * PostGIS - Spatial Types for PostgreSQL
 * Copyright 2009 Paul Ramsey <pramsey@cleverelephant.ca>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU General Public Licence. See the COPYING file.
 *
 **********************************************************************/

/*
** R-Tree Bibliography
**
** [1] A. Guttman. R-tree: a dynamic index structure for spatial searching.
**     Proceedings of the ACM SIGMOD Conference, pp 47-57, June 1984.
** [2] C.-H. Ang and T. C. Tan. New linear node splitting algorithm for
**     R-Trees. Advances in Spatial Databases - 5th International Symposium,
**     1997
** [3] N. Beckmann, H.-P. Kriegel, R. Schneider, B. Seeger. The R*tree: an
**     efficient and robust access method for points and rectangles. 
**     Proceedings of the ACM SIGMOD Conference. June 1990.
*/
 
#include "postgres.h"
#include "access/gist.h"    /* For GiST */
#include "access/itup.h"
#include "access/skey.h"

#include "../postgis_config.h"

#include "libgeom.h"         /* For standard geometry types. */
#include "lwgeom_pg.h"       /* For debugging macros. */
#include "geography.h"	     /* For utility functions. */

Datum geography_lt(PG_FUNCTION_ARGS);
Datum geography_le(PG_FUNCTION_ARGS);
Datum geography_eq(PG_FUNCTION_ARGS);
Datum geography_ge(PG_FUNCTION_ARGS);
Datum geography_gt(PG_FUNCTION_ARGS);
Datum geography_cmp(PG_FUNCTION_ARGS);

/*
** Utility function to return the center point of a 
** geocentric bounding box. We don't divide by two 
** because we're only using the values for comparison.
*/
static void geography_gidx_center(GIDX *gidx, POINT3D *p)
{
	p->x = GIDX_GET_MIN(gidx, 0) + GIDX_GET_MAX(gidx, 0);
	p->y = GIDX_GET_MIN(gidx, 1) + GIDX_GET_MAX(gidx, 1);
	p->z = GIDX_GET_MIN(gidx, 2) + GIDX_GET_MAX(gidx, 2);
}

/*
** BTree support function. Based on two geographies return true if
** they are "less than" and false otherwise.
*/
PG_FUNCTION_INFO_V1(geography_lt);
Datum geography_lt(PG_FUNCTION_ARGS)
{
	/* Put aside some stack memory and use it for GIDX pointers. */
	char gboxmem1[GIDX_MAX_SIZE];
	char gboxmem2[GIDX_MAX_SIZE];
	GIDX *gbox1 = (GIDX*)gboxmem1;
	GIDX *gbox2 = (GIDX*)gboxmem2;
	POINT3D p1, p2;

	/* Must be able to build box for each argument (ie, not empty geometry) */
	if( ! geography_datum_gidx(PG_GETARG_DATUM(0), gbox1) ||
	    ! geography_datum_gidx(PG_GETARG_DATUM(1), gbox2) )
	{
		PG_RETURN_BOOL(FALSE);
	}
	
	geography_gidx_center(gbox1, &p1);
	geography_gidx_center(gbox2, &p2);
	
	if( p1.x < p2.x || p1.y < p2.y || p1.z < p2.z ) 
		PG_RETURN_BOOL(TRUE);	
		
	PG_RETURN_BOOL(FALSE);	
}

/*
** BTree support function. Based on two geographies return true if
** they are "less than or equal" and false otherwise.
*/
PG_FUNCTION_INFO_V1(geography_le);
Datum geography_le(PG_FUNCTION_ARGS)
{
	/* Put aside some stack memory and use it for GIDX pointers. */
	char gboxmem1[GIDX_MAX_SIZE];
	char gboxmem2[GIDX_MAX_SIZE];
	GIDX *gbox1 = (GIDX*)gboxmem1;
	GIDX *gbox2 = (GIDX*)gboxmem2;
	POINT3D p1, p2;

	/* Must be able to build box for each argument (ie, not empty geometry) */
	if( ! geography_datum_gidx(PG_GETARG_DATUM(0), gbox1) ||
	    ! geography_datum_gidx(PG_GETARG_DATUM(1), gbox2) )
	{
		PG_RETURN_BOOL(FALSE);
	}
	
	geography_gidx_center(gbox1, &p1);
	geography_gidx_center(gbox2, &p2);
	
	if( p1.x <= p2.x || p1.y <= p2.y || p1.z <= p2.z ) 
		PG_RETURN_BOOL(TRUE);	
		
	PG_RETURN_BOOL(FALSE);	
}

/*
** BTree support function. Based on two geographies return true if
** they are "greater than" and false otherwise.
*/
PG_FUNCTION_INFO_V1(geography_gt);
Datum geography_gt(PG_FUNCTION_ARGS)
{
	/* Put aside some stack memory and use it for GIDX pointers. */
	char gboxmem1[GIDX_MAX_SIZE];
	char gboxmem2[GIDX_MAX_SIZE];
	GIDX *gbox1 = (GIDX*)gboxmem1;
	GIDX *gbox2 = (GIDX*)gboxmem2;
	POINT3D p1, p2;

	/* Must be able to build box for each argument (ie, not empty geometry) */
	if( ! geography_datum_gidx(PG_GETARG_DATUM(0), gbox1) ||
	    ! geography_datum_gidx(PG_GETARG_DATUM(1), gbox2) )
	{
		PG_RETURN_BOOL(FALSE);
	}
	
	geography_gidx_center(gbox1, &p1);
	geography_gidx_center(gbox2, &p2);
	
	if( p1.x > p2.x && p1.y > p2.y && p1.z > p2.z ) 
		PG_RETURN_BOOL(TRUE);	
		
	PG_RETURN_BOOL(FALSE);	
}

/*
** BTree support function. Based on two geographies return true if
** they are "greater than or equal" and false otherwise.
*/
PG_FUNCTION_INFO_V1(geography_ge);
Datum geography_ge(PG_FUNCTION_ARGS)
{
	/* Put aside some stack memory and use it for GIDX pointers. */
	char gboxmem1[GIDX_MAX_SIZE];
	char gboxmem2[GIDX_MAX_SIZE];
	GIDX *gbox1 = (GIDX*)gboxmem1;
	GIDX *gbox2 = (GIDX*)gboxmem2;
	POINT3D p1, p2;

	/* Must be able to build box for each argument (ie, not empty geometry) */
	if( ! geography_datum_gidx(PG_GETARG_DATUM(0), gbox1) ||
	    ! geography_datum_gidx(PG_GETARG_DATUM(1), gbox2) )
	{
		PG_RETURN_BOOL(FALSE);
	}
	
	geography_gidx_center(gbox1, &p1);
	geography_gidx_center(gbox2, &p2);
	
	if( p1.x >= p2.x && p1.y >= p2.y && p1.z >= p2.z ) 
		PG_RETURN_BOOL(TRUE);	
		
	PG_RETURN_BOOL(FALSE);	
}

/*
** BTree support function. Based on two geographies return true if
** they are "equal" and false otherwise.
*/
PG_FUNCTION_INFO_V1(geography_eq);
Datum geography_eq(PG_FUNCTION_ARGS)
{
	/* Put aside some stack memory and use it for GIDX pointers. */
	char gboxmem1[GIDX_MAX_SIZE];
	char gboxmem2[GIDX_MAX_SIZE];
	GIDX *gbox1 = (GIDX*)gboxmem1;
	GIDX *gbox2 = (GIDX*)gboxmem2;
	POINT3D p1, p2;

	/* Must be able to build box for each argument (ie, not empty geometry) */
	if( ! geography_datum_gidx(PG_GETARG_DATUM(0), gbox1) ||
	    ! geography_datum_gidx(PG_GETARG_DATUM(1), gbox2) )
	{
		PG_RETURN_BOOL(FALSE);
	}
	
	geography_gidx_center(gbox1, &p1);
	geography_gidx_center(gbox2, &p2);
	
	if( FP_EQUALS(p1.x, p2.x) && FP_EQUALS(p1.y, p2.y) && FP_EQUALS(p1.z, p2.z) ) 
		PG_RETURN_BOOL(TRUE);	
		
	PG_RETURN_BOOL(FALSE);	
}

/*
** BTree support function. Based on two geographies return true if
** they are "equal" and false otherwise.
*/
PG_FUNCTION_INFO_V1(geography_cmp);
Datum geography_cmp(PG_FUNCTION_ARGS)
{
	/* Put aside some stack memory and use it for GIDX pointers. */
	char gboxmem1[GIDX_MAX_SIZE];
	char gboxmem2[GIDX_MAX_SIZE];
	GIDX *gbox1 = (GIDX*)gboxmem1;
	GIDX *gbox2 = (GIDX*)gboxmem2;
	POINT3D p1, p2;

	/* Must be able to build box for each argument (ie, not empty geometry) */
	if( ! geography_datum_gidx(PG_GETARG_DATUM(0), gbox1) ||
	    ! geography_datum_gidx(PG_GETARG_DATUM(1), gbox2) )
	{
		PG_RETURN_BOOL(FALSE);
	}
	
	geography_gidx_center(gbox1, &p1);
	geography_gidx_center(gbox2, &p2);
	
	if( p1.x > p2.x && p1.y > p2.y && p1.z > p2.z ) 
		PG_RETURN_INT32(1);	

	if( FP_EQUALS(p1.x, p2.x) && FP_EQUALS(p1.y, p2.y) && FP_EQUALS(p1.z, p2.z) ) 
		PG_RETURN_INT32(0);	
	
	PG_RETURN_INT32(-1);	

}