//
//  SLCollisionController.h
//  Programming Lab
//
//  Unless you are making a point-and-click adventure game, every single game is
//  going to need some sort of collision detection. In a later lab, we will see
//  how to do this with a physics engine. For now, we use custom physics.
//
//  You might ask why we need this file when we have box2d. That is because we
//  are trying to make this code as close to that of 3152 as possible. At this
//  point in the semester of 3152, we had not covered box2d.
//
//  As you work on this class, there is an interesting thing to note. The
//  constructor and the initializer are SEPARATE.  That is because the
//  constructor is called as soon as the object is created and that happens
//  BEFORE we know the window size.
//
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//
//  Author: Walker M. White
//  Version: 1/20/26
//
#include "SLCollisionController.h"

/** Impulse for giving collisions a slight bounce. */
#define COLLISION_COEFF     0.1f

using namespace cugl;

/**
 * Returns true if there is a ship-asteroid collision
 *
 * In addition to checking for the collision, this method also resolves it.
 * That means it applies damage to the ship for EACH asteroid encountered.
 * It does not, however, play the sound. That happens in the main controller
 *
 * Note that this method must take wrap into consideration as well. If the
 * asteroid/ship can be drawn at multiple points on the screen, then it can
 * collide at multiple places as well.
 *
 * @param ship  The players ship
 * @param aset  The asteroid set
 *
 * @return true if there is a ship-asteroid collision
 */
bool CollisionController::resolveCollision(const std::shared_ptr<Ship>& ship, AsteroidSet& aset) {
    bool collision = false;
    for(auto it = aset.current.begin(); it != aset.current.end(); ++it) {
        // Calculate the normal of the (possible) point of collision
        std::shared_ptr<AsteroidSet::Asteroid> rock = *it;
        
        // This loop finds the NEAREST collision if we include wrap for the asteroid/ship
        Vec2 norm = ship->getPosition()-rock->position;
        float distance = norm.length();
        float impactDistance = (ship->getRadius() + aset.getRadius()*rock->getScale());
        
        // This loop finds the NEAREST collision if we include wrap for the asteroid/ship
        for(int ii = -1; ii <= 1; ii++) {
            for(int jj = -1; jj <= 1; jj++) {
                Vec2 pos = rock->position;
                pos.x += (ii)*_size.width;
                pos.y += (jj)*_size.height;
                pos = ship->getPosition()-pos;
                float dist = pos.length();
                if (dist < distance) {
                    distance = dist;
                    norm = pos;
                }
            }
        }
        
        // If this normal is too small, there was a collision
        if (distance < impactDistance) {
            // "Roll back" time so that the ships are barely touching (e.g. point of impact).
            norm.normalize();
            Vec2 temp = norm * ((impactDistance - distance) / 2);
            ship->setPosition(ship->getPosition()+temp);
            rock->position = rock->position-temp;

            // Now it is time for Newton's Law of Impact.
            // Convert the two velocities into a single reference frame
            Vec2 vel = ship->getVelocity()-rock->velocity;

            // Compute the impulse (see Essential Math for Game Programmers)
            float impulse = (-(1 + COLLISION_COEFF) * norm.dot(vel)) /
                            (norm.dot(norm) * (1.0f / ship->getMass() + 1.0f / (ship->getMass()*rock->getScale())));
            if (norm.dot(norm) == 0) {
                // Just use the coefficient if the impulse is degenerate.
                impulse = COLLISION_COEFF;
            }

            // Change velocity of the two ships using this impulse
            temp = norm * (impulse/ship->getMass());
            ship->setVelocity(ship->getVelocity()+temp);

            temp = norm * (impulse/(ship->getMass()*rock->getScale()));
            rock->velocity = rock->velocity - temp;
            
            // Damage the ship as the last step
            ship->setHealth(ship->getHealth()-aset.getDamage());
            collision = true;
        }
    }
    return collision;
}

bool CollisionController::resolvePhotonCollision(PhotonSet& photons, AsteroidSet& ast) {
    bool hit = false;

    auto& plist = photons.getPhotons();
    auto& alist = ast.current;   // AsteroidSet 里 active asteroids (public in starter)

    // 半径（注意：true radius = base radius * scale）
    float pr = photons.getRadius();

    for (auto pit = plist.begin(); pit != plist.end(); ) {
        auto p = *pit;
        float truePR = pr * p->scale;

        bool removedPhoton = false;

        for (auto ait = alist.begin(); ait != alist.end(); ) {
            auto a = *ait;
            float trueAR = ast.getRadius() * a->getScale();

            // 圆碰撞：距离 <= 半径和
            float dist2 = (p->position - a->position).lengthSquared();
            float rad   = truePR + trueAR;

            if (dist2 <= rad * rad) {
                // Save info before deleting
                cugl::Vec2 apos = a->position;
                cugl::Vec2 avel = a->velocity;
                int atype = a->getType();   // 需要 Asteroid 有 getType()

                // Photon direction (unit vector)
                cugl::Vec2 pdir = p->velocity;
                if (pdir.lengthSquared() > 0) {
                    pdir.normalize();
                } else {
                    pdir = cugl::Vec2(1,0);
                }

                // Asteroid speed magnitude (keep same speed as old asteroid)
                float aspeed = avel.length();

                // Remove the hit asteroid and the photon
                ait = alist.erase(ait);
                pit = plist.erase(pit);

                // Split if type > 1
                if (atype > 1) {
                    // base velocity: same speed, photon direction
                    cugl::Vec2 v0 = pdir * aspeed;

                    // rotate by ±120 degrees
                    const float c120 = -0.5f;
                    const float s120 = 0.8660254f; // sqrt(3)/2

                    cugl::Vec2 v1(c120*v0.x - s120*v0.y, s120*v0.x + c120*v0.y);  // +120
                    cugl::Vec2 v2(c120*v0.x + s120*v0.y, -s120*v0.x + c120*v0.y); // -120

                    ast.spawnAsteroid(apos, v0, atype-1);
                    ast.spawnAsteroid(apos, v1, atype-1);
                    ast.spawnAsteroid(apos, v2, atype-1);
                }

                hit = true;
                removedPhoton = true;
                break; 
            }

            else {
                ++ait;
            }
        }

        if (!removedPhoton) {
            ++pit;
        }
    }

    return hit;
}




