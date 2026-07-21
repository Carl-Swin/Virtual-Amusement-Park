#include "FerrisWheel.h"
#include <stdio.h>
#include <FL/math.h>

// Normalize a 3d vector.
static void
Normalize_3(float v[3])
{
    double  l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    if (l == 0.0)
        return;

    v[0] /= (float)l;
    v[1] /= (float)l;
    v[2] /= (float)l;
}


// Destructor
Ferris_Wheel::~Ferris_Wheel(void)
{
    if (initialized) {
        glDeleteLists(stand_list, 1);
    }
}


// Initializer. Would return false if anything could go wrong.
bool
Ferris_Wheel::Initialize(void)
{
    stand_list = glGenLists(1);
    glNewList(stand_list, GL_COMPILE);
    glColor3f(0.25, 0.25, 0.25);

    Build_Stand();

    glEndList();

    initialized = true;

    return true;
}

//* Additions -----------------------------------------------

int Ferris_Wheel::Increment_Precision() {
    return ++wheel_precision;
}
int Ferris_Wheel::Decrement_Precision() {
    return (wheel_precision == 1) ? 1 : --wheel_precision;
}

int Ferris_Wheel::Increment_Fractal() {
    return ++fractal_cap;
}
int Ferris_Wheel::Decrement_Fractal() {
    return (fractal_cap == 1) ? 1 : --fractal_cap;
}

void Ferris_Wheel::Set_Coords(float& x_at, float& y_at, float& z_at) {
    x_at = -39.0;
    y_at = cos(rotation * M_PI / 180.0) * WHEEL_RADIUS;
    z_at = sin(rotation * M_PI / 180.0) * WHEEL_RADIUS + WHEEL_RADIUS + WHEEL_HEIGHT - 1.0;
    return;
}

void Ferris_Wheel::Build_Leg() {
    glBegin(GL_QUADS);

    glNormal3f(0.0f, -1.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-10.0f, 0.0f, 1.0f);
    glVertex3f(-10.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, -1.0f, 0.0f);

    glNormal3f(0.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-10.0f, 1.0f, 0.0f);
    glVertex3f(-10.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-10.0f, 0.0f, 1.0f);
    glVertex3f(-10.0f, 1.0f, 0.0f);
    glVertex3f(-10.0f, 0.0f, 0.0f);
    glVertex3f(-10.0f, -1.0f, 0.0f);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);

    glEnd();
}

