#include "engine/ecs/components/transform.h"
#include "engine/serialization/jsonarchive.h"

#include <cassert>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>

#include "engine/debug/logger.h"

namespace {
    bool Near(float left, float right) {
        return std::abs(left - right) < 0.0001f;
    }

    void AssertVector(const Vector3& actual, const Vector3& expected) {
        assert(Near(actual.x, expected.x));
        assert(Near(actual.y, expected.y));
        assert(Near(actual.z, expected.z));
    }

    void TestIdentityDefaults() {
        Transform transform;
        AssertVector(transform.GetPosition(), Vector3(0.0f));
        AssertVector(transform.GetScale(), Vector3(1.0f));
        assert(Near(std::abs(transform.GetRotation().w), 1.0f));
    }

    void TestModificationFunctions() {
        Transform transform;
        Transform::SetPosition(transform, Vector3(1.0f, 2.0f, 3.0f));
        Transform::ChangePosition(transform, Vector3(2.0f, -1.0f, 4.0f));
        Transform::SetScale(transform, Vector3(2.0f, 3.0f, 4.0f));
        Transform::ChangeScale(transform, Vector3(1.0f));

        AssertVector(transform.GetPosition(), Vector3(3.0f, 1.0f, 7.0f));
        AssertVector(transform.GetScale(), Vector3(3.0f, 4.0f, 5.0f));
    }

    void TestCompositionAppliesParentRotationAndScale() {
        Transform parent;
        Transform child;
        Transform result;
        Transform::SetPosition(parent, Vector3(10.0f, 0.0f, 0.0f));
        Transform::SetRotation(parent, Quaternion(glm::angleAxis(
            glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f))));
        Transform::SetScale(parent, Vector3(2.0f));
        Transform::SetPosition(child, Vector3(1.0f, 0.0f, 0.0f));

        result.matrix = Transform::ComposeTransforms(parent.matrix, child.matrix);
        AssertVector(result.GetPosition(), Vector3(10.0f, 2.0f, 0.0f));
    }

    void TestRotationExtractionHandlesNegativeScale() {
        Transform transform;
        const Quaternion expected(0.0f, -1.0f, 0.0f, 0.0f);
        Transform::SetRotation(transform, expected);
        Transform::SetScale(transform, Vector3(-0.0788578f, -0.0788578f, -0.0628706f));

        const Quaternion actual = transform.GetRotation();
        // q and -q describe the same rotation, so compare their absolute dot.
        const float dot = actual.x * expected.x + actual.y * expected.y
            + actual.z * expected.z + actual.w * expected.w;
        assert(Near(std::abs(dot), 1.0f));
    }

    void TestSerializationPreservesTheExactMatrix() {
        Transform source;
        source.matrix(0, 1) = 0.25f; // Include shear so a TRS round trip would be lossy.
        source.matrix(2, 3) = 7.0f;

        JsonArchive writer(JsonArchive::Mode::Writing);
        Transform::Serialize(writer, source);
        assert(writer.Dump().contains("matrix"));
        assert(!writer.Dump().contains("position"));

        JsonArchive reader(JsonArchive::Mode::Reading);
        reader.OpenFromMemory(writer.Dump());
        Transform restored;
        Transform::Deserialize(reader, restored);
        for (std::size_t i = 0; i < 16; ++i) {
            assert(Near(source.matrix.Data()[i], restored.matrix.Data()[i]));
        }
    }
}

int main() {
    Logger::Info("TransformTests", "Starting Transform Tests");
    Logger::Info("TransformTests", "Running TestIdentityDefaults");
    TestIdentityDefaults();
    Logger::Info("TransformTests", "Running TestModificationFunctions");
    TestModificationFunctions();
    Logger::Info("TransformTests", "Running TestCompositionAppliesParentRotationAndScale");
    TestCompositionAppliesParentRotationAndScale();
    Logger::Info("TransformTests", "Running TestRotationExtractionHandlesNegativeScale");
    TestRotationExtractionHandlesNegativeScale();
    Logger::Info("TransformTests", "Running TestSerializationPreservesTheExactMatrix");
    TestSerializationPreservesTheExactMatrix();
    Logger::Info("TransformTests", "Finished Transform Tests");
}
