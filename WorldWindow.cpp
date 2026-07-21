/*
 * CS559 Maze Project
 *
 * Class file for the WorldWindow class.
 *
 * (c) Stephen Chenney, University of Wisconsin at Madison, 2001-2002
 *
 */

#include "WorldWindow.h"
#include <Fl/math.h>
#include <Fl/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <float.h>

const double WorldWindow::FOV_X = 45.0;

WorldWindow::WorldWindow(int x, int y, int width, int height, char *label)
	: Fl_Gl_Window(x, y, width, height, label)
{
    button = -1;

    // Initial viewing parameters.
    phi = 45.0f;
    theta = 0.0f;
    dist = 100.0f;
    x_at = 0.0f;
    y_at = 0.0f;

	// Additions -------------------------------
	z_at = 2.0f;

	fp_dx = fp_dy = 0;
	x_sensitivity = 10.0;
	y_sensitivity = 20.0;

	persp_state = 0;
	key_state = 0;

	crowbar_flag = true;
	animate_flag = false;

	fb_speed = lr_speed = 0.0;
	accel = 10.0;
	decel = 2.0;
}


void
WorldWindow::draw(void)
{
    double  eye[3];
    float   color[4], dir[4];

    if ( ! valid() )
    {
		// Stuff in here doesn't change from frame to frame, and does not
		// rely on any coordinate system. It only has to be done if the
		// GL context is damaged.

		double	fov_y;

		// Sets the clear color to sky blue.
		glClearColor(0.53f, 0.81f, 0.92f, 1.0);

		// Turn on depth testing
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Turn on back face culling. Faces with normals away from the viewer
		// will not be drawn.
		glEnable(GL_CULL_FACE);

		// Enable lighting with one light.
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);

		// Ambient and diffuse lighting track the current color.
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		// Turn on normal vector normalization. You don't have to give unit
		// normal vector, and you can scale objects.
		glEnable(GL_NORMALIZE);

		// Set up the viewport.
		glViewport(0, 0, w(), h());

		// Set up the persepctive transformation.
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		fov_y = 360.0f / M_PI * atan(h() * tan(FOV_X * M_PI / 360.0) / w());
		gluPerspective(fov_y, w() / (float)h(), 1.0, 1000.0);

		// Do some light stuff. Diffuse color, and zero specular color
		// turns off specular lighting.
		color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
		glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
		color[0] = 0.0f; color[1] = 0.0f; color[2] = 0.0f; color[3] = 1.0f;
		glLightfv(GL_LIGHT0, GL_SPECULAR, color);

		// Initialize all the objects.
		ground.Initialize();
		traintrack.Initialize();

		// Additions --------------------------------------------------------

		tree.Initialize();
		crowbar.Initialize();
		ferris_wheel.Initialize();

		// ------------------------------------------------------------------
    }

    // Stuff out here relies on a coordinate system or must be done on every
    // frame.

    // Clear the screen. Color and depth.
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Set up the viewing transformation. The viewer is at a distance
    // dist from (x_at, y_ay, 2.0) in the direction (theta, phi) defined
    // by two angles. They are looking at (x_at, y_ay, 2.0) and z is up.
    eye[0] = x_at + dist * cos(theta * M_PI / 180.0) * cos(phi * M_PI / 180.0);
    eye[1] = y_at + dist * sin(theta * M_PI / 180.0) * cos(phi * M_PI / 180.0);
    eye[2] = z_at + dist * sin(phi * M_PI / 180.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], x_at, y_at, z_at, 0.0, 0.0, 1.0);

    // Position the light source. This has to happen after the viewing
    // transformation is set up, so that the light stays fixed in world
    // space. This is a directional light - note the 0 in the w component.
    dir[0] = 1.0; dir[1] = 1.0; dir[2] = 1.0; dir[3] = 0.0;
    glLightfv(GL_LIGHT0, GL_POSITION, dir);

    // Draw stuff. Everything.
    ground.Draw();
    traintrack.Draw();

	// Additions --------------------------------------------------------
	
	ferris_wheel.Draw();

	tree.Draw();
	if (persp_state != 0 && crowbar_flag == true) crowbar.Draw();

	// ------------------------------------------------------------------
}


