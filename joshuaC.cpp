/* Author: Joshua Cancellier
 *
 * Start Date: February 24, 2018
 *
 * Purpose: handle character input, movement, and animations
 *
 */

#include <X11/keysym.h>
#include "Global.h"
#include "Timers.h"
#include "SpriteSheet.h"
#include "Character.h"
#include <GL/glx.h>
#include "joshuaC.h"
#include "fonts.h"
#include "HitBox.h"
#include <sstream>
extern float cx;
extern Timers timers;
extern Global g;
extern int keys[];
extern Character rambo;
extern SpriteSheet img[];
extern bool display_hitbox;

//Constructors
Character::Character(int ssIdx)
{
    centerX = 100;
    centerY = 800;
    height = .08 * (float)g.yres;
    width = height * 0.7;
    printf("%f\n", height);
    frame = 0;
    flipped = false;
    jumping = false;
    shooting = false;
    prone = false;
    aimUp = false;
    angleUp = false;
    angleDown = false;
    shootingStraight = false;
    health = 4;
    spriteSheetIndex = ssIdx;
    velocityX = 0;
    velocityY = 0;
    hitBox = new HitBox(centerY+(height/2),centerY-(height/2),centerX-(width/2),centerX+(height/2));
    boundingBox = new HitBox(centerY+(height/2),centerY-(height/2),centerX-(width/2),centerX+(height/2));
}

Character::Character()
{
    centerX = 100;
    centerY = 800;
    height = .08 * (float)g.yres;
    width = height * 0.7;
    printf("%f\n", height);
    frame = 0;
    flipped = false;
    jumping = false;
    shooting = false;
    prone = false;
    aimUp = false;
    angleUp = false;
    angleDown = false;
    shootingStraight = false;
    health = 4;
    spriteSheetIndex = 0;
    velocityX = 0;
    velocityY = 0;
    hitBox = new HitBox(centerY+(height/2),centerY-(height/2),centerX-(width/2),centerX+(height/2));
    boundingBox = new HitBox(centerY+(height/2),centerY-(height/2),centerX-(width/2),centerX+(height/2));
}

HitBox::HitBox(int top, int bottom, int left, int right)
{
    this->top = top;
    this->bottom = bottom;
    this->left = left;
    this->right = right;
}

void HitBox::updateHitBox(int top, int bottom, int left, int right)
{
    this->top = top;
    this->bottom = bottom;
    this->left = left;
    this->right = right;
}

int HitBox::getLeft() const {
    return(this->left);
}

int HitBox::getRight() const {
    return(this->right);
}

int HitBox::getTop() const {
    return(this->top);
}

int HitBox::getBottom() const {
    return(this->bottom);
}

void HitBox::draw()
{
    glBegin(GL_LINE_LOOP);

	glColor3f(1.0, 1.0, 1.0);
	glVertex2i(left, bottom);
	glVertex2i(left, top);
    glColor3f(1.0, 1.0, 1.0);
	glVertex2i(right, top);
	glVertex2i(right, bottom);

	glEnd();
}

void HitBox::draw(float r, float g, float b)
{
    glBegin(GL_LINE_LOOP);

	glColor3f(r, g, b);
	glVertex2i(left, bottom);
	glVertex2i(left, top);
    glColor3f(r, g, b);
	glVertex2i(right, top);
	glVertex2i(right, bottom);

	glEnd();
}

//Accessors
float Character::getCenterX() const{ return centerX; }
float Character::getCenterY() const{ return centerY; }
float Character::getHeight() const{ return height; }
float Character::getWidth() const{ return width; }
int Character::getFrame() const{ return frame; }
bool Character::isFlipped() const{ return flipped; }
bool Character::isJumping() const{ return jumping; }
float Character::getHealth() const{ return health; }
int Character::getSpriteSheetIndex() const{ return spriteSheetIndex; }
float Character::getVelocityX() const{ return velocityX; }
float Character::getVelocityY() const { return velocityY; }

