/*
 * Ground.cpp: A class for drawing the ground.
 *
 * (c) 2001-2002: Stephen Chenney, University of Wisconsin at Madison.
 */


#include "Tree.h"
#include "libtarga.h"
#include <time.h>
#include <stdio.h>
#include <GL/glu.h>

Tree::~Tree(void)
{
    if (initialized)
    {
        glDeleteLists(display_list, 1);
        glDeleteTextures(1, &texture_obj);
    }
}

bool
Tree::Initialize(void)
{
    ubyte* image_data;
    int	    image_height, image_width;

    if (!(image_data = (ubyte*)tga_load("images\\trees\\tree.tga", &image_width, &image_height, TGA_TRUECOLOR_32)))
    {
        fprintf(stderr, "Tree::Initialize: Couldn't load tree.tga\n");
        return false;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenTextures(1, &texture_obj);
    glBindTexture(GL_TEXTURE_2D, texture_obj);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image_width, image_height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    display_list = glGenLists(2);
    glNewList(display_list, GL_COMPILE);
    glColor3f(1.0, 1.0, 1.0);

    //glColor4f(1.0, 1.0, 1.0, 1.0);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_obj);
    
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.0, 2.0, 5.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(0.0, -2.0, 5.0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(0.0, -2.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.0, 2.0, 0.0);
    glEnd();

    glEndList();

    // Generate random locations for trees
    int balance_y = 0;
    double temp_x, temp_y;
    srand(time(NULL));
    for (int i = 0; i < MAX_TREE; ++i) {
        temp_x = (rand() % 98) - 49;
        temp_y = (rand() % 98) - 49;
        (temp_y > 0.0) ? ++balance_y : --balance_y;

        switch (balance_y) {
        case -2:
            ++balance_y;
            --i;
            break;
        case 2:
            --balance_y;
            --i;
            break;
        default:
            if (temp_x * temp_x + temp_y * temp_y > 25.0 * 25.0 && temp_x > -20.0) {
                random_x[i] = temp_x;
                random_y[i] = temp_y;
            }
            else --i;
            break;
        }
    }

    initialized = true;

    return true;
}


void
Tree::Draw(void)
{
    double SinTheta = sin(theta * M_PI / 180.0);
    double CosTheta = -cos(theta * M_PI / 180.0);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f); 
    glDisable(GL_LIGHTING);

    for (int i = 0; i < max_trees; ++i) {
        glPushMatrix();
        glTranslated(random_x[i], random_y[i], 0.0);
        glRotated(phi, SinTheta, CosTheta, 0.0);
        glRotated(theta, 0.0, 0.0, 1.0);

        glCallList(display_list);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
    glDisable(GL_ALPHA_TEST);
}


void Tree::Update(double theta_set, double phi_set) {
    theta = theta_set;
    phi = phi_set;
}

int Tree::Increment_Trees() {
    return (max_trees == MAX_TREE) ? MAX_TREE : ++max_trees;
}
int Tree::Decrement_Trees() {
    return (max_trees == 0) ? 0 : --max_trees;
}