void
WorldWindow::Drag(float dt)
{
    int	    dx = x_down - x_last;
    int     dy = y_down - y_last;

    switch ( button )
    {
      case FL_LEFT_MOUSE:
	// Left button changes the direction the viewer is looking from.
	theta = theta_down + 360.0f * dx / (float)w();
	while ( theta >= 360.0f )
	    theta -= 360.0f;
	while ( theta < 0.0f )
	    theta += 360.0f;
	phi = phi_down + 90.0f * dy / (float)h();
	if ( phi > 88.0f )
	    phi = 88.0f;
	if ( phi < -5.0f )
	    phi = -5.0f;
	break;
      case FL_MIDDLE_MOUSE:
	// Middle button moves the viewer in or out.
	dist = dist_down - ( 0.5f * dist_down * dy / (float)h() );
	if ( dist < 1.0f )
	    dist = 1.0f;
	break;
      case FL_RIGHT_MOUSE: {
	// Right mouse button moves the look-at point around, so the world
	// appears to move under the viewer.
	float	x_axis[2];
	float	y_axis[2];

	x_axis[0] = -(float)sin(theta * M_PI / 180.0);
	x_axis[1] = (float)cos(theta * M_PI / 180.0);
	y_axis[0] = x_axis[1];
	y_axis[1] = -x_axis[0];

	x_at = x_at_down + 100.0f * ( x_axis[0] * dx / (float)w()
				    + y_axis[0] * dy / (float)h() );
	y_at = y_at_down + 100.0f * ( x_axis[1] * dx / (float)w()
				    + y_axis[1] * dy / (float)h() );
	} break;
      default:;
    }
}

// Additions --------------------------------------------------------------------------------------

void WorldWindow::check_key_state(float dt) {
	crowbar_animate(dt);
	first_person_movement(dt);

	return;
}

void WorldWindow::crowbar_animate(float dt) {
	if (key_state & (1 << 7)) animate_flag = true;

	if (animate_flag) {
		unsigned int Index = crowbar.Animate(dt);
		if (Index == 0) {
			animate_flag = false;
		}
	}
}

void WorldWindow::first_person_movement(float dt) {
	z_at = 2.0;
	accel = 2.5;
	float speed_cap = 10.0;

	float fb_trig = (theta * M_PI / 180.0);
	float lr_trig = (theta * M_PI / 180.0) + M_PI / 2.0;

	if (key_state & (1 << 5)) {
		accel = 5.0;
		speed_cap = 25.0;
	}
	if (key_state & (1 << 6)) {
		z_at = 1.0;
		speed_cap = 3.0;
	}

	if (key_state & (1 << 0)) (lr_speed > speed_cap) ? lr_speed = speed_cap : lr_speed += accel;
	if (key_state & (1 << 2)) (lr_speed < -speed_cap) ? lr_speed = -speed_cap : lr_speed -= accel;

	if (key_state & (1 << 1)) (fb_speed > speed_cap) ? fb_speed = speed_cap : fb_speed += accel;
	if (key_state & (1 << 3)) (fb_speed < -speed_cap) ? fb_speed = -speed_cap : fb_speed -= accel;

	if (!(key_state & 0b0101)) {
		if (lr_speed < -decel) lr_speed += decel;
		else if (lr_speed > decel) lr_speed -= decel;
		else lr_speed = 0.0;
	}
	if (!(key_state & 0b1010)) {
		if (fb_speed < -decel) fb_speed += decel;
		else if (fb_speed > decel) fb_speed -= decel;
		else fb_speed = 0.0;
	}

	x_at += (cos(fb_trig) * fb_speed + cos(lr_trig) * lr_speed) * dt;
	y_at += (sin(fb_trig) * fb_speed + sin(lr_trig) * lr_speed) * dt;
}

void WorldWindow::first_person_camera(float dt) {
	fp_dx += x_down - x_last;
	fp_dy += y_down - y_last;

	SetCursorPos(x() + (w() / 2), y() + (h() / 2));

	theta = 360.0f * fp_dx * dt * x_sensitivity / (float)w();
	if (theta >= 360.0f) {
		theta -= 360.0f;
		fp_dx = 0;
	}
	if (theta < -360.0f) {
		theta += 360.0f;
		fp_dx = 0;
	}
	phi = -(90.0f * fp_dy * dt * y_sensitivity / (float)h());
	if (phi > 89.0f) {
		phi = 89.0f;
		fp_dy -= y_down - y_last;
	}
	if (phi < -89.0f) {
		phi = -89.0f;
		fp_dy -= y_down - y_last;
	}


	return;
}

