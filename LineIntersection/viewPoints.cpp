/* view.c
 
 Laura Toma, Ivy Xing, Zackery Leman
 
 What it does:
 
 Determines (1) whether there is an intersection
            (2) If so, whether it's proper or improper
 
 */
#include <set>
#include <vector>
#include "geom.h"
#include "rtimer.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat orange[3] = {1.0, 0.5, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};

GLint fillmode = 0;

/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void create_test_cases();
void create_answer_keys();
void print_result(int test_case_index);

/* global variables */
const int WINDOWSIZE = 500;
const int NUM_TEST_CASES = 6;

//NOTE: all the structures below need to be global so that they can be rendered

// Test cases and answer keys
vector<segment2D> segments_for_test_cases;
vector<int> key_intersection;
vector<int> key_proper_intersection;
vector<int> key_improper_intersection;

/* ****************************** */
int main(int argc, char** argv) {
    
    /* initialize GLUT  */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);
    
    /* Initialize test cases and answer keys */
    create_test_cases();
    create_answer_keys();
    
    /* Print out the testing results for each test case */
    for (int i = 0; i < NUM_TEST_CASES; i++) {
        print_result(i);
    }
    
    /* register callback functions */
    glutDisplayFunc(display);
    glutKeyboardFunc(keypress);
    
    /* init GL */
    /* set background color black*/
    glClearColor(0.3,0.3, 0.3, 0.3);
    /* here we can enable depth testing and double buffering and so
     on */
    
    /* give control to event handler */
    glutMainLoop();
    
    return 0;
}

// Helper method to make a line segment given 2 coordinates-pairs
segment2D make_segment(int x1, int y1, int x2, int y2) {
    point2D start, end;
    start.x = x1;
    start.y = y1;
    end.x = x2;
    end.y = y2;
    
    segment2D line;
    line.start = start;
    line.end = end;
    
    return line;
}

// Initializes all test cases
void create_test_cases() {
    // Upper half or lower half of canvas
    int y_upper = WINDOWSIZE / 2;
    int y_lower = 0;
    // Left, middle, or right side of canvas
    int x_left = 0;
    int x_mid = WINDOWSIZE / 3;
    int x_right = WINDOWSIZE / 3 * 2;
    
    // Test case 1 (Upper left)
    segment2D l1_a = make_segment(x_left + 30, y_upper + 80, x_left + 80, y_upper + 90);
    segment2D l1_b = make_segment(x_left + 60, y_upper + 70, x_left + 30, y_upper + 90);
    segments_for_test_cases.push_back(l1_a);
    segments_for_test_cases.push_back(l1_b);
    
    // Test case 2 (Upper middle)
    segment2D l2_a = make_segment(x_mid + 30, y_upper + 80, x_mid + 30, y_upper + 150);
    segment2D l2_b = make_segment(x_mid + 30, y_upper + 100, x_mid + 80, y_upper + 90);
    segments_for_test_cases.push_back(l2_a);
    segments_for_test_cases.push_back(l2_b);
    
    // Test case 3 (Upper right)
    segment2D l3_a = make_segment(x_right + 30, y_upper + 100, x_right + 80, y_upper + 90);
    segment2D l3_b = make_segment(x_right + 60, y_upper + 40,  x_right + 30, y_upper + 50);
    segments_for_test_cases.push_back(l3_a);
    segments_for_test_cases.push_back(l3_b);
    
    // Test case 4 (Lower left)
    segment2D l4_a = make_segment(x_left + 40, y_lower + 80,  x_left + 90, y_lower + 200);
    segment2D l4_b = make_segment(x_left + 60, y_lower + 100, x_left + 50, y_lower + 150);
    segments_for_test_cases.push_back(l4_a);
    segments_for_test_cases.push_back(l4_b);
    
    // Test case 5 (Lower middle)
    segment2D l5_a = make_segment(x_mid + 30, y_lower + 90, x_mid + 100, y_lower + 90);
    segment2D l5_b = make_segment(x_mid + 60, y_lower + 90, x_mid + 30, y_lower + 200);
    segments_for_test_cases.push_back(l5_a);
    segments_for_test_cases.push_back(l5_b);
    
    // Test case 6 (Lower right)
    segment2D l6_a = make_segment(x_right + 30, y_lower + 80, x_right + 80, y_lower + 150);
    segment2D l6_b = make_segment(x_right + 60, y_lower + 80, x_right + 30, y_lower + 50);
    segments_for_test_cases.push_back(l6_a);
    segments_for_test_cases.push_back(l6_b);
}

void create_answer_keys() {
    key_intersection = {1, 1, 0, 1, 1, 0};
    key_proper_intersection = {1, 0, 0, 1, 0, 0};
    key_improper_intersection = {0, 1, 0, 0, 1, 0};
}

/* Prints out the result given the test case index */
void print_result(int test_case_index) {
    // Test case index starts from 0
    int test_case_num = test_case_index + 1;
    // Two segments for each test case
    int first_segment_index = test_case_index * 2;
    int second_segment_index = first_segment_index + 1;
    
    if (segments_for_test_cases.size() > second_segment_index) {
        printf("TEST CASE #%d:\n", test_case_num);
        
        segment2D a = segments_for_test_cases[first_segment_index];
        segment2D b = segments_for_test_cases[second_segment_index];
        
        int inter = intersect(a, b);
        int proper_inter = intersect_proper(a, b);
        int improper_inter = intersect_improper(a, b);
        
        // Print line endpoint coordinates
        printf("Line a - x1: %d, y1: %d, x2: %d, y2: %d\n", a.start.x, a.start.y, a.end.x, a.end.y);
        printf("Line b - x1: %d, y1: %d, x2: %d, y2: %d\n\n", b.start.x, b.start.y, b.end.x, b.end.y);
        // Print intersection answer key and result
        printf("Intersection - answer key: %d, actual result: %d.\n",
               key_intersection[test_case_index], inter);
        printf("Proper Intersection - answer key: %d, actual result: %d.\n",
               key_proper_intersection[test_case_index], proper_inter);
        printf("Improper Intersection - answer key: %d, actual result: %d.\n\n",
               key_improper_intersection[test_case_index], improper_inter);
    } else {
        printf("Segment index out of bounds for test case index: %d.", test_case_index);
    }
}

// Draw a segment with current color
void draw_segment(segment2D s) {
    glBegin(GL_LINES);
    glVertex2f(s.start.x, s.start.y);
    glVertex2f(s.end.x, s.end.y);
    glEnd();
}

/* ****************************** */
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //clear the matrix
    
    /* The default GL window is [-1,1]x[-1,1] with the origin in the
     center.
     
     The points are in the range (0,0) to (WINSIZE,WINSIZE), so they
     need to be mapped to [-1,1]x [-1,1]x */
    glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);
    glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0);
    
    /* Draw test cases */
    for (int i = 0; i < segments_for_test_cases.size(); i++) {
        // Use different colors
        if (key_proper_intersection[i / 2] == 1) {
            glColor3fv(yellow);
        } else if (key_improper_intersection[i / 2] == 1) {
            glColor3fv(green);
        } else {
            glColor3fv(red);
        }
        
        draw_segment(segments_for_test_cases[i]);
    }

    /* execute the drawing commands */
    glFlush();
}

/* ****************************** */
void keypress(unsigned char key, int x, int y) {
    switch(key) {
        case 'q':
            exit(0);
            break;
        case 'i':
            create_test_cases();
            create_answer_keys();
            glutPostRedisplay();
            break;
    }
}

/* Handler for window re-size event. Called back when the window first appears and
 whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset
    gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height);
}
