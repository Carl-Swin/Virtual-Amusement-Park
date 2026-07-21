#pragma once

#include <Fl/gl.h>
#include "libtarga.h"

#define IMG_ARRAY_LEN 13

#define CROWBAR_SCALE 0.31
#define CROWBAR_SHIFT 0.11

#define CROSSHAIR_SCALE 0.01

#ifndef GL_ONE_MINUS_SRC_ALPHA
#define GL_ONE_MINUS_SRC_ALPHA 0x0310
#endif // !GL_ONE_MINUS_SRC_ALPHA

class Crowbar {
private:
    bool    initialized;                   // Whether or not we have been initialised.

    GLuint  texture_obj[IMG_ARRAY_LEN];    // The object for the grass texture.

    ubyte* image_data[IMG_ARRAY_LEN];
    int	    image_height, image_width;

    GLuint crosshair_obj;
    ubyte* crosshair_data;
    int	   crosshair_image_height, crosshair_image_width;

    double x_at, y_at, z_at;
    double theta, phi;

    double animate_index;

    void    Build(void);

public:
    // Constructor. Can't do initialization here because we are
    // created before the OpenGL context is set up.
    Crowbar(void) { initialized = false; };

    // Destructor. Frees the display lists and texture object.
    ~Crowbar(void);

    bool    Initialize(void);
    void    Draw(void);
    void    Update(double x_set, double y_set, double z_set, double theta_set, double phi_set);
    unsigned int Animate(float dt);
};