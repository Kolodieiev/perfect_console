#pragma once
#include "../../defines.h"

namespace pixeler
{
  class Tile
  {
  public:
    // Типи поверхні плитки. Можете розширити власними типами, за потреби.
    enum TileType : uint16_t
    {
      TYPE_NONE = 0,
      TYPE_WALL = 1 << 0,
      TYPE_GROUND = 1 << 1,
      TYPE_WATER = 1 << 2,
      TYPE_AIR = 1 << 3,
      TYPE_FIRE = 1 << 4,
      TYPE_SNOW = 1 << 5,
      TYPE_MUD = 1 << 6,
      TYPE_ROAD = 1 << 7,
      TYPE_TREE = 1 << 8,
      TYPE_IMPASSABLE = 1 << 9,
    };

    Tile(TileType type, const uint16_t* img_ptr) : _img_ptr{img_ptr}, _type{type} {}

    const uint16_t* _img_ptr;
    const TileType _type;
  };
}  // namespace pixeler