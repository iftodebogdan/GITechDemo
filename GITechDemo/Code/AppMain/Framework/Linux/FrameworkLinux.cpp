/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   FrameworkLinux.cpp
 *      Author: Bogdan Iftode
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "stdafx.h"

#include "FrameworkLinux.h"
using namespace AppFramework;

#include <GL/glut.h>

/*
** Function called to update rendering
*/
void DisplayFunc(void)
{
    static float alpha = 0;

    /* Clear the buffer, clear the matrix */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* A step backward, then spin the cube */
    glTranslatef(0, 0, -10);
    glRotatef(30, 1, 0, 0);
    glRotatef(alpha, 0, 1, 0);

    /* We tell we want to draw quads */
    glBegin(GL_QUADS);

    /* Every four calls to glVertex, a quad is drawn */
    glColor3f(0, 0, 0); glVertex3f(-1, -1, -1);
    glColor3f(0, 0, 1); glVertex3f(-1, -1, 1);
    glColor3f(0, 1, 1); glVertex3f(-1, 1, 1);
    glColor3f(0, 1, 0); glVertex3f(-1, 1, -1);

    glColor3f(1, 0, 0); glVertex3f(1, -1, -1);
    glColor3f(1, 0, 1); glVertex3f(1, -1, 1);
    glColor3f(1, 1, 1); glVertex3f(1, 1, 1);
    glColor3f(1, 1, 0); glVertex3f(1, 1, -1);

    glColor3f(0, 0, 0); glVertex3f(-1, -1, -1);
    glColor3f(0, 0, 1); glVertex3f(-1, -1, 1);
    glColor3f(1, 0, 1); glVertex3f(1, -1, 1);
    glColor3f(1, 0, 0); glVertex3f(1, -1, -1);

    glColor3f(0, 1, 0); glVertex3f(-1, 1, -1);
    glColor3f(0, 1, 1); glVertex3f(-1, 1, 1);
    glColor3f(1, 1, 1); glVertex3f(1, 1, 1);
    glColor3f(1, 1, 0); glVertex3f(1, 1, -1);

    glColor3f(0, 0, 0); glVertex3f(-1, -1, -1);
    glColor3f(0, 1, 0); glVertex3f(-1, 1, -1);
    glColor3f(1, 1, 0); glVertex3f(1, 1, -1);
    glColor3f(1, 0, 0); glVertex3f(1, -1, -1);

    glColor3f(0, 0, 1); glVertex3f(-1, -1, 1);
    glColor3f(0, 1, 1); glVertex3f(-1, 1, 1);
    glColor3f(1, 1, 1); glVertex3f(1, 1, 1);
    glColor3f(1, 0, 1); glVertex3f(1, -1, 1);

    /* No more quads */
    glEnd();

    /* Rotate a bit more */
    alpha = alpha + 0.1f;

    /* End */
    glFlush();
    glutSwapBuffers();

    /* Update again and again */
    glutPostRedisplay();
}

/*
** Function called when the window is created or resized
*/
void ReshapeFunc(int width, int height)
{
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluPerspective(20.f, (float)width / (float)height, 5.f, 15.f);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
}

/*
** Function called when a key is hit
*/
void KeyboardFunc(unsigned char key, int x, int y)
{
    if ('q' == key || 'Q' == key || 27 == key)
        exit(0);
}

int FrameworkLinux::Run()
{
    int argc = 0;
    char argv[] = "";

    /* Creation of the window */
    glutInit(&argc, (char**)&argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("GITechDemo");

    /* OpenGL settings */
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);

    /* Declaration of the callbacks */
    glutDisplayFunc(&DisplayFunc);
    glutReshapeFunc(&ReshapeFunc);
    glutKeyboardFunc(&KeyboardFunc);

    /* Loop */
    glutMainLoop();
    
    return 0;
}