#include <direct.h>
#include "crowbar.h"
#include "libtarga.h"
#include <stdio.h>
#include <GL/glu.h>
#include <FL/math.h>

 // Destructor
Crowbar::~Crowbar(void)
{
    if (initialized)
    {
        for (int i = 0; i < IMG_ARRAY_LEN; ++i) {
            glDeleteTextures(1, &(texture_obj[i]));
        }
    }
}


// Initializer. Returns false if something went wrong, like not being able to
// load the texture.
bool
Crowbar::Initialize(void)
{
    animate_index = 0;
    
    // Load the image for the texture. The texture file has to be in
    // a place where it will be found.
    for (int i = 0; i < IMG_ARRAY_LEN; ++i) {
        char buffer[50] = {'\0'};
        sprintf(buffer, "images\\crowbar\\frame_%d.tga", i);
        if (!(image_data[i] = (ubyte*)tga_load(buffer, &image_width, &image_height, TGA_TRUECOLOR_32)))
        {
            fprintf(stderr, "Couldn't load %s\n", buffer);
            return false;
        }
    }
    if (!(crosshair_data = (ubyte*)tga_load("images\\crosshair\\crosshair.tga", &crosshair_image_width, &crosshair_image_height, TGA_TRUECOLOR_32)))
    {
        fprintf(stderr, "Couldn't load crosshair.tga\n");
        return false;
    }


    for (int i = 0; i < IMG_ARRAY_LEN; ++i) {
        // This creates a texture object and binds it, so the next few operations apply to this texture.
        glGenTextures(1, &(texture_obj[i]));
        glBindTexture(GL_TEXTURE_2D, texture_obj[i]);

        // This sets a parameter for how the texture is loaded and interpreted.
        // basically, it says that the data is packed tightly in the image array.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image_width, image_height, GL_RGBA, GL_UNSIGNED_BYTE, image_data[i]);
    }
    glGenTextures(1, &crosshair_obj);
    glBindTexture(GL_TEXTURE_2D, crosshair_obj);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, crosshair_image_width, crosshair_image_height, GL_RGBA, GL_UNSIGNED_BYTE, crosshair_data);


    // This says what to do with the texture. Modulate will multiply the
    // texture by the underlying color.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;

    return true;
}

// Draw just calls the display list we set up earlier.
void
Crowbar::Draw(void)
{
    Build();
}

void Crowbar::Build(void) {
    double sTheta = sin(theta * M_PI / 180.0);
    double cTheta = cos(theta * M_PI / 180.0);
    double sPhi = sin(phi * M_PI / 180.0);
    double cPhi = cos(phi * M_PI / 180.0);

    double sPcT = sPhi * cTheta;
    double sPsT = sPhi * sTheta;

    // Use white, because the texture supplies the color.
    glColor4f(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Turn on texturing and bind the crosshair texture.
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, crosshair_obj);

    //Crosshair
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(x_at + (-CROSSHAIR_SCALE * (sTheta + sPcT) ),
               y_at + ( CROSSHAIR_SCALE * (cTheta - sPsT) ),
               z_at + ( CROSSHAIR_SCALE * cPhi ));
    glTexCoord2f(0.0, 1.0);
    glVertex3f(x_at + ( CROSSHAIR_SCALE * (sTheta - sPcT) ),
               y_at + (-CROSSHAIR_SCALE * (cTheta + sPsT) ),
               z_at + ( CROSSHAIR_SCALE * cPhi ));
    glTexCoord2f(0.0, 0.0);
    glVertex3f(x_at + ( CROSSHAIR_SCALE * (sTheta + sPcT) ),
               y_at + (-CROSSHAIR_SCALE * (cTheta - sPsT) ),
               z_at + (-CROSSHAIR_SCALE * cPhi ));
    glTexCoord2f(1.0, 0.0);
    glVertex3f(x_at + (-CROSSHAIR_SCALE * (sTheta - sPcT) ),
               y_at + ( CROSSHAIR_SCALE * (cTheta + sPsT) ),
               z_at + (-CROSSHAIR_SCALE * cPhi ));
    glEnd();
    glEnable(GL_LIGHTING);

    //*  // To give crowbar shadow depending on the direction of player
    glNormal3f(sTheta + sPcT, cTheta - sPsT, cPhi);
    /*/  // Crowbar always bright
    glNormal3f(1.0, 1.0, 0.0);
    //*/ 

    // Turn on texturing and bind the crowbar texture.
    glBindTexture(GL_TEXTURE_2D, texture_obj[(unsigned int)animate_index]);

    // Crowbar
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 0.99);
    glVertex3f(x_at + (-CROWBAR_SCALE * (sTheta + sPcT) ) - (CROWBAR_SHIFT * sTheta),
               y_at + ( CROWBAR_SCALE * (cTheta - sPsT) ) + (CROWBAR_SHIFT * cTheta),
               z_at + ( CROWBAR_SCALE * cPhi ));
    glTexCoord2f(0.01, 0.99);
    glVertex3f(x_at + ( CROWBAR_SCALE * (sTheta - sPcT) ) - (CROWBAR_SHIFT * sTheta),
               y_at + (-CROWBAR_SCALE * (cTheta + sPsT) ) + (CROWBAR_SHIFT * cTheta),
               z_at + ( CROWBAR_SCALE * cPhi ));
    glTexCoord2f(0.01, 0.0);
    glVertex3f(x_at + ( CROWBAR_SCALE * (sTheta + sPcT) ) - (CROWBAR_SHIFT * sTheta),
               y_at + (-CROWBAR_SCALE * (cTheta - sPsT) ) + (CROWBAR_SHIFT * cTheta),
               z_at + (-CROWBAR_SCALE * cPhi ));
    glTexCoord2f(1.0, 0.0);
    glVertex3f(x_at + (-CROWBAR_SCALE * (sTheta - sPcT) ) - (CROWBAR_SHIFT * sTheta),
               y_at + ( CROWBAR_SCALE * (cTheta + sPsT) ) + (CROWBAR_SHIFT * cTheta),
               z_at + (-CROWBAR_SCALE * cPhi ));
    glEnd();

    /* 
    system("cls");
    printf("%lf\t%lf\t%lf\n", 
               x_at + ( CROWBAR_SCALE * (sin(Theta) + ( sin(Phi) * cos(Theta) )) ) - (0.0 * cos(Theta)),
               y_at + (-CROWBAR_SCALE * (cos(Theta) - ( sin(Phi) * sin(Theta) )) ) + (0.0 * sin(Theta)),
               z_at + (-CROWBAR_SCALE * (cos(Phi)))
    );
    printf("%lf\t%lf\t%lf\n\n",
               x_at + (-CROWBAR_SCALE * (sin(Theta) - ( sin(Phi) * cos(Theta) )) ) - (0.0 * cos(Theta)),
               y_at + ( CROWBAR_SCALE * (cos(Theta) + ( sin(Phi) * sin(Theta) )) ) + (0.0 * sin(Theta)),
               z_at + (-CROWBAR_SCALE * (cos(Phi)))
    );
    printf("%lf\t%lf\t%lf\t%lf\n\n\n\n", cos(Theta), sin(Theta), cos(Phi), sin(Phi));
    //*/
}

void Crowbar::Update(double x_set, double y_set, double z_set, double theta_set, double phi_set) {
    x_at = x_set;
    y_at = y_set;
    z_at = z_set;
    theta = theta_set;
    phi = phi_set;
}

unsigned int Crowbar::Animate(float dt) {
    return (((unsigned int)animate_index) == IMG_ARRAY_LEN - 1) ? animate_index = 0.0 : floor(animate_index += (dt * 40.0));
}