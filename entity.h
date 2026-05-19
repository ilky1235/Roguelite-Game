#ifndef ENTITY_H
#define ENTITY_H
#include "splashkit.h"
#include "dynamic_array.h"


class Entity {
    protected:
        point_2d coordinates{};
        double health{};
        double movement_speed{};
        double max_health{};

    public:
        Entity(double x, double y, double health, double speed);
        void draw();
        void update();
        void take_damage(double amount);
        point_2d get_coordinates() const;
        double get_health() const;
        
};










#endif