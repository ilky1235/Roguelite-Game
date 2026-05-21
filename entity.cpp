#include "entity.h"
#include "dynamic_array.h"


Entity::Entity(double x, double y, double health, double speed)
{
    coordinates.x = x;
    coordinates.y = y;
    this->health = health;
    this->max_health = health;
    movement_speed = speed;
}




void Entity::take_damage(double amount)
{
    health -= amount;
}

void Entity::update()
{

}

point_2d Entity::get_coordinates() const
{
    return coordinates;
}

double Entity::get_health() const
{
    return health;
}




