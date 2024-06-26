#ifndef XMACRO_H
#define XMACRO_H

#define REGISTER_MEMBER(instance, name, value) \
    instance.registerMember(#name, value)

#define REGISTER_MEMBER_STR(instance, name, value) \
    instance.registerMember(name, value)

#define REGISTER_MEMBER_ATTR(instance, name, value, isFromOutSide) \
    instance.registerMember(#name, value, isFromOutSide)

#define REGISTER_MEMBER_ATTR_STR(instance, name, value, isFromOutSide) \
    instance.registerMember(name, value, isFromOutSide)

#define REGISTER_TYPE(instance, name, value) \
    instance.registerType(#name, value)

#define REGISTER_TYPE_STR(instance, name, value) \
    instance.registerType(name, value)

//#define REGISTER_MEMBER_WITH_FLAG(instance, name, value, b) \
//    instance.registerMember(#name, value, b)
//
//#define REGISTER_MEMBER_WITH_FLAG_STR(instance, name, value, b) \
//    instance.registerMember(name, value, b)

#define GET_MEMBER_WITH_TYPE(instance, type, name) \
    instance.getMemberWithT<type>(#name)

#define GET_MEMBER_WITH_TYPE_STR(instance, type, name) \
    instance.getMemberWithT<type>(name)

#define GET_MEMBER(instance, name) \
    instance.getMember(#name)

#define GET_MEMBER_STR(instance, name) \
    instance.getMember(name)

#define GET_MEMBER_TYPE(instance, name) \
    instance.getMemberType(#name)

#define GET_MEMBER_TYPE_STR(instance, name) \
    instance.getMemberType(name)

#define SET_SOURCEFROM(instance, name, sourceFrom) \
    instance.setSourceFrom(#name, sourceFrom)

#define SET_SOURCEFROM_STR(instance, name, sourceFrom) \
    instance.setSourceFrom(name, sourceFrom)

#define GET_SOURCEFROM(instance, name) \
    instance.getSourceFrom(#name)

#define GET_SOURCEFROM_STR(instance, name) \
    instance.getSourceFrom(name)

#define IS_MEMBER_FROM_OUTSIDE(instance, name) \
    instance.isFromOutside(#name)

#define IS_MEMBER_FROM_OUTSIDE_STR(instance, name) \
    instance.isFromOutside(name)

#define ACQUIRE_NAMES(instance) \
    instance.acquireNames()

#define IS_TYPE(obj, name, type) (*GET_MEMBER_TYPE(obj, name) == typeid(type))

// convert variable name to string
#define VNAME(variable) #variable

#define InspectType(obj, name, type){ \
    if (IS_TYPE(obj, name, type)) \
    { \
          type; \
    }

#endif //XMACRO_H