void Ferris_Wheel::Build_Stand() {
    glColor3f(0.25, 0.25, 0.25);

    glTranslatef(0.0, 10.0, 0.0);
    Build_Leg();

    glTranslatef(0.0, -20.0, 0.0);
    Build_Leg();

    glTranslatef(0.0, 10.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
// Left
    // Top
    glNormal3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-8.0f, -1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-10.0f, -1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-10.0f, -10.0f, 1.0f);
    glVertex3f(-8.0f, -10.0f, 1.0f);
    // Bottom
    glNormal3f(1.0f, 0.0f, -1.0f);
    glVertex3f(-8.0f, -9.0f, 0.0f);
    glVertex3f(-10.0f, -9.0f, 0.0f);
    glVertex3f(-10.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    //Front
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-10.0f, -9.0f, 0.0f);
    glVertex3f(-10.0f, -10.0f, 1.0f);
    glVertex3f(-10.0f, -1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-10.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    // Back
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    glVertex3f(-8.0f, -1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-8.0f, -10.0f, 1.0f);
    glVertex3f(-8.0f, -9.0f, 0.0f);
// Right
    // Top
    glNormal3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-8.0f, 10.0f, 1.0f);
    glVertex3f(-10.0f, 10.0f, 1.0f);
    glVertex3f(-10.0f, 1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-8.0f, 1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    // Bottom
    glNormal3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    glVertex3f(-10.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    glVertex3f(-10.0f, 9.0f, 0.0f);
    glVertex3f(-8.0f, 9.0f, 0.0f);
    //Front
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-10.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    glVertex3f(-10.0f, 1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-10.0f, 10.0f, 1.0f);
    glVertex3f(-10.0f, 9.0f, 0.0f);
    // Back
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-8.0f, 9.0f, 0.0f);
    glVertex3f(-8.0f, 10.0f, 1.0f);
    glVertex3f(-8.0f, 1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    // Top
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-8.0f, 9.0f, 0.0f);
    glVertex3f(-8.0f, 10.0f, 1.0f);
    glVertex3f(-8.0f, 1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
// Head
    // Back
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-10.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    glVertex3f(-10.0f, -1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-10.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT + 1.0);
    glVertex3f(-10.0f, 1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    // Left
    glNormal3f(0.0f, -1.0f, 1.0f);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT + 1.0);
    glVertex3f(-10.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT + 1.0);
    glVertex3f(-10.0f, -1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-8.0f, -1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    // Right
    glNormal3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-8.0f, 1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-10.0f, 1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-10.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT + 1.0);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT + 1.0);
    glEnd();
// Spike
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_TRIANGLES);
    // Top Left
    glNormal3f(-1.0f, 0.0f, 1.0f);
    glVertex3f(-8.0f, -1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(0.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT + 1.0);
    // Bottom Left
    glNormal3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    glVertex3f(0.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-8.0f, -1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    // Top Right
    glNormal3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT + 1.0);
    glVertex3f(0.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-8.0f, 1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    // Bottom Left
    glNormal3f(1.0f, 0.0f, -1.0f);
    glVertex3f(-8.0f, 1.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(0.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT);
    glVertex3f(-8.0f, 0.0f, WHEEL_RADIUS + WHEEL_HEIGHT - 1.0);
    glEnd();
}

void Ferris_Wheel::Build_Wheel() {
    for (int i = 0; i < wheel_precision; ++i) {
        for (int j = 0; j < 4; ++j) {
            glColor3f(1.0, 0.5, 0.25);
            glBegin(GL_TRIANGLES);

            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, cos((j * M_PI / 2.0) + ((M_PI * i) / (2.0 * wheel_precision))) * WHEEL_RADIUS, sin((j * M_PI / 2.0) + ((M_PI * i) / (2.0 * wheel_precision))) * WHEEL_RADIUS);
            glVertex3f(0.0f, cos((j * M_PI / 2.0) + ((M_PI * (i + 1)) / (2.0 * wheel_precision))) * WHEEL_RADIUS, sin((j * M_PI / 2.0) + ((M_PI * (i + 1)) / (2.0 * wheel_precision))) * WHEEL_RADIUS);

            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, cos((j * M_PI / 2.0) + ((M_PI * (i + 1)) / (2.0 * wheel_precision))) * WHEEL_RADIUS, sin((j * M_PI / 2.0) + ((M_PI * (i + 1)) / (2.0 * wheel_precision))) * WHEEL_RADIUS);
            glVertex3f(0.0f, cos((j * M_PI / 2.0) + ((M_PI * i) / (2.0 * wheel_precision))) * WHEEL_RADIUS, sin((j * M_PI / 2.0) + ((M_PI * i) / (2.0 * wheel_precision))) * WHEEL_RADIUS);

            glEnd();


            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_LINE_STRIP);

            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, cos((j * M_PI / 2.0) + ((M_PI * (i + 1)) / (2.0 * wheel_precision))) * WHEEL_RADIUS, sin((j * M_PI / 2.0) + ((M_PI * (i + 1)) / (2.0 * wheel_precision))) * WHEEL_RADIUS);

            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, cos((j * M_PI / 2.0) + ((M_PI * i) / (2.0 * wheel_precision))) * WHEEL_RADIUS, sin((j * M_PI / 2.0) + ((M_PI * i) / (2.0 * wheel_precision))) * WHEEL_RADIUS);

            glEnd();
        }
    }
}

void Ferris_Wheel::Build_Pod() {
    glBegin(GL_QUADS);
    glColor3f(0.375, 0.125, 0.0);
    //Top Left
    glNormal3f(0.0f, -0.25f, 1.0f);
    glVertex3f(0.75f, 0.0f, 0.25f);
    glVertex3f(-0.75f, 0.0f, 0.25f);
    glVertex3f(-0.75f, -0.75f, 0.0f);
    glVertex3f(0.75f, -0.75f, 0.0f);

    glNormal3f(0.0f, -0.25f, -1.0f);
    glVertex3f(0.75f, -0.75f, 0.0f);
    glVertex3f(-0.75f, -0.75f, 0.0f);
    glVertex3f(-0.75f, 0.0f, 0.25f);
    glVertex3f(0.75f, 0.0f, 0.25f);
    // Top Right
    glNormal3f(0.0f, 0.25f, 1.0f);
    glVertex3f(0.75f, 0.75f, 0.0f);
    glVertex3f(-0.75f, 0.75f, 0.0f);
    glVertex3f(-0.75f, 0.0f, 0.25f);
    glVertex3f(0.75f, 0.0f, 0.25f);

    glNormal3f(0.0f, 0.25f, -1.0f);
    glVertex3f(0.75f, 0.0f, 0.25f);
    glVertex3f(-0.75f, 0.0f, 0.25f);
    glVertex3f(-0.75f, 0.75f, 0.0f);
    glVertex3f(0.75f, 0.75f, 0.0f);
    // Bottom
    glColor3f(1.0, 1.0, 1.0);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.75f, 0.75f, -3.0f);
    glVertex3f(-0.75f, 0.75f, -3.0f);
    glVertex3f(-0.75f, -0.75f, -3.0f);
    glVertex3f(0.75f, -0.75f, -3.0f);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(0.75f, -0.75f, -3.0f);
    glVertex3f(-0.75f, -0.75f, -3.0f);
    glVertex3f(-0.75f, 0.75f, -3.0f);
    glVertex3f(0.75f, 0.75f, -3.0f);
    // Left
    glColor3f(0.375, 0.125, 0.0);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.75f, -0.75f, -3.0f);
    glVertex3f(0.75f, -0.75f, -3.0f);
    glVertex3f(0.75f, -0.75f, -2.0f);
    glVertex3f(-0.75f, -0.75f, -2.0f);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.75f, -0.75f, -2.0f);
    glVertex3f(0.75f, -0.75f, -2.0f);
    glVertex3f(0.75f, -0.75f, -3.0f);
    glVertex3f(-0.75f, -0.75f, -3.0f);
    // Right
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.75f, 0.75f, -2.0f);
    glVertex3f(0.75f, 0.75f, -2.0f);
    glVertex3f(0.75f, 0.75f, -3.0f);
    glVertex3f(-0.75f, 0.75f, -3.0f);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.75f, 0.75f, -3.0f);
    glVertex3f(0.75f, 0.75f, -3.0f);
    glVertex3f(0.75f, 0.75f, -2.0f);
    glVertex3f(-0.75f, 0.75f, -2.0f);
    // Front
    glColor3f(1.0, 1.0, 1.0);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.75f, -0.75f, -2.0f);
    glVertex3f(0.75f, 0.75f, -2.0f);
    glVertex3f(0.75f, 0.75f, -3.0f);
    glVertex3f(0.75f, -0.75f, -3.0f);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.75f, -0.75f, -3.0f);
    glVertex3f(0.75f, 0.75f, -3.0f);
    glVertex3f(0.75f, 0.75f, -2.0f);
    glVertex3f(0.75f, -0.75f, -2.0f);
    // Back
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.75f, -0.75f, -2.0f);
    glVertex3f(-0.75f, 0.75f, -2.0f);
    glVertex3f(-0.75f, 0.75f, -3.0f);
    glVertex3f(-0.75f, -0.75f, -3.0f);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.75f, -0.75f, -3.0f);
    glVertex3f(-0.75f, 0.75f, -3.0f);
    glVertex3f(-0.75f, 0.75f, -2.0f);
    glVertex3f(-0.75f, -0.75f, -2.0f);
    glEnd();
}

