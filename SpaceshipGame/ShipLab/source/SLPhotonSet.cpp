//
//  PhotonSet.cpp
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
#include "SLPhotonSet.h"
using namespace cugl;
using namespace cugl::graphics;

#pragma mark -
#pragma mark PhotonSet

PhotonSet::PhotonSet() :
_speed(0),
_mass(0),
_maxage(0),
_radius(0),
_texture(nullptr) { }

bool PhotonSet::init(std::shared_ptr<cugl::JsonValue> data) {
    if (!data) {
        return false;
    }

    _photons.clear();

    _speed  = data->getFloat("speed", 0);
    _mass   = data->getFloat("mass", 0);
    _maxage = data->getInt("max age", 0);

    _radius  = 0;
    _texture = nullptr;

    return true;
}

void PhotonSet::setTexture(const std::shared_ptr<cugl::graphics::Texture>& value) {
    _texture = value;
    if (_texture) {
        cugl::Size size = _texture->getSize();
        _radius = std::max(size.width, size.height) / 2.0f;
    } else {
        _radius = 0;
    }
}

#pragma mark -
#pragma mark Photon (Inner Class)

PhotonSet::Photon::Photon(const cugl::Vec2 p,
                          const cugl::Vec2 v,
                          float s,
                          int maxa)
: position(p), velocity(v), scale(s), age(0), maxage(maxa) {
}

void PhotonSet::Photon::update(cugl::Size size, float baseRadius) {
    position += velocity;

    while (position.x > size.width)  { position.x -= size.width; }
    while (position.x < 0)           { position.x += size.width; }
    while (position.y > size.height) { position.y -= size.height; }
    while (position.y < 0)           { position.y += size.height; }

    age += 1;
    scale = 1.5f - 1.5f * ((float)age / (float)maxage);
}

void PhotonSet::update(cugl::Size size) {
    for (auto it = _photons.begin(); it != _photons.end(); ) {
        (*it)->update(size, _radius);

        if ((*it)->age >= (*it)->maxage) {
            it = _photons.erase(it);
        } else {
            ++it;
        }
    }
}

void PhotonSet::spawnPhoton(const cugl::Vec2 shipPos,
                            const cugl::Vec2 shipVel,
                            float shipAngle) {
    cugl::Vec2 dir(std::cos(shipAngle), std::sin(shipAngle));
    cugl::Vec2 vel = dir * _speed + shipVel;

    auto p = std::make_shared<Photon>(shipPos, vel, 1.5f, _maxage);
    _photons.emplace(p);
    CULog("Spawn photon. Count=%zu", _photons.size());
}

void PhotonSet::draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch, cugl::Size size) {
    if (!_texture) return;

    // 以贴图中心为 origin，避免画的时候左下角对齐
    cugl::Vec2 origin(_radius, _radius);

    for (auto it = _photons.begin(); it != _photons.end(); ++it) {
        std::shared_ptr<Photon> p = *it;

        float scale = p->scale;
        cugl::Vec2 pos = p->position;

        cugl::Affine2 trans;
        trans.scale(scale);
        trans.translate(pos);

        // 画主图
        batch->draw(_texture, origin, trans);

        // wrap-around：需要看“边缘”是否越界，所以用 true radius = baseRadius * scale
        float r = _radius * scale;

        // 横向越界补画
        if (pos.x + r > size.width) {
            trans.translate(-size.width, 0);
            batch->draw(_texture, origin, trans);
            trans.translate(size.width, 0);
        } else if (pos.x - r < 0) {
            trans.translate(size.width, 0);
            batch->draw(_texture, origin, trans);
            trans.translate(-size.width, 0);
        }

        // 纵向越界补画
        if (pos.y + r > size.height) {
            trans.translate(0, -size.height);
            batch->draw(_texture, origin, trans);
            trans.translate(0, size.height);
        } else if (pos.y - r < 0) {
            trans.translate(0, size.height);
            batch->draw(_texture, origin, trans);
            trans.translate(0, -size.height);
        }
    }
}
