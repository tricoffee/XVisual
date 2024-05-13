#ifndef DeletePtrMacro_H
#define DeletePtrMacro_H

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) \
    if (p != nullptr)  \
    {   delete p; \
        p = nullptr; \
    }
#endif

#endif // DeletePtrMacro_H

