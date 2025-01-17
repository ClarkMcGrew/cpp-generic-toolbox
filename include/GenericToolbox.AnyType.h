//
// Created by Nadrino on 27/07/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ANYTYPE_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ANYTYPE_H


#include "GenericToolbox.Wrappers.h"

#include <utility>
#include "memory"
#include "iostream"
#include "any"
#include "cmath"

namespace GenericToolbox{

  // Structs to decide if a stream function can be implemented
  template<typename S, typename T, typename = void> struct is_streamable : std::false_type {};
  template<typename S, typename T> struct is_streamable<S, T, decltype(std::declval<S&>() << std::declval<T&>(), void())> : std::true_type {};

  // Required by C++11 and C++14
  template <typename T, bool> class StreamerImpl {};
  template <typename T> class StreamerImpl<T,true> { public: static void implement(std::ostream& os, T const& t) { os << t; } };
  template <typename T> class StreamerImpl<T,false>{ public: static void implement(std::ostream& os, T const& t) { os << typeid(t).name(); } };
  template <typename T, bool> class DoubleCastImpl {};
  template <typename T> class DoubleCastImpl<T,true> { public: static double implement(T const& t) { return static_cast<double>(t); } };
  template <typename T> class DoubleCastImpl<T,false>{ public: static double implement(T const& t) { return std::nan(typeid(t).name()); } };

  // PlaceHolder is used in AnyType as a pointer member
  struct PlaceHolder{
    virtual ~PlaceHolder() = default;
    virtual const std::type_info& getType() const = 0;
    virtual PlaceHolder* clone() const = 0;
    virtual void writeToStream(std::ostream& o) const = 0;
    virtual double getVariableAsDouble() const = 0;
    virtual size_t getVariableSize() const = 0;
    virtual const void* getVariableAddress() const = 0;
    virtual void* getVariableAddress() = 0;
  };

  // VariableHolder is the specialized PlaceHolder containing the _variable_ of interest
  template<typename VariableType> struct VariableHolder: public PlaceHolder{
    explicit VariableHolder(VariableType value_) : _nBytes_(sizeof(value_)), _variable_(std::move(value_)){  }
    const std::type_info & getType() const override { return typeid(VariableType); }
    PlaceHolder* clone() const override { return new VariableHolder(_variable_); }
    void writeToStream(std::ostream& o) const override {
      // For C++17:
//      if constexpr (is_streamable<std::ostream,VariableType>::value) {
//        o << _variable_;
//      }
      StreamerImpl<VariableType, is_streamable<std::ostream,VariableType>::value>::implement(o, _variable_);
    }
    double getVariableAsDouble() const override {
      return DoubleCastImpl<VariableType, std::is_convertible<VariableType, double>::value>::implement(_variable_);
    }
    virtual size_t getVariableSize() const override{
      return _nBytes_;
    }
    virtual const void* getVariableAddress() const override{
      return static_cast<const void*>(&_variable_);
    }
    virtual void* getVariableAddress() override{
      return static_cast<void*>(&_variable_);
    }

    size_t _nBytes_;
    VariableType _variable_;
  };


  // The Class:
  class AnyType{

  public:

    inline AnyType();
    inline AnyType(const AnyType& other_);
    template<typename ValueType> inline explicit AnyType(const ValueType& value_);
    inline virtual ~AnyType();

    // Operators
    template<typename ValueType> inline AnyType& operator=(const ValueType & rhs);
    inline AnyType& operator=(const AnyType& rhs);

    inline void reset();
    inline bool empty();
    inline const std::type_info& getType() const;
    const PlaceHolder* getPlaceHolderPtr() const;
    PlaceHolder* getPlaceHolderPtr();
    inline size_t getStoredSize() const;

    template<typename ValueType> inline void setValue(const ValueType& value_);
    template<typename ValueType> inline ValueType& getValue();
    template<typename ValueType> inline const ValueType& getValue() const;
    double getValueAsDouble() const;

    inline friend std::ostream& operator <<( std::ostream& o, const AnyType& v );


  protected:
    inline AnyType& swap(AnyType& rhs);


  private:
    std::unique_ptr<PlaceHolder> _varPtr_;

  };

}

#include "implementation/GenericToolbox.AnyType.Impl.h"

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_ANYTYPE_H
