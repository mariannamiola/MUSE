#ifndef MUSE_SAVE_OBJ_H
#define MUSE_SAVE_OBJ_H

#include <string>

namespace MUSE
{
// Export OBJ coordinates using the shortest lossless decimal representation.
template<class Mesh>
void save_obj(const Mesh& mesh, const std::string& filename);
}

#ifndef STATIC_MUSELIB
#include "save_obj.cpp"
#endif

#endif // MUSE_SAVE_OBJ_H
