#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "entity.h"

class Projectile : public Entity {
    protected:
        vector_2d velocity{};
        bool is_deflected = false;
    public:
        Projectile(point_2d start, point_2d target);

        void update(double dt);
        void draw();
        void deflect();
        bool get_is_deflected() const;
};


#endif