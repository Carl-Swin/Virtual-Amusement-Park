#ifndef _FERRISWHEEL_H_
#define _FERRISWHEEL_H_

#include <math.h>
#include <Fl/gl.h>
#include "CubicBspline.h"

#define WHEEL_RADIUS 20.0
#define WHEEL_HEIGHT 8.0

class Ferris_Wheel {
  private:
    GLubyte 	    stand_list;

    bool    	    initialized;    // Whether or not we have been initialized.
    float	    speed;	    // The train's speed, in world coordinates

    // Additions -----------------------------------------------
    
    int fractal_cap = 1;
    int wheel_precision = 2;
    double rotation = 0.0; // Degrees

    // ---------------------------------------------------------

  public:
    // Constructor
    Ferris_Wheel(void) { initialized = false; speed = 10.0f; };

    // Destructor
    ~Ferris_Wheel(void);

    bool    Initialize(void);	// Gets everything set up for drawing.
    void    Update(float);	// Updates the location of the train
    void    Draw(void);		// Draws everything.

    // Additions -----------------------------------------------

    int Increment_Precision();
    int Decrement_Precision();

    int Increment_Fractal();
    int Decrement_Fractal();

    void Set_Coords(float& x_at, float& y_at, float& z_at);

    void Build_Leg();
    void Build_Stand();
    void Build_Wheel();
    void Build_Pod();
    void Recursive_Wheel(unsigned long Index, double Rotate);
};


#endif // _FERRISWHEEL_H_