//Mutators
void Character::setCenterX(float x){ centerX = x; }
void Character::setCenterY(float y){ centerY = y; }
void Character::setHeight(float h){ height = h; }
void Character::setWidth(float w){ width = w; }
void Character::setFrame(int f){ frame = f; }
void Character::setFlipped(bool f){ flipped = f; }
void Character::setJumping(bool j){ jumping = j; }
void Character::setHealth(float h){health = h; }
void Character::setSpriteSheetIndex(int ssIdx){ spriteSheetIndex = ssIdx; }
void Character::setVelocityX(float vx) { velocityX = vx; }
void Character::setVelocityY(float vy) { velocityY = vy; }

//Character Member functions
void Character::draw()
{
    #ifdef PROFILING
    ///////////////////////////Timer///////////////////
    static double timeDifference = 0.0;
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    //////////////////////////////////////////////////
    #endif

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.walkTexture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);
    
    float ssWidth = (float)1.0/img[spriteSheetIndex].columns;
    float ssHeight = (float)1.0/img[spriteSheetIndex].rows;
    
    int ix = this->frame % img[spriteSheetIndex].columns;
    int iy = 0;
    
    //move to next row of spriteSheet (if available)
    if (frame >= img[spriteSheetIndex].columns) {
        iy = 1;
    }

    if (frame >= (img[spriteSheetIndex].columns*2)) {
        iy = 2;
    }

    if (frame >= (img[spriteSheetIndex].columns*3)) {
        iy = 3;
    }
    
    float textureX = (float)ix / img[spriteSheetIndex].columns;
    float textureY = (float)iy / img[spriteSheetIndex].rows;
    
    glBegin(GL_QUADS);
    glTexCoord2f(textureX, textureY+ssHeight);
    glVertex2i(flipped ? centerX+width : centerX-width, centerY-height);
    
    glTexCoord2f(textureX, textureY);
    glVertex2i(flipped ? centerX+width : centerX-width, centerY+height);
    
    glTexCoord2f(textureX+ssWidth, textureY);
    glVertex2i(flipped ? centerX-width : centerX+width, centerY+height);
    
    glTexCoord2f(textureX+ssWidth, textureY+ssHeight);
    glVertex2i(flipped ? centerX-width : centerX+width, centerY-height);
    glEnd();
    
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);

    if (display_hitbox) {
        hitBox->draw();
        boundingBox->draw(0.0, 0.0, 0.0);
    }
    update();
    
    #ifdef PROFILING
    //////////////////TIMER/////////////////////////////
    for (int i = 0; i < 1000000; i++) {
        int a = 2 / 4;
        a *= 2;
    }

    clock_gettime(CLOCK_REALTIME, &end);
    timeDifference += timers.timeDiff(&start, &end);


    //print time
    Rect r;
	r.bot = g.yres - 200;
	r.left = 10;
	r.center = 0;
    ggprint8b(&r, 16, 0x00ff0000, "Joshua Cancellier");
	ggprint8b(&r, 16, 0x00000000, "Character::draw(): %lf", timeDifference);
    ///////////////////////////////////////////////////
    #endif
}

