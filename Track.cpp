/*
 * Track.cpp: A class that draws the train and its track.
 *
 * (c) 2001-2002: Stephen Chenney, University of Wisconsin at Madison.
 */


#include "Track.h"
#include <stdio.h>
#include <FL/math.h>


// The control points for the track spline.
const int   Track::TRACK_NUM_CONTROLS = 4;
const float Track::TRACK_CONTROLS[TRACK_NUM_CONTROLS][3] =
		{ { -20.0, -20.0, -18.0 }, { 20.0, -20.0, 40.0 },
		  { 20.0, 20.0, -18.0 }, { -20.0, 20.0, 40.0 } };

// The carriage energy and mass
const float Track::TRAIN_ENERGY = 250.0f;


// Normalize a 3d vector.
static void
Normalize_3(float v[3])
{
    double  l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    if ( l == 0.0 )
	return;

    v[0] /= (float)l;
    v[1] /= (float)l;
    v[2] /= (float)l;
}


// Destructor
Track::~Track(void)
{
    if ( initialized )
    {
	glDeleteLists(track_list, 1);
    for (int i = 0; i < CART_NUM; ++i) glDeleteLists(train_list[i], 1);
    }
}


// Initializer. Would return false if anything could go wrong.
bool
Track::Initialize(void)
{
    CubicBspline    refined(3, true);
    int		    n_refined;
    int		    i;

    // Create the track spline.
    track = new CubicBspline(3, true);
    for ( i = 0 ; i < TRACK_NUM_CONTROLS ; i++ )
	track->Append_Control(TRACK_CONTROLS[i]);

    // Refine it down to a fixed tolerance. This means that any point on
    // the track that is drawn will be less than 0.1 units from its true
    // location. In fact, it's even closer than that.
    track->Refine_Tolerance(refined, 0.1f);
    n_refined = refined.N();

    // Create the display list for the track - just a set of line segments
    // We just use curve evaluated at integer paramer values, because the
    // subdivision has made sure that these are good enough.
    track_list = glGenLists(1);
    glNewList(track_list, GL_COMPILE);
    Track_Sweep(refined, n_refined);
    glEndList();

    // Set up the train. At this point a cube is drawn. NOTE: The
    // x-axis will be aligned to point along the track. The origin of the
    // train is assumed to be at the bottom of the train.
    train_list[0] = glGenLists(1);
    glNewList(train_list[0], GL_COMPILE);
    Head_Cart_Build();
    glEndList();
    for (i = 1; i < CART_NUM; ++i) {
        train_list[i] = glGenLists(1);
        glNewList(train_list[i], GL_COMPILE);
        Cart_Build();
        glEndList();
    }

    initialized = true;

    return true;
}

// Additions -----------------------------------------------

float Track::Get_posn() { return posn_on_track; }
float Track::Get_speed() { return speed; }

void Track::Get_Coords(const float posn, float* Coords) {
    track->Evaluate_Point(posn, Coords);
    return;
}

void Track::Track_Sweep(CubicBspline& refined, int n_refined) {
    int i, i_n;
    float p[3], p_n[3];
    double Theta, Theta_n;

    double Sign, Sign_n;

    for (i = 0; i < n_refined; i++) {
        i_n = (i == n_refined - 1) ? 0 : i + 1;

        //glNormal3f(1.0f, 0.0f, 0.0f);
        refined.Evaluate_Point((float)i, p);
        refined.Evaluate_Point((float)i_n, p_n);
        Theta = atan(p[1] / p[0]);
        Theta_n = atan(p_n[1] / p_n[0]);

        Sign = (p[0] < 0.0) ? -1.0 : 1.0;
        Sign_n = (p_n[0] < 0.0) ? -1.0 : 1.0;

        Track_Build(i, n_refined, p, p_n, Theta, Theta_n, Sign, Sign_n, 0.125);
    }

    /*
    glColor3f(1.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= n_refined; i++)
    {
        refined.Evaluate_Point((float)i, p);
        glVertex3fv(p);
    }
    glEnd();
    //*/
}