void Ferris_Wheel::Recursive_Wheel(unsigned long Index, double Rotate) {
    if (Index < 1) Index = 1;

    // Try to do 7.0/4.0 (1.75) or 5.0/3.0 (1.66667) to prevent crashing pods
    //double prev_scale = (Index - 1) * (Index - 1);
    double power = 97.0 / 60.0;
    double factorial = tgamma(Index + 1);

    double scale = pow(Index, power);
    double next_scale = pow(Index + 1, power);

    glScaled(1.0/scale, 1.0/scale, 1.0/scale);


    glPushMatrix();
        glTranslatef(-40.0 * pow(factorial, power), 0.0, WHEEL_RADIUS + (WHEEL_HEIGHT * pow(factorial, power)));
        glRotatef(Rotate, 1.0, 0.0, 0.0);

        Build_Wheel();
    glPopMatrix();


    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
            glTranslatef(0.0, 0.0, WHEEL_RADIUS);
            if (fractal_cap > 1) (fractal_cap == Index) ? glTranslatef(-40.0 * pow(factorial, power), 0.0, (WHEEL_HEIGHT * pow(factorial, power))) : glTranslatef(0.0, 0.0, -(WHEEL_RADIUS / next_scale));
            glRotatef(Rotate + (360 * i / 8.0), 1.0, 0.0, 0.0);
            glTranslatef(0.0, 0.0, -WHEEL_RADIUS);
            glRotatef(-Rotate - (360 * i / 8.0), 1.0, 0.0, 0.0);


            if (Index == 1) {
                if (fractal_cap > 1) glTranslatef(0.0, 0.0, (WHEEL_RADIUS / next_scale));
                glTranslatef(-39.0 * pow(factorial, power), 0.0, (WHEEL_HEIGHT * pow(factorial, power)));

                Build_Pod();

                if (fractal_cap > 1) glTranslatef(0.0, 0.0, -(WHEEL_RADIUS / next_scale));
                glTranslatef(39.0 * pow(factorial, power), 0.0, -(WHEEL_HEIGHT * pow(factorial, power)));
            }

            if (1 < fractal_cap) (Index < fractal_cap) ? Recursive_Wheel(Index + 1, Rotate * -2.5) : Build_Pod();
        glPopMatrix();
    }
}

// --------------------------------------------------------- */

// Draw
void
Ferris_Wheel::Draw(void)
{
    float   posn[3] = { 0.0f };
    float   tangent[3] = { 0.0f };
    double  angle = 0.0;

    if (!initialized)
        return;


    glPushMatrix();

    glTranslatef(-35.0, 0.0, 0.0);
    glColor3f(0.25, 0.25, 0.25);
    glCallList(stand_list);
    //Build_Stand();

    glPopMatrix();


    Recursive_Wheel(1, rotation);
}


void
Ferris_Wheel::Update(float dt)
{
    float   point[3];
    float   deriv[3];
    double  length;
    double  parametric_speed;

    if (!initialized)
        return;

    rotation += (rotation < 360) ? (speed * dt) : -360.0 + (speed * dt);
}


