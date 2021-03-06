 /* ------------------------------------------------------------------
  * GEM - Graphics Environment for Multimedia
  *
  *  Copyright (c) 2002-2011 IOhannes m zmölnig. forum::für::umläute. IEM. zmoelnig@iem.at
  *	zmoelnig@iem.kug.ac.at
  *  For information on usage and redistribution, and for a DISCLAIMER
  *  OF ALL WARRANTIES, see the file, "GEM.LICENSE.TERMS"
  *
  *  this file has been generated...
  * ------------------------------------------------------------------
  */

#ifndef _INCLUDE__GEM_OPENGL_GEMGLPOINTSIZE_H_
#define _INCLUDE__GEM_OPENGL_GEMGLPOINTSIZE_H_

#include "Base/GemGLBase.h"

/*
 CLASS
	GEMglPointSize
 KEYWORDS
	openGL	0
 DESCRIPTION
	wrapper for the openGL-function
	"glPointSize( GLfloat size)"
 */

class GEM_EXTERN GEMglPointSize : public GemGLBase
{
	CPPEXTERN_HEADER(GEMglPointSize, GemGLBase);

	public:
	  // Constructor
	  GEMglPointSize (t_float);	// CON

	protected:
	  // Destructor
	  virtual ~GEMglPointSize ();
	  // Do the rendering
	  virtual void	render (GemState *state);

	// variables
	  GLfloat	size;		// VAR
	  virtual void	sizeMess(t_float);	// FUN


	private:

	// we need some inlets
	  t_inlet *m_inlet[1];

	// static member functions
	  static void	 sizeMessCallback (void*, t_float);
};
#endif // for header file
