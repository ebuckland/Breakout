//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 8

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    
    srand48(time(NULL));
    double veloX = drand48() * .05 * 2 - .05 + .01;
    double veloY = .02;
    
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        move(ball, veloX, veloY);
        
        if ((getX(ball) + RADIUS * 2) >= WIDTH) 
        {
            veloX = veloX * -1;
        }
        else if (getX(ball) <= 0) 
        {
            veloX = veloX * -1;
        }
        else if (getY(ball) <= 20) 
        {
            veloY = veloY * -1;
        }
        else if ((getY(ball) + RADIUS * 2) >= (HEIGHT - 20)) 
        {
            lives--;
            // reset position
            setLocation(ball,WIDTH / 2 - RADIUS, HEIGHT / 2 - RADIUS);
            veloX = drand48() * .05 * 2 - .05 + .01;
            veloY = .02;
        }
        
        GObject object = detectCollision(window, ball);
        
        if (object == paddle) 
        {
            veloY = veloY * -1;
        } 
        else if (object != NULL && strcmp(getType(object),"GRect") == 0) 
        {
            veloY = veloY * -1;
            removeGWindow(window,object);
            points++;
            bricks--;
            updateScoreboard(window,label,points);
        }
        
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if we heard one
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure circle follows top cursor
                setLocation(paddle, getX(event) - 30, HEIGHT - 70);
            }
        }
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    string colors[] = {"PINK","RED","ORANGE","YELLOW","GREEN","CYAN","BLUE","MAGENTA"};
    int x;
    int y = 20;
    for (int i = 0; i < COLS; i++) 
    {
        x = 20;
        for (int j = 0; j < ROWS; j++) 
        {
            GRect brick = newGRect(x, y, 69, 16);
            setColor(brick,colors[i]);
            setFilled(brick, true);
            add(window, brick);
            x += 72;
        }
        y += 19;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(WIDTH / 2 - RADIUS, HEIGHT / 2 - RADIUS, RADIUS * 2, RADIUS * 2);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);
    
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(WIDTH / 2, HEIGHT - 70, 60, 15);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);
    
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel score = newGLabel("0");
    setLocation(score, WIDTH / 2, HEIGHT / 2);
    setColor(score, "GRAY");
    setFilled(score, true);
    setFont(score,"Calibri-40");
    sendToBack(score);
    add(window, score);
    return score;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
