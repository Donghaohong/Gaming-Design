//
//  SLGameScene.h
//  Ship Lab
//
//  This is the scene for running the game. Games are often separated into
//  mutliple scenes, each scene corresponding to a specific play interface.
//  Scenes correspond to the concept of player modes, which we talk about in
//  the introductory course.
//
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//
//  Author: Walker White
//  Version: 1/20/26
//
#ifndef __SL_GAME_SCENE_H__
#define __SL_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <vector>
#include <unordered_set>
#include "SLShip.h"
#include "SLPhotonSet.h"
#include "SLAsteroidSet.h"
#include "SLInputController.h"
#include "SLCollisionController.h"



/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller. This this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::scene2::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // CONTROLLERS are attached directly to the scene (no pointers)
    /** The controller to manage the ship */
    InputController _input;
    /** The controller for managing collisions */
    CollisionController _collisions;
    
    // MODELS should be shared pointers or a data structure of shared pointers
    /** The JSON value with all of the constants */
    std::shared_ptr<cugl::JsonValue> _constants;
    /** Location and animation information for the ship */
    std::shared_ptr<Ship> _ship;
    /** The location of all of the active asteroids */
    AsteroidSet _asteroids;
    
    PhotonSet _photons;

    
    // VIEW items are going to be individual variables
    // In the future, we will replace this with the scene graph
    /** The backgrounnd image */
    std::shared_ptr<cugl::graphics::Texture> _background;
    /** The text with the current health */
    std::shared_ptr<cugl::graphics::TextLayout> _text;
    /** The sound of a ship-asteroid collision */
    std::shared_ptr<cugl::audio::Sound> _bang;
    
    std::shared_ptr<cugl::audio::Sound> _blast;
    
    std::shared_ptr<cugl::audio::Sound> _laser;
    
    /** Whether the game is over (win or loss) */
    bool _gameOver = false;

    /** True if win, false if loss (valid only when _gameOver is true) */
    bool _didWin = false;

    /** Big end-of-game message (WIN / LOSE) */
    std::shared_ptr<cugl::graphics::TextLayout> _endText;

    
public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game mode with the default values.
     *
     * This constructor does not allocate any objects or start the game. This
     * allows us to use the object without a heap pointer.
     */
    GameScene() : cugl::scene2::Scene2() {}
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose() override;
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory. This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation. Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not a graphics command.
     *
     * @param dt    The amount of time (in seconds) since the last frame
     */
    void update(float dt) override;

    /**
     * Draws all this scene to the scene's SpriteBatch.
     *
     * The default implementation of this method simply draws the scene graph
     * to the sprite batch. By overriding it, you can do custom drawing
     * in its place.
     */
    void render() override;

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset() override;
};

#endif /* __SG_GAME_SCENE_H__ */