unsigned char WorldWindow::set_persp_state(unsigned char new_persp_state) {
	persp_state = new_persp_state;

	switch (persp_state) {
	case 0:
		theta_down = theta = 0.0;
		phi_down = phi = 45.0;

		cursor(FL_CURSOR_DEFAULT);

		x_at = y_at = 0.0;
		z_at = 2.0;
		dist = dist_down = 100.0;
		break;
	case 1:
		fp_dx = fp_dy = 0.0;

		cursor(FL_CURSOR_NONE);
		SetCursorPos(x() + (w() / 2), y() + (h() / 2));

		x_last = x_down = w() / 2;
		y_last = y_down = h() / 2;
		phi_down = phi;
		theta_down = theta;
		dist = dist_down = 1.0 + DIST_TO_0;
		x_at_down = x_at = 0.0;
		y_at_down = y_at = 0.0;
		break;
	case 2:
		fp_dx = fp_dy = 0.0;

		cursor(FL_CURSOR_NONE);
		SetCursorPos(x() + (w() / 2), y() + (h() / 2));

		x_last = x_down = w() / 2;
		y_last = y_down = h() / 2;
		phi_down = phi;
		theta_down = theta;
		dist = dist_down = 1.0 + DIST_TO_0;
		break;
	case 3:
		fp_dx = fp_dy = 0.0;

		cursor(FL_CURSOR_NONE);
		SetCursorPos(x() + (w() / 2), y() + (h() / 2));

		x_last = x_down = w() / 2;
		y_last = y_down = h() / 2;
		phi_down = phi;
		theta_down = theta;
		dist = dist_down = 1.0 + DIST_TO_0;
		break;
	default:
		cursor(FL_CURSOR_DEFAULT);
		break;
	}

	return persp_state;
}

// ------------------------------------------------------------------------------------------------

bool
WorldWindow::Update(float dt)
{
	float position_shift;
	float coords[3] = { 0.0 };

    // Update the view. This gets called once per frame before doing the
    // drawing.
	switch (persp_state) {
	case 0:
		if (button != -1) Drag(dt);  // Only do anything if the mouse button is down. 

		break;
	case 1:
		check_key_state(dt);
		first_person_camera(dt);
		break;
	case 2:
		position_shift = (floor(traintrack.Get_posn()) == 1 || floor(traintrack.Get_posn()) == 3) ? -0.02 : 0.02;

		traintrack.Get_Coords(traintrack.Get_posn() + (position_shift * sin((traintrack.Get_posn() - floor(traintrack.Get_posn())) * M_PI)) + 0.01, coords);
		x_at = coords[0];
		y_at = coords[1];
		z_at = coords[2] + 1.25;

		crowbar_animate(dt);
		first_person_camera(dt);
		break;
	case 3:
		ferris_wheel.Set_Coords(x_at, y_at, z_at);

		crowbar_animate(dt);
		first_person_camera(dt);
		break;
	}

    // Animate the train.5
    traintrack.Update(dt);

	ferris_wheel.Update(dt);

	tree.Update(theta, phi);
	crowbar.Update(x_at, y_at, z_at, theta, phi);

    return true;
}


