//
//  SLGameScene.cpp
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
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "SLGameScene.h"
//#include "SLCollisionController.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#pragma mark -
#pragma mark Level Layout

// Lock the screen size to fixed height regardless of aspect ratio
#define SCENE_HEIGHT 720

#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked height
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(Size(0,SCENE_HEIGHT))) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    
    // Get the background image and constant values
    _background = assets->get<Texture>("background");
    _constants = assets->get<JsonValue>("constants");

    // Make a ship and set its texture
    _ship = std::make_shared<Ship>(getSize()/2, _constants->get("ship"));
    _ship->setTexture(assets->get<Texture>("ship"));

    // Initialize the asteroid set
    _asteroids.init(_constants->get("asteroids"));
    _asteroids.setTexture(assets->get<Texture>("asteroid1"));
    
    _photons.init(_constants->get("photons"));
    _photons.setTexture(assets->get<Texture>("photon"));

    // Get the bang sound
    _bang = assets->get<Sound>("bang");
    
    _laser = assets->get<Sound>("laser");
    
    _blast = assets->get<Sound>("blast");

    // Create and layout the health meter
    std::string msg = strtool::format("Health %d", _ship->getHealth());
    _text = TextLayout::allocWithText(msg, assets->get<Font>("pixel32"));
    _text->layout();
    
    _collisions.init(getSize());
    
    reset();
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
        _assets = nullptr;
    }
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    
    _gameOver = false;
    _didWin   = false;
    _endText  = nullptr;
    _photons.clear();
    
    _ship->setPosition(getSize()/2);
    _ship->setAngle(0);
    _ship->setVelocity(Vec2::ZERO);
    _ship->setHealth(_constants->get("ship")->getInt("health",0));
    _asteroids.init(_constants->get("asteroids"));
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not a graphics command
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void GameScene::update(float dt) {
    // Read the keyboard for each controller.
    _input.readInput();
    if (_input.didPressReset()) {
        reset();
    }
    
    if (_gameOver) {
        return;
    }

    // Move the ships and photons forward (ignoring collisions)
    _ship->move( _input.getForward(),  _input.getTurn(), getSize());
    
    // Move the asteroids
    _asteroids.update(getSize());
    
    if (_input.didPressFire() && _ship->canFireWeapon()) {
        _photons.spawnPhoton(_ship->getPosition(),
                             _ship->getVelocity(),
                             _ship->getAngle());
        _ship->reloadWeapon();
        
        AudioEngine::get()->play("laser", _laser, false);
    }
    
    _photons.update(getSize());
    
    if (_collisions.resolvePhotonCollision(_photons, _asteroids)) {
        AudioEngine::get()->play("blast", _blast, false);
    }
    
    // Check for collisions and play sound
    if (_collisions.resolveCollision(_ship, _asteroids)) {
        AudioEngine::get()->play("bang", _bang, false, _bang->getVolume(), true);
    }
    
    // Update the health meter
    _text->setText(strtool::format("Health %d", _ship->getHealth()));
    _text->layout();
    
    if (!_gameOver && _ship->getHealth() <= 0) {
        _gameOver = true;
        _didWin   = false;

        _endText = cugl::graphics::TextLayout::allocWithText("YOU LOSE", _assets->get<cugl::graphics::Font>("pixel32"));
        _endText->layout();
    } else if (!_gameOver && _asteroids.current.empty()) {
        _gameOver = true;
        _didWin   = true;

        _endText = cugl::graphics::TextLayout::allocWithText("YOU WIN", _assets->get<cugl::graphics::Font>("pixel32"));
        _endText->layout();
    }
}

/**
 * Draws all this scene to the scene's SpriteBatch.
 *
 * The default implementation of this method simply draws the scene graph
 * to the sprite batch. By overriding it, you can do custom drawing
 * in its place.
 */
void GameScene::render() {
    // For now we render 3152-style
    // DO NOT DO THIS IN YOUR FINAL GAME
    _batch->setPerspective(getCamera()->getCombined());
    _batch->begin();
    
    _batch->draw(_background,Rect(Vec2::ZERO,getSize()));
    _asteroids.draw(_batch,getSize());
    _ship->draw(_batch,getSize());
    
    _photons.draw(_batch, getSize());
    
    _batch->setColor(Color4::BLACK);
    _batch->drawText(_text,Vec2(10,getSize().height-_text->getBounds().size.height));
    _batch->setColor(Color4::WHITE);
    
    if (_gameOver && _endText) {
        // Choose color: green for win, red for loss
        _batch->setColor(_didWin ? Color4::GREEN : Color4::RED);

        // Scale by 3 (fonts are fixed size)
        Affine2 trans;
        trans.scale(3.0f);

        // Center on screen (note: bounds are BEFORE scaling)
        Vec2 tsize = _endText->getBounds().size;
        Vec2 pos((getSize().width  - 3.0f*tsize.x) / 2.0f,
                 (getSize().height - 3.0f*tsize.y) / 2.0f);
        trans.translate(pos);

        _batch->drawText(_endText, trans);

        // Restore color
        _batch->setColor(Color4::WHITE);
    }
    
    _batch->end();
}
