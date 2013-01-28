/*========================================================*/
/*
Project-1 Part E. Sifteo Cubes
Hamburgers! by Elwin Lee
Carnegie Mellon University
 
51-882 Interactive Art & Computational Design
Class by Golan Levin, Spring 2013
Carnegie Mellon University, School of Art
 
 
/*Description
 Very basic program utilizing the shake function of the Sifteo Cubes.
 An animation is played when the cubes is shaken.
 
 
/*Credits
 - Based on Sifteo SDK Stars example
 - Pikachu sprite http://sprites.pokecheck.org/
 - mediochrea's SpritePlane https://github.com/mediochrea/SpritePlane
 */
/*========================================================*/


#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

static const unsigned gNumCubes = 1;	//how many active cubes

static AssetSlot MainSlot = AssetSlot::allocate()
    .bootstrap(GameAssets);

static Metadata M = Metadata()
    .title("PokeDemo")
    .package("com.sifteo.sdk.shaker", "0.9")
    .icon(Icon)
    .cubeRange(gNumCubes, CUBE_ALLOCATION);

static TiltShakeRecognizer motion[CUBE_ALLOCATION];


class PokeDemo {
public:
    
    static const float textSpeed = 0.2f;

	bool shaken = false;
	bool shakeAni = false;

    void init(CubeID cube)
    {
		Events::cubeAccelChange.set(&PokeDemo::onAccelChange, this);

        frame = 0;

        vid.initMode(BG0_SPR_BG1);	//background, foreground & sprites
        vid.attach(cube);
		motion[cube].attach(cube);

        // Our background is 18x18 to match BG0, and it seamlessly tiles
        vid.bg0.image(vec(0,0), Background);

        // Allocate 16x2 tiles on BG1 for text at the bottom of the screen
        vid.bg1.setMask(BG1Mask::filled(vec(0,14), vec(16,2)));

    }
    
    void update(TimeDelta timeStep)
    {
        /*
         * Action triggers
         */
        
        switch (frame) {
        case 0:
            text.set(0, -20);
            textTarget = text;
            break;
        
        case 100:
			shaken && shakeAni ? writeText(" ** DANCE!!! ** ") : writeText(" ** Shake it ** ");
	
            textTarget.y = 8;
            break;

        case 200:
			frame = 0;
            fpsTimespan = 0;
            textTarget.y = -20;
            break;
        }
       

        /*
         * Update global animation
         */

        frame++;
        fpsTimespan += timeStep;


        text += (textTarget - text) * textSpeed;
        vid.bg1.setPanning(text.round());

		const Float2 center = { 64 - 12.5f, 64 - 18.5f };
		if( shaken && shakeAni) 
		{
			vid.sprites[0].setImage(Pika, (frame) % Pika.numFrames());	//animated pikachu
			vid.sprites[0].move(center);
		} 
		else
		{
			vid.sprites[0].setImage(Pika, 0);	//static image
			vid.sprites[0].move(center);
		}
    }

	void onAccelChange(unsigned id)
    {
        CubeID cube(id);
        auto accel = cube.accel();		//physical accelerometer state, as a signed byte-vector
										//The auto storage-class specifier declares an automatic variable, a variable with a local lifetime. It is the default storage-class specifier for block-scoped variable declarations.

		if( shaken && accel.x == 0 && accel.y == 0 ) shakeAni = true;

		unsigned changeFlags = motion[id].update();
        if (changeFlags)
		{
			if( motion[id].shake && !shaken) {
				shaken = true;
				//LOG("Shake: %d\n", motion[id].shake);
			}
		}
    }


private:   
    VideoBuffer vid;
    unsigned frame;
    Float2 bg, text, textTarget;
    float fpsTimespan;

    void writeText(const char *str)
    {
        // Text on BG1, in the 16x2 area we allocated
        vid.bg1.text(vec(0,14), Font, str);
    }
};


void main()
{
    static PokeDemo instances[gNumCubes];

    AudioTracker::play(Music);

    for (unsigned i = 0; i < arraysize(instances); i++)
        instances[i].init(i);
    
    TimeStep ts;
    while (1) {
        for (unsigned i = 0; i < arraysize(instances); i++)
            instances[i].update(ts.delta());

        System::paint();
        ts.next();
    }
}