int
WorldWindow::handle(int event)
{
    // Event handling routine. Only looks at mouse events.
    // Stores a bunch of values when the mouse goes down and keeps track
    // of where the mouse is and what mouse button is down, if any.

	/* Display: event
	for (int i = (sizeof(int) * 8) - 1; i >= 0; --i) printf("%d", (event & (1 << i)) ? 1 : 0);
	printf("\t{ %d }\n", event);
	//*/

    switch ( event )
    {
	case FL_PUSH:
		switch (persp_state) {
		case 0:
			button = Fl::event_button();
			x_last = x_down = Fl::event_x();
			y_last = y_down = Fl::event_y();
			phi_down = phi;
			theta_down = theta;
			dist_down = dist;
			x_at_down = x_at;
			y_at_down = y_at;
			break;
		case 1:
		case 2:
		case 3:
			if (Fl::event_button() == FL_LEFT_MOUSE) key_state |= (1 << 7);  // Left Mouse
			break;
		}
		break;
// --------------------------------------------------------------------------------------------------------------
	case FL_DRAG:
		x_last = Fl::event_x();
		y_last = Fl::event_y();
		break;
// --------------------------------------------------------------------------------------------------------------
	case FL_RELEASE:
		switch (persp_state) {
		case 0:
			button = -1;
			break;
		case 1:
		case 2:
		case 3:
			if (Fl::event_button() == FL_LEFT_MOUSE) key_state &= (255 ^ (1 << 7));  // Left Mouse
			break;
		}
		break;
// --------------------------------------------------------------------------------------------------------------
	case FL_MOVE:
		switch (persp_state) {
		case 1:
		case 2:
		case 3:
			handle(FL_DRAG);
			break;
		}
		break;
// --------------------------------------------------------------------------------------------------------------
	case FL_KEYDOWN:
		if (Fl::event_button() == 0xffff017d) {  // E
			crowbar_flag = !crowbar_flag;
		}

		if (Fl::event_button() == 0x00000069) {  // Left
			ferris_wheel.Decrement_Precision();
		}
		if (Fl::event_button() == 0x0000006b) {  // Right
			ferris_wheel.Increment_Precision();
		}
		if (Fl::event_button() == 0x0000006a) {  // Up
			ferris_wheel.Increment_Fractal();
		}
		if (Fl::event_button() == 0x0000006c) {  // Down
			ferris_wheel.Decrement_Fractal();
		}

		if (Fl::event_button() == 0xffff0145) {  // NumLine -
			tree.Decrement_Trees();
		}
		if (Fl::event_button() == 0xffff0155) {  // NumLine +
			tree.Increment_Trees();
		}

		if (Fl::event_button() == 0xffff0148) {  // NumLine 0
			//printf("persp_state: %d\n", set_persp_state(0));
			printf("\b%d", set_persp_state(0));
			fflush(stdout);
		}
		if (Fl::event_button() == 0xffff0149) {  // NumLine 1
			//printf("persp_state: %d\n", set_persp_state(1));
			printf("\b%d", set_persp_state(1));
			fflush(stdout);
		}
		if (Fl::event_button() == 0xffff014a) {  // NumLine 2
			//printf("persp_state: %d\n", set_persp_state(2));
			printf("\b%d", set_persp_state(2));
			fflush(stdout);
		}
		if (Fl::event_button() == 0xffff014b) {  // NumLine 3
			//printf("persp_state: %d\n", set_persp_state(3));
			printf("\b%d", set_persp_state(3));
			fflush(stdout);
		}
		if (Fl::event_button() == 0xffff014c) {  // NumLine 4
			//printf("persp_state: %d\n", set_persp_state(4));
			printf("\b%d", set_persp_state(4));
			fflush(stdout);
		}

		/* Display: Fl::event_button()
		for (int i = (sizeof(int) * 8) - 1; i >= 0; --i) printf("%d", (Fl::event_button() & (1 << i)) ? 1 : 0);
		printf("\t{ 0x%x }\n", Fl::event_button());
		//*/
		if (Fl::event_button() == 0xffff017c) key_state |= (1 << 0);  // D
		if (Fl::event_button() == 0xffff018b) key_state |= (1 << 1);  // S
		if (Fl::event_button() == 0xffff0179) key_state |= (1 << 2);  // A
		if (Fl::event_button() == 0xffff018f) key_state |= (1 << 3);  // W
		if (Fl::event_button() == 0xffff0138) key_state |= (1 << 4);  // Space
		if (Fl::event_button() == 0x000000f9) key_state |= (1 << 5);  // Shift
		if (Fl::event_button() == 0x000000fb) key_state |= (1 << 6);  // Ctrl
		//if (Fl::event_button() == FL_LEFT_MOUSE) key_state |= (1 << 7);  // Left Mouse (in FL_PUSH)
		break;
// --------------------------------------------------------------------------------------------------------------
	case FL_KEYUP:
		if (Fl::event_button() == 0xffff017c) key_state &= (255 ^ (1 << 0));  // D
		if (Fl::event_button() == 0xffff018b) key_state &= (255 ^ (1 << 1));  // S
		if (Fl::event_button() == 0xffff0179) key_state &= (255 ^ (1 << 2));  // A
		if (Fl::event_button() == 0xffff018f) key_state &= (255 ^ (1 << 3));  // W
		if (Fl::event_button() == 0xffff0138) key_state &= (255 ^ (1 << 4));  // Space
		if (Fl::event_button() == 0x000000f9) key_state &= (255 ^ (1 << 5));  // Shift
		if (Fl::event_button() == 0x000000fb) key_state &= (255 ^ (1 << 6));  // Ctrl
		//if (Fl::event_button() == FL_LEFT_MOUSE) key_state &= (255 ^ (1 << 7));  // Left Mouse (in FL_RELEASE)
		break;
    }

    // Pass any other event types on the superclass.
    return Fl_Gl_Window::handle(event);
}


