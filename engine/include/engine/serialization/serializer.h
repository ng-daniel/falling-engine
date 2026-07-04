#ifndef ENGINE_SERIALIZATION_SERIALIZER_H
#define ENGINE_SERIALIZATION_SERIALIZER_H

template<typename T>
class Serializer
{
public:
    virtual ~Serializer() = default;
    virtual void Read(Archive& archive, T& object) = 0;
    virtual void Write(Archive& archive, const T& object) = 0;
};

#endif // ENGINE_SERIALIZATION_SERIALIZER_H