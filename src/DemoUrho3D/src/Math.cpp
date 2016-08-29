#include <Urho3D/Math/Vector3.h>

#include "Math.h"


int Math::CircularIncrease(int value, int min, int max)
{
    ++value;
    return (value > max) ? min : value;
}

BoundingBox Math::CalculateBoundingBox(float *buffer, unsigned numVertexes)
{
    BoundingBox box(1e6f, -1e6f);

    for(unsigned i = 0; i < numVertexes; i++)
    {
        box.Merge(Vector3(buffer + i * 8));
    }

    return box;
}