//Implements updating of any child components such as the hitbox
void Character::update()
{
    /* **************Hitbox updating***************** */
    if (jumping) {
        if (flipped) {
            hitBox->updateHitBox(centerY+(height/2)-(height*.12),
                centerY-(height/2)+(height*.07),
                centerX-(width/2),
                centerX+(height/2)-(height*.16));
        } else {
            hitBox->updateHitBox(centerY+(height/2)-(height*.12),
                centerY-(height/2)+(height*.07),
                centerX-(width/2),
                centerX+(height/2)-(height*.16));
        }
    } else if (flipped) {
        if (prone) {
            hitBox->updateHitBox(centerY - (height*.45),
                                 centerY - (height*1),
                                 centerX - (height*.5),      //13
                                 centerX + (height / 2) - (height * .2080));  //12
        }
        else { //standing
            hitBox->updateHitBox(centerY+(height/2),
                                centerY-(height/2)-(height*.486111), //28
                                centerX-(width/2)+(height*.08), //13
                                centerX+(height/2)-(height*.2080)); //12
        }
    } else {
        if (prone) {
            hitBox->updateHitBox(centerY - (height*.45),
                                 centerY - (height*1), 
                                 centerX - (height*.5),  //5
                                 centerX + (height / 2) - (height * .2));     //20
        } else { //standing
            hitBox->updateHitBox(centerY+(height/2),
                                centerY-(height/2)-(height*.486111), //28
                                centerX-(width/2)+(height*.086805), //5
                                centerX+(height/2)-(height*.2)); //20
        }
    }
    /* **************End Hitbox Updating************** */

    /* **************Bounding box Updating************ */
    if (jumping) {
        if (flipped) {
            boundingBox->updateHitBox(centerY+(height/2)-(height*.12),
                centerY-(height/2)+(height*.07),
                centerX-(width/2),
                centerX+(height/2)-(height*.16));
        } else {
            boundingBox->updateHitBox(centerY+(height/2)-(height*.12),
                centerY-(height/2)+(height*.07),
                centerX-(width/2),
                centerX+(height/2)-(height*.16));
        }
    } else if (flipped) {
        if (prone) {
            boundingBox->updateHitBox(centerY - (height*.45),
                                 centerY - (height*1),
                                 centerX - (height*.5),      //13
                                 centerX + (height / 2) - (height * .2080));  //12
        } else if (aimUp) {
            boundingBox->updateHitBox(centerY + (height),
                                centerY - (height / 2) - (height * .486111), //28
                                centerX - (width / 2),      //5
                                centerX + (width / 2) + (height * .04));     //20
        } else { //standing
            boundingBox->updateHitBox(centerY+(height/2),
                                centerY-(height/2)-(height*.486111), //28
                                centerX-(width/2)-(height*.35), //13
                                centerX+(height/2)-(height*.2080)); //12
        }
    } else {
        if (prone) {
            boundingBox->updateHitBox(centerY - (height*.45),
                                 centerY - (height*1), 
                                 centerX - (height*.5),  //5
                                 centerX + (height / 2) - (height * .2));     //20
        } else if (aimUp) {
            boundingBox->updateHitBox(centerY + (height),
                                centerY - (height / 2) - (height * .486111), //28
                                centerX - (width / 2) + (height * .07),      //5
                                centerX + (width / 2));     //20
        } else { //standing
            boundingBox->updateHitBox(centerY+(height/2),
                                centerY-(height/2)-(height*.486111), //28
                                centerX-(width/2)+(height*.05), //5
                                centerX+(width/2)+(height*.35)); //20
        }
    }
    /* ************End Bounding Box Updating********** */
}

void Character::drawOptimized()
{
    #ifdef PROFILING
    ///////////////////////////Timer///////////////////

    static double timeDifference = 0.0;
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    //////////////////////////////////////////////////
    #endif

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.walkTexture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);
    
    float ssWidth = (float)1.0/img[spriteSheetIndex].columns;
    float ssHeight = (float)1.0/img[spriteSheetIndex].rows;
    
    int ix = rambo.frame % img[spriteSheetIndex].columns;
    int iy = 0;
    
    //move to next row of spriteSheet (if available)
    if (frame >= img[spriteSheetIndex].columns) {
        iy = 1;
    }
    
    float textureX = (float)ix / img[spriteSheetIndex].columns;
    float textureY = (float)iy / img[spriteSheetIndex].rows;
    
    glBegin(GL_QUADS);
    glTexCoord2f(textureX, textureY+ssHeight);
    glVertex2i(flipped ? centerX+width : centerX-width, centerY-height);
    
    glTexCoord2f(textureX, textureY);
    glVertex2i(flipped ? centerX+width : centerX-width, centerY+height);
    
    glTexCoord2f(textureX+ssWidth, textureY);
    glVertex2i(flipped ? centerX-width : centerX+width, centerY+height);
    
    glTexCoord2f(textureX+ssWidth, textureY+ssHeight);
    glVertex2i(flipped ? centerX-width : centerX+width, centerY-height);
    glEnd();
    
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);

    #ifdef PROFILING
    //////////////////TIMER/////////////////////////////
    for (int i = 0; i < 1000000; i++) {
        int a = 2 >> 2;
        a = a << 1;
    }

    clock_gettime(CLOCK_REALTIME, &end);
    timeDifference += timers.timeDiff(&start, &end);

    //print time
    Rect r;
	r.bot = g.yres - 230;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00000000, "Character::drawOptimized(): %lf", timeDifference);
    ///////////////////////////////////////////////////
    #endif
}
 
