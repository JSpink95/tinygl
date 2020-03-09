//////////////////////////////////////////////////////////////////////////
//    File        	: type_aabb.hpp
//    Created By    : Jack Spink
//    Created On 	: [13/9/2019]
//////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "type_vec.hpp"

//////////////////////////////////////////////////////////////////////////

namespace glm
{
    template <typename T, precision P, template <typename, precision> class vecType>
    struct taabb
    {
        vecType<T, P> min, max;
    };

    typedef taabb<float, highp, tvec2> highp_faabb2;
    typedef taabb<float, highp, tvec3> highp_faabb3;

    typedef highp_faabb3 aabb;
    typedef highp_faabb2 aabb2;
}

//////////////////////////////////////////////////////////////////////////
#include "type_aabb.inl"
//////////////////////////////////////////////////////////////////////////