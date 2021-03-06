/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    A mesh_line

    Copyright (c) 1997-2000 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef _INCLUDE__GEM_GEOS_MESH_LINE_H_
#define _INCLUDE__GEM_GEOS_MESH_LINE_H_

#include "Base/GemShape.h"
#include <vector>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    mesh_line

    Creates a mesh_line

KEYWORDS
    geo

DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXTERN mesh_line : public GemShape
{
    CPPEXTERN_HEADER(mesh_line, GemShape);

    public:

    //////////
    // Constructor
    mesh_line(t_floatarg sizeX);

    protected:

    //////////
    // Destructor
    virtual ~mesh_line(void);


    //////////
    // Do the rendering
    virtual void   renderShape(GemState *state);

    void    setGrid( int valueX);
    void    getTexCoords(void);

    //////////
    // getStuff
    int     gridX;
    float    xsize, xsize0;
    int     alreadyInit;
    std::vector<float>    texCoords;
};

#endif  // for header file