//Handle Input and animations
void joshuaCInput()
{
    //check if rambo is standing
    if (keys[XK_Right] == 0 && keys[XK_Left] == 0 && !rambo.jumping) {
        rambo.frame = 0;
        rambo.velocityX = 0;
    }

    jumpAnimation();
    walkLeft();
    walkRight();
    angleUpAnimation();
    angleDownAnimation();
    shootAndRunAnimation();
    proneAnimation();
    aimUpAnimation();
}

//function that makes character walk left
void walkLeft()
{
    if (keys[XK_Left]) {
        
        rambo.flipped = true;
        rambo.velocityX = -4;
        if (rambo.shooting || keys[XK_space] || keys[XK_Up] || keys[XK_Down])
            return;
        timers.recordTime(&timers.timeCurrent);
        //record time between frames
        double timeSpan = timers.timeDiff(&timers.walkTime,
                                          &timers.timeCurrent);
        if (timeSpan > g.delay) {
            //advance frame
            ++rambo.frame;
            if (rambo.frame >= 7) {
                rambo.frame = 1;
            }
            timers.recordTime(&timers.walkTime);
        }
    }
}

//function that makes character walk right
void walkRight()
{
    if (keys[XK_Right]) {
        rambo.flipped = false;
        rambo.velocityX = 4;
        if(rambo.shooting || keys[XK_space] || keys[XK_Up] || keys[XK_Down])
            return;
        timers.recordTime(&timers.timeCurrent);
        //record time between frames
        double timeSpan = timers.timeDiff(&timers.walkTime,
                                          &timers.timeCurrent);
        if (timeSpan > g.delay) {
            //advance frame
            ++rambo.frame;
            if (rambo.frame >= 7) {
                rambo.frame = 1;
            }
            timers.recordTime(&timers.walkTime);
        }
    }
}

void jumpAnimation()
{
    if (rambo.centerY > 100) {
        rambo.jumping = true;
    } else {
        rambo.jumping = false;
    }

    if (keys[XK_a] || rambo.jumping) {
        
        //check if in walk state
        if ((rambo.frame >= 0 && rambo.frame <= 6) || (rambo.frame >= 11 && rambo.frame <= 13)) {
            rambo.frame = 7;
        }
        //rambo.flipped = (keys[XK_Left] ? true : false);
        
        timers.recordTime(&timers.timeCurrent);
        //record time between frames
        double timeSpan = timers.timeDiff(&timers.walkTime,
                                          &timers.timeCurrent);
        if (timeSpan > g.delay) {
            //advance frame
            ++rambo.frame;
            if (rambo.frame >= 11) {
                rambo.frame = 7;
            }
            timers.recordTime(&timers.walkTime);
        }
    }
}

void shootAndRunAnimation()
{
    static double timeDifference = 0.0;
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    if (keys[XK_space])
        timeDifference = 0;
    if ((keys[XK_space] && !rambo.jumping && /*rambo.frame != 0 &&*/ !keys[XK_Up] && !keys[XK_Down]) 
        || (rambo.shooting && !rambo.jumping && !rambo.angleUp 
        && !rambo.angleDown && !rambo.aimUp && !rambo.prone)) {

        rambo.shootingStraight = true;

        
        //printf("we in here boiii\n");
        rambo.shooting = true;
        //start rambo in correct position
        //TODO: must update this switch statement as more
        //animations are added
        switch (rambo.frame) {
            case 1:
            case 4:
                rambo.frame = 12;
                break;
            case 2:
            case 5:
                rambo.frame = 13;
                break;
            case 3:
            case 6:
                rambo.frame = 11;
                break;
            case 7:
            case 8:
            case 9:
            case 10:
                rambo.frame = 11;
            default:
                break;
        }

        timers.recordTime(&timers.timeCurrent);

        double timeSpan = timers.timeDiff(&timers.walkTime,
                                          &timers.timeCurrent);

        if (timeSpan > g.delay) {
            //advance frame
            ++rambo.frame;
            if (rambo.frame >= 14 || rambo.frame < 11) {
                rambo.frame = 11;
            }
            timers.recordTime(&timers.walkTime);
        }

        //prevent rambo from releasing from his shooting position too early
        //not doing this makes the shooting look glitchy
        clock_gettime(CLOCK_REALTIME, &end);
        timeDifference += timers.timeDiff(&start, &end);
        if (timeDifference > .00002) {
            rambo.shooting = false;
            timeDifference = 0;
        }   
    }
    else{
        rambo.shootingStraight = false;
    }
    return;
}

