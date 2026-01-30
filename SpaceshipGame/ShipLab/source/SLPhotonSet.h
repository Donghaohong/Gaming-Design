//
//  PhotonSet.h
//
//  In the intro class we implemented the photons as a "particle system". That
//  is because memory was a tricky resource in Java. While we obviously need
//  to worry about memory in C++, we don't need to use anything as advanced as
//  free lists just yet.  Smart pointers will take care of us!
//
//  With that said, the design of this class is entirely up to you.  Follow
//  the instructions carefully.
//
//  Author:  YOUR NAME HERE
//  Version: THE DATE HERE
//
#ifndef __SL_PHOTON_SET_H__
#define __SL_PHOTON_SET_H__
#include <cugl/cugl.h>
#include <unordered_set>

class PhotonSet {
public:
    /** Inner photon class (a single photon instance) */
    class Photon {
    public:
        cugl::Vec2 position;
        cugl::Vec2 velocity;
        float      scale;
        int        age;
        int        maxage;

        Photon(const cugl::Vec2 p, const cugl::Vec2 v, float s, int maxa);

        /** Update this photon by 1 animation frame (wrap-around supported) */
        void update(cugl::Size size, float baseRadius);
    };

protected:
    // Shared constants for all photons
    float _speed;
    float _mass;
    int   _maxage;

    // Drawing/shared resources
    float _radius; // base radius (from texture size)
    std::shared_ptr<cugl::graphics::Texture> _texture;

    // Active photons
    std::unordered_set<std::shared_ptr<Photon>> _photons;

public:
    PhotonSet();

    /** Initialize shared photon constants from JSON (speed/mass/max age) */
    bool init(std::shared_ptr<cugl::JsonValue> data);

    /** Set the texture used by all photons and compute radius from it */
    void setTexture(const std::shared_ptr<cugl::graphics::Texture>& value);

    /** Spawn a new photon at ship position, with ship angle + ship velocity */
    void spawnPhoton(const cugl::Vec2 shipPos, const cugl::Vec2 shipVel, float shipAngle);

    /** Update all photons; delete those that reach max age */
    void update(cugl::Size size);

    /** Draw all photons; supports wrap-around */
    void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, cugl::Size size);

    /** Clear all photons (useful for reset) */
    void clear() { _photons.clear(); }
    
    std::unordered_set<std::shared_ptr<Photon>>& getPhotons() { return _photons; }
    const std::unordered_set<std::shared_ptr<Photon>>& getPhotons() const { return _photons; }

    float getMass()   const { return _mass;   }
    float getRadius() const { return _radius; }
};
#endif /* __SL_PHOTON_SET_H__ */
