/*
 * World.cpp: Main program file for Project 3, CS 559
 *
 * (c) 2001-2002: Stephen Chenney
 */


#include <Fl/Fl.h>
#include "WorldWindow.h"
#include <stdio.h>


// The time per frame, in seconds (enforced only by timeouts.)
static const float  FRAME_TIME = 0.025f;

static WorldWindow  *world_window; // The window with world view in it

int intro_message();

// This callback is called every 40th of a second if the system is fast
// enough. You should change the variable FRAME_TIME defined above if you
// want to change the frame rate.
static void
Timeout_Callback(void *data)
{
    // Update the motion in the world. This both moves the view and
    // animates the train.
    world_window->Update(FRAME_TIME);
    world_window->redraw();

    // Do the timeout again for the next frame.
    Fl::repeat_timeout(FRAME_TIME, Timeout_Callback);
}


int
main(int argc, char *argv[])
{
    intro_message();

    // Fl::visual(FL_RGB);

    world_window = new WorldWindow(800, 100, 1325, 975, "World");

    world_window->show(argc, argv);

    Fl::add_timeout(0.0, Timeout_Callback, NULL);

    return Fl::run();
}


int intro_message() {
    unsigned int i = 0;

    printf(" %c", (char)218);
    for (i = 0; i < 10; ++i) printf("%c", (char)196);
    printf("%c CS447: Virtual Amusement Park\n", (char)191);
    printf(" %c Controls %c Computer Graphics Final Project\n", (char)179, (char)179);

    printf("%c", (char)201);
    printf("%c", (char)207);
    for (i = 0; i < 10; ++i) printf("%c", (char)205);
    printf("%c", (char)207);
    for (i = 0; i < 35; ++i) printf("%c", (char)205);
    printf("%c\n", (char)187);

    //printf("%c ", (char)186);
    printf("%c Isometric View:\tNumline 0\t\t", (char)186);
    printf("%c\n%c", (char)186, (char)186);
    for (i = 0; i < 47; ++i) printf(" ");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Rotate view around:\tLeft Mouse (Hold)\t");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Zoom camera in/out:\tMiddle Mouse (Hold)\t");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Move camera position:\tRight Mouse (Hold)\t");

    printf("%c\n%c", (char)186, (char)199);
    for (i = 0; i < 47; ++i) printf("%c", (char)196);
    printf("%c\n%c ", (char)182, (char)186);

    printf("First-Person View:\tNumline 1\t\t", (char)186);
    printf("%c\n%c", (char)186, (char)186);
    for (i = 0; i < 47; ++i) printf(" ");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Move camera location:\tW A S D (Hold)\t\t");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Swing crowbar:\tLeft Mouse\t\t");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Sprint:\t\tLeft Shift (Hold)\t");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Crouch:\t\tLeft Ctrl (Hold)\t");
    
    printf("%c\n%c", (char)186, (char)199);
    for (i = 0; i < 47; ++i) printf("%c", (char)196);
    printf("%c\n%c ", (char)182, (char)186);

    printf("Roller Coaster View:\tNumline 2\t\t", (char)186);
    printf("%c\n%c", (char)186, (char)186);
    for (i = 0; i < 47; ++i) printf(" ");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Swing crowbar:\tLeft Mouse\t\t");

    printf("%c\n%c", (char)186, (char)199);
    for (i = 0; i < 47; ++i) printf("%c", (char)196);
    printf("%c\n%c ", (char)182, (char)186);

    printf("Ferris Wheel View:\tNumline 3\t\t", (char)186);
    printf("%c\n%c", (char)186, (char)186);
    for (i = 0; i < 47; ++i) printf(" ");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Swing crowbar:\tLeft Mouse\t\t");

    printf("%c\n%c", (char)186, (char)199);
    for (i = 0; i < 47; ++i) printf("%c", (char)196);
    printf("%c\n%c ", (char)182, (char)186);

    printf("Miscellaneous (Work in all perspectives)\t", (char)186);
    printf("%c\n%c", (char)186, (char)186);
    for (i = 0; i < 47; ++i) printf(" ");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Increase/Decrease Fractal:\tArrow Up/Down\t");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Refine ferris Wheel:\t\tArrow Left/Right");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Change tree density:\t\tNumLine +/-\t");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Change perspective:\t\tNumLine 0-4\t");
    printf("%c\n%c ", (char)186, (char)186);
    printf("Toggle Crowbar:\t\tE\t\t");

    printf("%c\n%c", (char)186, (char)200);
    for (i = 0; i < 47; ++i) printf("%c", (char)205);
    printf("%c\n", (char)188);

    /*
    for (unsigned int i = 0; i < 256; ++i) {
        printf("value %u\t%c\n", i, (char)i);
    }
    //*/

    printf("persp_state: 0");
    fflush(stdout);
    return 0;
}

// IDEAS:  mirror room, ferris wheel, whack-a-mole

// https://web.cecs.pdx.edu/~fliu/courses/cs447/project.html
/* Subdivision --- O
An object defined using subdivision schemes. You must include a key press that 
refines the model, so that we can see the improved quality.You can either 
implement the ones we talk about in our class, or any others. Reading Chapter 
4 of SIGGRAPH 2000 Course Notes on Subdivision for Modelingand Animation.
https://multires.caltech.edu/pubs/sig00notes.pdf

Points: 50
*/

/* Sweep Objects --- O
Add an object created as a sweep, either an extrusion or a surface of revolution.
The important thing is that it be created by moving some basic shape along a path.
The overall object must use at least three different uses of the swept polygon. In 
other words, something like a cylinder isn't enough, but something like two 
cylinders joined to form an elbow is.

Points: 25
*/

/* Hierarchical Animated Model --- O
Add a hierarchical, animated model. The model must combine multiple components in a 
transformation hierarchy. Different models need different hierarchies.

Points: 25
*/

/* Change the Navigation System --- O
The navigation system now is not great. Change it to something better. To get the 
points in this category, you must have a mode where the viewer rides the 
roller-coaster.	

Points: 20
*/

/* Paramatric Instancing --- O
Add an object described by parameters. You must create multiple instances with 
different parameters, and each class of model counts for separate points, not each 
instance.

Points: 20
*/

/* Modeling using software --- X
You can use software like Blender  or Maya to model complex objects. Based on the 
complexity and aesthetics of the objects, you can get extra points ranging from 
10~20. (Blender is free and Maya is not.)	

Points: 10~20
*/