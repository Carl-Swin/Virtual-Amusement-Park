/*
 * Track.h: Header file for a class that draws the train and its track.
 *
 * (c) 2001: Stephen Chenney, University of Wisconsin at Madison.
 */


#ifndef _TRAINTRACK_H_
#define _TRAINTRACK_H_

#include <math.h>
#include <Fl/gl.h>
#include "CubicBspline.h"

#define CART_NUM 6

class Track {
  private:
    GLubyte 	    track_list;	    // The display list for the track.
    GLubyte 	    train_list[CART_NUM];	    // The display lists for each train cart.
    bool    	    initialized;    // Whether or not we have been initialized.
    CubicBspline    *track;	    // The spline that defines the track.
    float	    posn_on_track;  // The train's parametric position on the
				    // track.
    float	    speed;	    // The train's speed, in world coordinates

    static const int	TRACK_NUM_CONTROLS;	// Constants about the track.
    static const float 	TRACK_CONTROLS[][3];
    static const float 	TRAIN_ENERGY;

  public:
    // Constructor
    Track(void) { initialized = false; posn_on_track = 0.0f; speed = 0.0f; };

    // Destructor
    ~Track(void);

    bool    Initialize(void);	// Gets everything set up for drawing.
    void    Update(float);	// Updates the location of the train
    void    Draw(void);		// Draws everything.

    // Additions -----------------------------------------------

    float Get_posn();
    float Get_speed();
    void Get_Coords(const float posn, float* Coords);

    void Cart_Build();
    void Head_Cart_Build();

    void Track_Sweep(CubicBspline& refined, int n_refined);
    void Track_Build(int i, int n_refined, float* p, float* p_n, double Theta, double Theta_n, double Sign, double Sign_n, double Scale);
};


#endif

