#if !defined(BLUEPRINTS_H)
#define BLUEPRINTS_H

#include <string>
#include <vector>
#include <SDL_rect.h>

namespace shape
{
    std::string         get_map_list_blueprint();
    std::vector<float>  get_sword_blueprint();
    std::vector<float>  get_shield_blueprint();
    std::vector<float>  get_cross_blueprint();
} // namespace shape


#endif // BLUEPRINTS_H
