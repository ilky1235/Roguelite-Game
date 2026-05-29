#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "entity.h"

class Projectile : public Entity {
    protected:
        vector_2d velocity{};
        bool is_deflected = false;
        bool is_friendly;
        double anim_timer{};
        int current_frame{};

    public:
        Projectile(point_2d start, point_2d target);

        void update(double dt);
        void draw();
        void deflect();
        bool get_is_deflected() const;
        bool get_is_friendly() const;
        void set_is_friendly(bool friendly);
};


#endif