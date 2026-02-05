//
//  GLGameScene.cpp
//  Geometry Lab
//
//  This is the primary class file for running the game.  You should study this 
//  file for ideas on how to structure your primary game scene. This class 
//  illustrates how to use the geometry tools in CUGL, and how to combine
//  them with the physics engine.
//
//  Note that this time we do not have any additional model classes.
//
//  Author: Walker White
//  Version: 1/29/24
//
#include <cugl/cugl.h>
#include <cugl/core/math/polygon/CUPathFactory.h>  // Bug that this is not in <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "GLGameScene.h"

using namespace cugl;
using namespace std;

#pragma mark -
#pragma mark Level Layout

// Lock the screen size to fixed height regardless of aspect ratio
#define SCENE_HEIGHT 720

/** How big the handle should be */
#define KNOB_RADIUS 15

/** The extrustion width. */
#define LINE_WIDTH 50

/** The width of a handle. */
#define HANDLE_WIDTH 3

/** The ratio between the physics world and the screen. */
#define PHYSICS_SCALE 50

/** The initial control points for the spline. */
float CIRCLE[] = {    0,  200,  120,  200,
        200,  120,  200,    0,  200, -120,
        120, -200,    0, -200, -120, -200,
       -200, -120, -200,    0, -200,  120,
       -120,  200,    0,  200};


