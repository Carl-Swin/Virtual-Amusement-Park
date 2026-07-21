#pragma once

#ifndef _TREE_H_
#define _TREE_H_

#include <FL/math.h>
#include <Fl/gl.h>

#define MAX_TREE 200

class Tree {
private:
    GLubyte display_list;   // The display list that does all the work.
    GLuint  texture_obj;    // The object for the grass texture.
    bool    initialized;    // Whether or not we have been initialised.

    unsigned int max_trees = 100;

    double theta, phi;
    double random_x[MAX_TREE] = { 0.0 };
    double random_y[MAX_TREE] = { 0.0 };

public:
    // Constructor. Can't do initialization here because we are
    // created before the OpenGL context is set up.
    Tree(void) { display_list = 0; initialized = false; };

    // Destructor. Frees the display lists and texture object.
    ~Tree(void);

    // Initializer. Creates the display list.
    bool    Initialize(void);

    // Does the drawing.
    void    Draw(void);

    void Update(double theta_set, double phi_set);

    int Increment_Trees();
    int Decrement_Trees();
};


#endif  // _TREE_H_