void angleUpAnimation()
{
    if (((keys[XK_Right] && keys[XK_Up]) || (keys[XK_Left] && keys[XK_Up])) && !rambo.isJumping()) {
        rambo.angleUp = true;
        //start rambo in correct position
        //TODO: must update this switch statement as more
        //animations are added
        switch (rambo.frame)
        {
        case 1:
        case 4:
        case 12:
            rambo.frame = 15;
            break;
        case 2:
        case 5:
        case 13:
            rambo.frame = 16;
            break;
        case 3:
        case 6:
        case 11:
            rambo.frame = 14;
            break;
        case 7:
        case 8:
        case 9:
        case 10:
            rambo.frame = 14;
        case 0:
            rambo.frame = 15;
        }

        timers.recordTime(&timers.timeCurrent);
        //record time between frames
        double timeSpan = timers.timeDiff(&timers.walkTime,
                                          &timers.timeCurrent);
        if (timeSpan > g.delay) {
            //advance frame
            ++rambo.frame;
            if (rambo.frame >= 17)
            {
                rambo.frame = 14;
            }
            timers.recordTime(&timers.walkTime);
        }
    } else {
        rambo.angleUp = false;
    }
}

void angleDownAnimation()
{
    if (((keys[XK_Right] && keys[XK_Down]) || (keys[XK_Left] && keys[XK_Down])) && !rambo.isJumping()) {
        rambo.angleDown = true;
        //start rambo in correct position
        //TODO: must update this switch statement as more
        //animations are added
        switch (rambo.frame) {
        case 1:
        case 4:
        case 12:
        case 15:
            rambo.frame = 18;
            break;
        case 2:
        case 5:
        case 13:
        case 16:
            rambo.frame = 19;
            break;
        case 3:
        case 6:
        case 11:
        case 14:
            rambo.frame = 17;
            break;
        case 7:
        case 8:
        case 9:
        case 10:
            rambo.frame = 17;
        case 0:
            rambo.frame = 18;
        }

        timers.recordTime(&timers.timeCurrent);
        //record time between frames
        double timeSpan = timers.timeDiff(&timers.walkTime,
                                          &timers.timeCurrent);
        if (timeSpan > g.delay) {
            //advance frame
            ++rambo.frame;
            if (rambo.frame >= 20) {
                rambo.frame = 17;
            }
            timers.recordTime(&timers.walkTime);
        }
    } else {
        rambo.angleDown = false;
    }
}

void proneAnimation()
{
    if ((rambo.frame == 0 || rambo.prone) 
        && keys[XK_Down] && !keys[XK_Up] && !keys[XK_Left]
        && !keys[XK_Right] && !rambo.isJumping()) {

        rambo.setFrame(20);
        rambo.prone = true;
    } else {
        rambo.prone = false;
    }
    return;
}

void aimUpAnimation()
{
    if ((rambo.frame == 0 || rambo.aimUp) 
        && keys[XK_Up] && !keys[XK_Down] && !keys[XK_Left] && !keys[XK_Right] 
        && !rambo.isJumping()) {

        rambo.setFrame(21);
        rambo.aimUp = true;
    } else {
        rambo.aimUp = false;
    }
    return;
}

//This function logs rambo's various shooting directions to the debug menu
void printJoshuaC(int x, int y, int size, int color)
{
    Rect r;
    r.bot = y;
    r.left = x-50;
    r.center = 0;
    
    ggprint8b(&r, size, 0, "Rambo shooting state: ");
    r.left = x;
    r.bot = y-20;
    if (rambo.jumping)
        ggprint8b(&r, size, color, "jumping");
    if (rambo.aimUp)
        ggprint8b(&r, size, color, "Aiming up");
    if (rambo.prone)
        ggprint8b(&r, size, color, "Prone");
    if (rambo.angleUp)
        ggprint8b(&r, size, color, "Angle up");
    if (rambo.angleDown)
        ggprint8b(&r, size, color, "Angle down");
    if (rambo.shootingStraight)
        ggprint8b(&r, size, color, "Shooting straight");
}