/** The (CLOCKWISE) polygon for the star */
float STAR[] = {     0,    50,  10.75,    17,   47,     17,
                 17.88, -4.88,   29.5, -40.5,    0, -18.33,
                 -29.5, -40.5, -17.88, -4.88,  -47,     17,
                -10.75,    17};

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
    // Initialize the scene to a locked width
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(Size(0,SCENE_HEIGHT))) {
        return false;
    }

    _input.init();
    
    int count = (int)(sizeof(CIRCLE) / sizeof(float)) / 2;
    const cugl::Vec2* pts = reinterpret_cast<const cugl::Vec2*>(CIRCLE);
    _spline.set(pts, count);
    _spline.setClosed(true);

    int scount = (int)(sizeof(STAR) / sizeof(float)) / 2;
    const cugl::Vec2* spts = reinterpret_cast<const cugl::Vec2*>(STAR);

    cugl::Path2 starPath;
    starPath.vertices.reserve(scount);
    for (int i = 0; i < scount; i++) {
        starPath.vertices.push_back(spts[i]);
    }
    starPath.closed = true;

    starPath.reverse();

    cugl::EarclipTriangulator tri;
    tri.set(starPath);
    tri.calculate();
    _starPoly = tri.getPolygon();

    // ---- create star node ONCE ----
    cugl::Vec2 offset = getSize() / 2.0f;
    _starNode = cugl::scene2::PolygonNode::allocWithPoly(_starPoly);
    _starNode->setColor(cugl::Color4::BLUE);
    _starNode->setAnchor(cugl::Vec2::ANCHOR_CENTER);
    _starNode->setPosition(offset);
    addChild(_starNode);


    buildGeometry();
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    // NOTHING TO DO THIS TIME
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * The method called to indicate the start of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the beginning of the core application loop,
 * which is concluded with a call to {@link #postUpdate}.
 *
 * This method should be used to process any events that happen early in
 * the application loop, such as user input or events created by the
 * {@link schedule} method. In particular, no new user input will be
 * recorded between the time this method is called and {@link #postUpdate}
 * is invoked.
 *
 * Note that the time passed as a parameter is the time measured from the
 * start of the previous frame to the start of the current frame. It is
 * measured before any input or callbacks are processed. It agrees with
 * the value sent to {@link #postUpdate} this animation frame.
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void GameScene::preUpdate(float dt) {
    _input.update();

    auto toSplineCoords = [this](const cugl::Vec2& screenPos) {
        cugl::Vec2 w = this->screenToWorldCoords(screenPos);
        w -= this->getSize() / 2.0f;
        return w;
    };

    if (_input.didPress()) {
        cugl::Vec2 mouse = toSplineCoords(_input.getPosition());

        int anchors = (int)_spline.size();
        int tcount  = 2 * anchors;

        float r2 = (float)(KNOB_RADIUS * KNOB_RADIUS);
        int best = -1;
        float bestd2 = r2;

        for (int i = 0; i < tcount; i++) {
            cugl::Vec2 cp = _spline.getTangent((size_t)i);
            float d2 = (mouse - cp).lengthSquared();
            if (d2 <= bestd2) {
                best = i;
                bestd2 = d2;
            }
        }

        if (best != -1) {
            _activeTangent = best;
            _dragging = true;
            _lastMouse = mouse;
        }
    }

    if (_dragging && _activeTangent != -1 && _input.isDown()) {
        cugl::Vec2 curr = toSplineCoords(_input.getPosition());
        cugl::Vec2 delta = curr - _lastMouse;
        _lastMouse = curr;

        if (delta.lengthSquared() > 0) {
            cugl::Vec2 cp = _spline.getTangent((size_t)_activeTangent);
            _spline.setTangent((size_t)_activeTangent, cp + delta, true);

            buildGeometry();
        }
    }

    if (_input.didRelease()) {
        _dragging = false;
        _activeTangent = -1;
    }
}




/**
 * The method called to provide a deterministic application loop.
 *
 * This method provides an application loop that runs at a guaranteed fixed
 * timestep. This method is (logically) invoked every {@link getFixedStep}
 * microseconds. By that we mean if the method {@link draw} is called at
 * time T, then this method is guaranteed to have been called exactly
 * floor(T/s) times this session, where s is the fixed time step.
 *
 * This method is always invoked in-between a call to {@link #preUpdate}
 * and {@link #postUpdate}. However, to guarantee determinism, it is
 * possible that this method is called multiple times between those two
 * calls. Depending on the value of {@link #getFixedStep}, it can also
 * (periodically) be called zero times, particularly if {@link #getFPS}
 * is much faster.
 *
 * As such, this method should only be used for portions of the application
 * that must be deterministic, such as the physics simulation. It should
 * not be used to process user input (as no user input is recorded between
 * {@link #preUpdate} and {@link #postUpdate}) or to animate models.
 *
 * The time passed to this method is NOT the same as the one passed to
 * {@link #preUpdate}. It will always be exactly the same value.
 *
 * @param step    The fixed timestep in microseconds
 */
void GameScene::fixedUpdate(Uint64 step) {
    // ADD CODE HERE
}

/**
 * The method called to indicate the end of a deterministic loop.
 *
 * This method is used instead of {@link #update} if {@link #setDeterministic}
 * is set to true. It marks the end of the core application loop, which was
 * begun with a call to {@link #preUpdate}.
 *
 * This method is the final portion of the update loop called before any
 * drawing occurs. As such, it should be used to implement any final
 * animation in response to the simulation provided by {@link #fixedUpdate}.
 * In particular, it should be used to interpolate any visual differences
 * between the the simulation timestep and the FPS.
 *
 * This method should not be used to process user input, as no new input
 * will have been recorded since {@link #preUpdate} was called.
 *
 * Note that the time passed as a parameter is the time measured from the
 * last call to {@link #fixedUpdate}. That is because this method is used
 * to interpolate object position for animation.
 *
 * @param remain    The amount of time (in seconds) last fixedUpdate
 */
void GameScene::postUpdate(float remain) {
    // ADD CODE HERE
}

/**
 * Draws all this scene to its associated SpriteBatch.
 *
 * The default implementation of this method simply draws the scene graph
 * to the sprite batch. By overriding it, you can do custom drawing
 * in its place. The expectation is that you will use the associated
 * SpriteBatch for drawing, but this is not required.
 */
void GameScene::render() {
    
    cugl::scene2::Scene2::render();
   
    // DO NOT DO THIS IN YOUR FINAL GAME
    _batch->setPerspective(getCamera()->getCombined());
    _batch->begin();


    _batch->end();
}

/**
 * Rebuilds the geometry.
 *
 * This method should recreate all the polygons for the spline, the handles
 * and the falling star. It should also recreate all physics objects.
 *
 * However, to cut down on performance overhead, this method should NOT add
 * those physics objects to the world inside this method (as this method is
 * called repeatedly while the user moves a handle). Instead, those objects
 * should not be activated until the state is "stable".
 */
void GameScene::buildGeometry() {
    cugl::Vec2 offset = getSize() / 2.0f;

    cugl::SplinePather pather;
    pather.set(&_spline);
    pather.calculate();
    _path = pather.getPath();

    cugl::SimpleExtruder extruder;
    extruder.set(_path);
    extruder.calculate(LINE_WIDTH);
    _extrusion = extruder.getPolygon();

    if (_solid == nullptr) {
        _solid = cugl::scene2::PolygonNode::allocWithPoly(_extrusion);
        _solid->setColor(cugl::Color4::BLACK);
        _solid->setAnchor(cugl::Vec2::ANCHOR_CENTER);
        _solid->setPosition(offset);
        addChild(_solid);
    } else {
        _solid->setPolygon(_extrusion);
    }

    if (_wire == nullptr) {
        _wire = cugl::scene2::WireNode::allocWithPath(_path);
        _wire->setColor(cugl::Color4::BLACK);
        _wire->setAnchor(cugl::Vec2::ANCHOR_CENTER);
        _wire->setPosition(offset);
        addChild(_wire);
    } else {
        _wire->setPath(_path);
    }

    int anchors = (int)_spline.size();
    int tcount  = 2 * anchors;

    if ((int)_handleNodes.size() != anchors) {
        for (auto& n : _handleNodes) { if (n) n->removeFromParent(); }
        _handleNodes.assign(anchors, nullptr);
    }
    if ((int)_knobNodes.size() != tcount) {
        for (auto& n : _knobNodes) { if (n) n->removeFromParent(); }
        _knobNodes.assign(tcount, nullptr);
    }

    cugl::PolyFactory factory;
    cugl::Poly2 knobUnit;

    knobUnit = factory.makeCircle(cugl::Vec2::ZERO, KNOB_RADIUS);

    for (int pos = 0; pos < anchors; pos++) {
        int ridx = 2 * pos;
        int lidx = 2 * pos - 1;
        if (lidx < 0) lidx = tcount - 1;

        cugl::Vec2 right = _spline.getTangent((size_t)ridx);
        cugl::Vec2 left  = _spline.getTangent((size_t)lidx);
        cugl::Vec2 mid   = (left + right) * 0.5f;

        cugl::Path2 seg;
        seg.vertices.push_back(left  - mid);
        seg.vertices.push_back(right - mid);

        cugl::SimpleExtruder hextr;
        hextr.set(seg);
        hextr.calculate(HANDLE_WIDTH);
        cugl::Poly2 handlePoly = hextr.getPolygon();

        auto& hnode = _handleNodes[pos];
        if (!hnode) {
            hnode = cugl::scene2::PolygonNode::allocWithPoly(handlePoly);
            hnode->setColor(cugl::Color4::WHITE);
            hnode->setAnchor(cugl::Vec2::ANCHOR_CENTER);
            addChild(hnode);
        } else {
            hnode->setPolygon(handlePoly);
        }
        hnode->setPosition(offset + mid);

        auto ensureKnob = [&](int idx, const cugl::Vec2& p) {
            auto& kn = _knobNodes[idx];
            if (!kn) {
                kn = cugl::scene2::PolygonNode::allocWithPoly(knobUnit);
                kn->setColor(cugl::Color4::RED);
                kn->setAnchor(cugl::Vec2::ANCHOR_CENTER);
                addChild(kn);
            }
            kn->setPosition(offset + p);
        };

        ensureKnob(lidx, left);
        ensureKnob(ridx, right);
    }
}


