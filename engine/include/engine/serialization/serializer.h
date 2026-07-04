template<typename T>
class Serializer
{
public:
    virtual ~Serializer() = default;
    virtual void Serialize(Archive& archive, T& object) = 0;
};