void Track::Track_Build(int i, int n_refined, float* p, float* p_n, double Theta, double Theta_n, double Sign, double Sign_n, double Scale) {
// Bottom rail
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.5f, 0.5f);
    // Top
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(p[0] + (cos(Theta) * Scale * Sign),
                p[1] + (sin(Theta) * Scale * Sign),
                p[2]);
    glVertex3f(p_n[0] + (cos(Theta_n) * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * Scale * Sign_n),
                p_n[2]);
    glVertex3f(p_n[0] - (cos(Theta_n) * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * Scale * Sign_n),
                p_n[2]);
    glVertex3f(p[0] - (cos(Theta) * Scale * Sign),
                p[1] - (sin(Theta) * Scale * Sign),
                p[2]);
    // Bottom
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(p[0] - (cos(Theta) * Scale * Sign),
                p[1] - (sin(Theta) * Scale * Sign),
                p[2] - Scale * 2.0);
    glVertex3f(p_n[0] - (cos(Theta_n) * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * Scale * Sign_n),
                p_n[2] - Scale * 2.0);
    glVertex3f(p_n[0] + (cos(Theta_n) * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * Scale * Sign_n),
                p_n[2] - Scale * 2.0);
    glVertex3f(p[0] + (cos(Theta) * Scale * Sign),
                p[1] + (sin(Theta) * Scale * Sign),
                p[2] - Scale * 2.0);
    // Left
    glNormal3f(-cos(Theta) * Sign, -sin(Theta) * Sign, 0.0);
    glVertex3f(p[0] - (cos(Theta) * Scale * Sign),
                p[1] - (sin(Theta) * Scale * Sign),
                p[2]);
    glVertex3f(p_n[0] - (cos(Theta_n) * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * Scale * Sign_n),
                p_n[2]);
    glVertex3f(p_n[0] - (cos(Theta_n) * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * Scale * Sign_n),
                p_n[2] - Scale * 2.0);
    glVertex3f(p[0] - (cos(Theta) * Scale * Sign),
                p[1] - (sin(Theta) * Scale * Sign),
                p[2] - Scale * 2.0);
    // Right
    glNormal3f(cos(Theta) * Sign, sin(Theta) * Sign, 0.0);
    glVertex3f(p[0] + (cos(Theta) * Scale * Sign),
                p[1] + (sin(Theta) * Scale * Sign),
                p[2] - Scale * 2.0);
    glVertex3f(p_n[0] + (cos(Theta_n) * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * Scale * Sign_n),
                p_n[2] - Scale * 2.0);
    glVertex3f(p_n[0] + (cos(Theta_n) * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * Scale * Sign_n),
                p_n[2]);
    glVertex3f(p[0] + (cos(Theta) * Scale * Sign),
                p[1] + (sin(Theta) * Scale * Sign),
                p[2]);
    glEnd();
