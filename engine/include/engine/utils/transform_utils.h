#pragma once

#include "engine/utils/matrix.h"
#include "engine/utils/quaternion.h"
#include "engine/utils/vector.h"

struct TransformDecomposition {
    Vector3 translation;
    Quaternion rotation;
    Vector3 scale;
};

class TransformUtils {
public:
    static TransformDecomposition Decompose(const Matrix4& matrix);
    static Matrix4 FromTRS(
        const Vector3& position,
        const Quaternion& rotation,
        const Vector3& scale
    );
};
