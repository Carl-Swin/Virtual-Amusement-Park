/*
 * CS559 Maze Project
 *
 * Class header file for the WorldWindow class. The WorldWindow is
 * the window in which the viewer's view of the world is displayed.
 *
 * (c) Stephen Chenney, University of Wisconsin at Madison, 2001-2002
 *
 */

#ifndef _WORLDWINDOW_H_
#define _WORLDWINDOW_H_

#include <Fl/Fl.h>
#include <Fl/Fl_Gl_Window.h>
#include "Ground.h"
#include "Track.h"

#include "Tree.h"
#include "crowbar.h"
#include "FerrisWheel.h"

#define DIST_TO_0 0.00005

// Subclass the Fl_Gl_Window because we want to draw OpenGL in here.
class WorldWindow : public Fl_Gl_Window {
    public:
	// Constructor takes window position and dimensions, the title.
	WorldWindow(int x, int y, int w, int h, char *label);

	// draw() method invoked whenever the view changes or the window
	// otherwise needs to be redrawn.
	void	draw(void);

	// Event handling method. Uses only mouse events.
	int	handle(int);

	// Update the world according to any events that have happened since
	// the last time this method was called.
	bool	Update(float);

    private:
	Ground	    ground;	    // The ground object.
	Track  traintrack;	    // The train and track.

	static const double FOV_X; // The horizontal field of view.

	float	phi;	// Viewer's inclination angle.
	float	theta;	// Viewer's azimuthal angle.
	float	dist;	// Viewer's distance from the look-at point.
	float	x_at;	// The x-coord to look at.
	float	y_at;	// The y-coord to look at. 
	float	z_at;	// The z-coord to look at (mainly for changing height in 1st person)

	int     button;	// The mouse button that is down, -1 if none.
	int	x_last;	// The location of the most recent mouse event
	int	y_last;
	int	x_down; // The location of the mouse when the button was pushed
	int	y_down;
	float   phi_down;   // The view inclination angle when the mouse
			    // button was pushed
	float   theta_down; // The view azimuthal angle when the mouse
			    // button was pushed
	float	dist_down;  // The distance when the mouse button is pushed.
	float	x_at_down;  // The x-coord to look at when the mouse went down.
	float	y_at_down;  // The y-coord to look at when the mouse went down.

	void	Drag(float);	// The function to call for mouse drag events

	// Additions -----------------------------------------------

	Tree tree;
	Crowbar crowbar;
	Ferris_Wheel ferris_wheel;

	int fp_dx;
	int fp_dy;
	float x_sensitivity;
	float y_sensitivity;

	float accel;
	float decel;
	float lr_speed;
	float fb_speed;

	unsigned char key_state;  // shows when key is down (1=down, 0=up) | 0000 0000 (E, Ctrl, Shift, Space) (W, A, S, D)
	unsigned char persp_state;  // indicates where the camera should be | 0: isometric, 1: 1st person, 2: rollercoaster, 3: ferris wheel, 4: 

	bool crowbar_flag;  // Says if crowbar is visible
	bool animate_flag;  // Helps get through the animation frames

	unsigned char set_persp_state(unsigned char new_persp_state);

	void check_key_state(float dt);
	void crowbar_animate(float dt);
	void first_person_movement(float dt);
	void first_person_camera(float dt);
};


#endif