// Right rail
    glBegin(GL_QUADS);
    glColor3f(0.75f, 0.75f, 0.75f);
    // Top
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(p[0] + (cos(Theta) * 7.0 * Scale * Sign),
                p[1] + (sin(Theta) * 7.0 * Scale * Sign),
                p[2] + 4.0 * Scale);
    glVertex3f(p_n[0] + (cos(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[2] + 4.0 * Scale);
    glVertex3f(p_n[0] + (cos(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[2] + 4.0 * Scale);
    glVertex3f(p[0] + (cos(Theta) * 5.0 * Scale * Sign),
                p[1] + (sin(Theta) * 5.0 * Scale * Sign),
                p[2] + 4.0 * Scale);
    // Bottom
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(p[0] + (cos(Theta) * 5.0 * Scale * Sign),
                p[1] + (sin(Theta) * 5.0 * Scale * Sign),
                p[2] + 2.0 * Scale);
    glVertex3f(p_n[0] + (cos(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[2] + 2.0 * Scale);
    glVertex3f(p_n[0] + (cos(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[2] + 2.0 * Scale);
    glVertex3f(p[0] + (cos(Theta) * 7.0 * Scale * Sign),
                p[1] + (sin(Theta) * 7.0 * Scale * Sign),
                p[2] + 2.0 * Scale);
    // Left
    glNormal3f(-cos(Theta) * Sign, -sin(Theta) * Sign, 0.0);
    glVertex3f(p[0] + (cos(Theta) * 5.0 * Scale * Sign),
                p[1] + (sin(Theta) * 5.0 * Scale * Sign),
                p[2] + 4.0 * Scale);
    glVertex3f(p_n[0] + (cos(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[2] + 4.0 * Scale);
    glVertex3f(p_n[0] + (cos(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[2] + 2.0 * Scale);
    glVertex3f(p[0] + (cos(Theta) * 5.0 * Scale * Sign),
                p[1] + (sin(Theta) * 5.0 * Scale * Sign),
                p[2] + 2.0 * Scale);
    // Right
    glNormal3f(cos(Theta) * Sign, sin(Theta) * Sign, 0.0);
    glVertex3f(p_n[0] + (cos(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[2] + 4.0 * Scale);
    glVertex3f(p[0] + (cos(Theta) * 7.0 * Scale * Sign),
                p[1] + (sin(Theta) * 7.0 * Scale * Sign),
                p[2] + 4.0 * Scale);
    glVertex3f(p[0] + (cos(Theta) * 7.0 * Scale * Sign),
                p[1] + (sin(Theta) * 7.0 * Scale * Sign),
                p[2] + 2.0 * Scale);
    glVertex3f(p_n[0] + (cos(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[1] + (sin(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[2] + 2.0 * Scale);
    glEnd();
// Left rail
    glBegin(GL_QUADS);
    glColor3f(0.75f, 0.75f, 0.75f);
    // Top
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(p[0] - (cos(Theta) * 5.0 * Scale * Sign),
                p[1] - (sin(Theta) * 5.0 * Scale * Sign),
                p[2] + 4.0 * Scale);
    glVertex3f(p_n[0] - (cos(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[2] + 4.0 * Scale);
    glVertex3f(p_n[0] - (cos(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[2] + 4.0 * Scale);
    glVertex3f(p[0] - (cos(Theta) * 7.0 * Scale * Sign),
                p[1] - (sin(Theta) * 7.0 * Scale * Sign),
                p[2] + 4.0 * Scale);
    // Bottom
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(p[0] - (cos(Theta) * 7.0 * Scale * Sign),
                p[1] - (sin(Theta) * 7.0 * Scale * Sign),
                p[2] + 2.0 * Scale);
    glVertex3f(p_n[0] - (cos(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[2] + 2.0 * Scale);
    glVertex3f(p_n[0] - (cos(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[2] + 2.0 * Scale);
    glVertex3f(p[0] - (cos(Theta) * 5.0 * Scale * Sign),
                p[1] - (sin(Theta) * 5.0 * Scale * Sign),
                p[2] + 2.0 * Scale);
    // Left
    glNormal3f(-cos(Theta) * Sign, -sin(Theta) * Sign, 0.0);
    glVertex3f(p[0] - (cos(Theta) * 7.0 * Scale * Sign),
                p[1] - (sin(Theta) * 7.0 * Scale * Sign),
                p[2] + 4.0 * Scale);
    glVertex3f(p_n[0] - (cos(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[2] + 4.0 * Scale);
    glVertex3f(p_n[0] - (cos(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * 7.0 * Scale * Sign_n),
                p_n[2] + 2.0 * Scale);
    glVertex3f(p[0] - (cos(Theta) * 7.0 * Scale * Sign),
                p[1] - (sin(Theta) * 7.0 * Scale * Sign),
                p[2] + 2.0 * Scale);
    // Right
    glNormal3f(cos(Theta) * Sign, sin(Theta) * Sign, 0.0);
    glVertex3f(p_n[0] - (cos(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[2] + 4.0 * Scale);
    glVertex3f(p[0] - (cos(Theta) * 5.0 * Scale * Sign),
                p[1] - (sin(Theta) * 5.0 * Scale * Sign),
                p[2] + 4.0 * Scale);
    glVertex3f(p[0] - (cos(Theta) * 5.0 * Scale * Sign),
                p[1] - (sin(Theta) * 5.0 * Scale * Sign),
                p[2] + 2.0 * Scale);
    glVertex3f(p_n[0] - (cos(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[1] - (sin(Theta_n) * 5.0 * Scale * Sign_n),
                p_n[2] + 2.0 * Scale);
    glEnd();
// Connecting ties
    if (i % 4 == 0) {
        glBegin(GL_QUADS);
        glColor3f(0.25, 0.25, 0.25);
// Left
        // Top
        glVertex3f(p[0] - (cos(Theta) * Scale * Sign),
                    p[1] - (sin(Theta) * Scale * Sign),
                    p[2] + Scale * 0.01);
        glVertex3f(p_n[0] - (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] - (sin(Theta_n) * Scale * Sign_n),
                    p_n[2] + Scale * 0.01);
        glVertex3f(p_n[0] - (cos(Theta_n) * 5.0 * Scale * Sign_n),
                    p_n[1] - (sin(Theta_n) * 5.0 * Scale * Sign_n),
                    p_n[2] + 2.0 * Scale);
        glVertex3f(p[0] - (cos(Theta) * 5.0 * Scale * Sign),
                    p[1] - (sin(Theta) * 5.0 * Scale * Sign),
                    p[2] + 2.0 * Scale);
        // Bottom
        glVertex3f(p[0] - (cos(Theta) * 5.0 * Scale * Sign),
                    p[1] - (sin(Theta) * 5.0 * Scale * Sign),
                    p[2] + 2.0 * Scale);
        glVertex3f(p_n[0] - (cos(Theta_n) * 5.0 * Scale * Sign_n),
                    p_n[1] - (sin(Theta_n) * 5.0 * Scale * Sign_n),
                    p_n[2] + 2.0 * Scale);
        glVertex3f(p_n[0] - (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] - (sin(Theta_n) * Scale * Sign_n),
                    p_n[2] + Scale * 0.01);
        glVertex3f(p[0] - (cos(Theta) * Scale * Sign),
                    p[1] - (sin(Theta) * Scale * Sign),
                    p[2] + Scale * 0.01);
// Right
        // Top
        glVertex3f(p[0] + (cos(Theta) * Scale * Sign),
                    p[1] + (sin(Theta) * Scale * Sign),
                    p[2] + Scale * 0.01);
        glVertex3f(p_n[0] + (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] + (sin(Theta_n) * Scale * Sign_n),
                    p_n[2] + Scale * 0.01);
        glVertex3f(p_n[0] + (cos(Theta_n) * 5.0 * Scale * Sign_n),
                    p_n[1] + (sin(Theta_n) * 5.0 * Scale * Sign_n),
                    p_n[2] + 2.0 * Scale);
        glVertex3f(p[0] + (cos(Theta) * 5.0 * Scale * Sign),
                    p[1] + (sin(Theta) * 5.0 * Scale * Sign),
                    p[2] + 2.0 * Scale);
        // Bottom
        glVertex3f(p[0] + (cos(Theta) * 5.0 * Scale * Sign),
                    p[1] + (sin(Theta) * 5.0 * Scale * Sign),
                    p[2] + 2.0 * Scale);
        glVertex3f(p_n[0] + (cos(Theta_n) * 5.0 * Scale * Sign_n),
                    p_n[1] + (sin(Theta_n) * 5.0 * Scale * Sign_n),
                    p_n[2] + 2.0 * Scale);
        glVertex3f(p_n[0] + (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] + (sin(Theta_n) * Scale * Sign_n),
                    p_n[2] + Scale * 0.01);
        glVertex3f(p[0] + (cos(Theta) * Scale * Sign),
                    p[1] + (sin(Theta) * Scale * Sign),
                    p[2] + Scale * 0.01);
        glEnd();
    }
// Pillars
    if (i != n_refined/4 && i % (n_refined/8) == 0) {
        glBegin(GL_QUADS);
        //glColor3f(0.375, 0.125, 0.0);  // Brown
        glColor3f(0.25, 0.5, 1.0);
        //*
        // Back
        glNormal3f(p[0] - p_n[0], p[1] - p_n[1], 0.0);
        glVertex3f(p[0] - (cos(Theta) * Scale * Sign),
                    p[1] - (sin(Theta) * Scale * Sign),
                    0.0);
        glVertex3f(p[0] + (cos(Theta) * Scale * Sign),
                    p[1] + (sin(Theta) * Scale * Sign),
                    0.0);
        glVertex3f(p[0] + (cos(Theta) * Scale * Sign),
                    p[1] + (sin(Theta) * Scale * Sign),
                    p[2] - Scale * 2.0);
        glVertex3f(p[0] - (cos(Theta) * Scale * Sign),
                    p[1] - (sin(Theta) * Scale * Sign),
                    p[2] - Scale * 2.0);
        // Front
        glNormal3f(p_n[0] - p[0], p_n[1] - p[1], 0.0);
        glVertex3f(p_n[0] + (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] + (sin(Theta_n) * Scale * Sign_n),
                    p_n[2] - Scale * 2.0);
        glVertex3f(p_n[0] + (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] + (sin(Theta_n) * Scale * Sign_n),
                    0.0);
        glVertex3f(p_n[0] - (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] - (sin(Theta_n) * Scale * Sign_n),
                    0.0);
        glVertex3f(p_n[0] - (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] - (sin(Theta_n) * Scale * Sign_n),
                    p_n[2] - Scale * 2.0);
        // Left
        glNormal3f(-cos(Theta) * Sign, -sin(Theta) * Sign, 0.0);
        glVertex3f(p[0] - (cos(Theta) * Scale * Sign),
                    p[1] - (sin(Theta) * Scale * Sign),
                    p[2] - Scale * 2.0);
        glVertex3f(p_n[0] - (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] - (sin(Theta_n) * Scale * Sign_n),
                    p_n[2] - Scale * 2.0);
        glVertex3f(p_n[0] - (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] - (sin(Theta_n) * Scale * Sign_n),
                    0.0);
        glVertex3f(p[0] - (cos(Theta) * Scale * Sign),
                    p[1] - (sin(Theta) * Scale * Sign),
                    0.0);
        // Right
        glNormal3f(cos(Theta) * Sign, sin(Theta) * Sign, 0.0);
        glVertex3f(p[0] + (cos(Theta) * Scale * Sign),
                    p[1] + (sin(Theta) * Scale * Sign),
                    0.0);
        glVertex3f(p_n[0] + (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] + (sin(Theta_n) * Scale * Sign_n),
                    0.0);
        glVertex3f(p_n[0] + (cos(Theta_n) * Scale * Sign_n),
                    p_n[1] + (sin(Theta_n) * Scale * Sign_n),
                    p_n[2] - Scale * 2.0);
        glVertex3f(p[0] + (cos(Theta) * Scale * Sign),
                    p[1] + (sin(Theta) * Scale * Sign),
                    p[2] - Scale * 2.0);
        glEnd();
    }
}

void Track::Cart_Build() {
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    // Top
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, 0.25f);
    glVertex3f(-0.5f, 0.5f, 0.25f);
    glVertex3f(-0.5f, -0.5f, 0.25f);
    glVertex3f(0.5f, -0.5f, 0.25f);
    // Bottom
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    // Front
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.75f);
    glVertex3f(0.5f, 0.5f, 0.75f);
    glVertex3f(0.5f, 0.5f, 0.0f);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.75f);
    glVertex3f(0.5f, -0.5f, 0.75f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    // Back
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.75f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.75f);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.75f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.75f);
    // Left
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.75f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.75f);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.75f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.75f);
    // Right
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.75f);
    glVertex3f(-0.5f, -0.5f, 0.75f);
    glVertex3f(-0.5f, -0.5f, 0.0f);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.75f);
    glVertex3f(0.5f, -0.5f, 0.75f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glEnd();
}

void Track::Head_Cart_Build() {
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    // Top
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, 0.25f);
    glVertex3f(-0.5f, 0.5f, 0.25f);
    glVertex3f(-0.5f, -0.5f, 0.25f);
    glVertex3f(0.5f, -0.5f, 0.25f);
    // Bottom
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    // Front
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.75f);
    glVertex3f(0.5f, 0.5f, 0.75f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    // Back
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.75f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.75f);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.75f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.75f);
    // Left
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.75f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.75f);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.75f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.75f);
    // Right
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.75f);
    glVertex3f(-0.5f, -0.5f, 0.75f);
    glVertex3f(-0.5f, -0.5f, 0.0f);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.75f);
    glVertex3f(0.5f, -0.5f, 0.75f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    // Front Ramp
    glColor3f(1.0, 0.0, 1.0);
    glNormal3f(0.5f, 0.0f, 0.5f);
    glVertex3f(1.0f, 0.5f, 0.25f);
    glVertex3f(0.5f, 0.5f, 0.75f);
    glVertex3f(0.5f, -0.5f, 0.75f);
    glVertex3f(1.0f, -0.5f, 0.25f);

    glNormal3f(0.25f, 0.0f, -0.75f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(1.0f, 0.5f, 0.25f);
    glVertex3f(1.0f, -0.5f, 0.25f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 0.5f, 0.25f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.75f);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.75f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(1.0f, -0.5f, 0.25f);
    glEnd();
}

// ---------------------------------------------------------

// Draw
void
Track::Draw(void)
{
    float   posn[3];
    float   tangent[3];
    double  angle;
    double  cart_dist;

    if ( ! initialized )
	return;

    for (int i = 0; i < CART_NUM; ++i) {
        glPushMatrix();

        // Draw the track
        glCallList(track_list);

        glPushMatrix();

        // Figure out where the train is
        cart_dist = (posn_on_track - (i * 0.04) < 0.0) ? -(4.0 - (i * 0.04)) : (i * 0.04);
        track->Evaluate_Point(posn_on_track - cart_dist, posn);

        // Translate the train to the point
        glTranslatef(posn[0], posn[1], posn[2]);

        // ...and what it's orientation is
        track->Evaluate_Derivative(posn_on_track - cart_dist, tangent);
        Normalize_3(tangent);

        // Rotate it to poitn along the track, but stay horizontal
        angle = atan2(tangent[1], tangent[0]) * 180.0 / M_PI;
        glRotatef((float)angle, 0.0f, 0.0f, 1.0f);

        // Another rotation to get the tilt right.
        angle = asin(-tangent[2]) * 180.0 / M_PI;
        glRotatef((float)angle, 0.0f, 1.0f, 0.0f);

        // Draw the train
        glCallList(train_list[i]);

        glPopMatrix();
        glPopMatrix();
    }
}


void
Track::Update(float dt)
{
    float   point[3];
    float   deriv[3];
    double  length;
    double  parametric_speed;

    if ( ! initialized )
	return;

    // First we move the train along the track with its current speed.

    // Get the derivative at the current location on the track.
    track->Evaluate_Derivative(posn_on_track, deriv);

    // Get its length.
    length = sqrt(deriv[0]*deriv[0] + deriv[1]*deriv[1] + deriv[2]*deriv[2]);
    if ( length == 0.0 )
	return;

    // The parametric speed is the world train speed divided by the length
    // of the tangent vector.
    parametric_speed = speed / length;

    // Now just evaluate dist = speed * time, for the parameter.
    posn_on_track += (float)(parametric_speed * dt);

    // If we've just gone around the track, reset back to the start.
    if ( posn_on_track > track->N() )
	posn_on_track -= track->N();

    // As the second step, we use conservation of energy to set the speed
    // for the next time.
    // The total energy = z * gravity + 1/2 speed * speed, assuming unit mass
    track->Evaluate_Point(posn_on_track, point);
    if ( TRAIN_ENERGY - 9.81 * point[2] < 0.0 )
	speed = 0.0;
    else
	speed = (float)sqrt(2.0 * ( TRAIN_ENERGY - 9.81 * point[2] ));
}


