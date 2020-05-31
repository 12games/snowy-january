#ifndef CLAMPEDVALUE_H
#define CLAMPEDVALUE_H

template<type T>
class ClampedValue
{
    T _value;
    T _min;
    T _max;
    
    void clamp()
    {
        if (_value < _min) _value = _min;
        if (_value > _max) _value = _max;
    }

public:
    ClampedValue(
        T value,
        T min,
        T max)
        : _value(value)
          _min(min),
          _max(max)
    {
        if (_min > _max)
        {
            auto tmp = _min;
            _min = _max;
            _max = tmp;
        }
        clamp();
    }
    virtual ~ClampedValue() = default;
};

#endif // CLAMPEDVALUE_H
