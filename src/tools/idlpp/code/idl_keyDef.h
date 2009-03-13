#ifndef IDL_KEYDEF_H
#define IDL_KEYDEF_H

#include <c_metabase.h>
#include <idl_scope.h>

C_CLASS(idl_keyDef);

idl_keyDef idl_keyDefNew (void);

void idl_keyDefFree (idl_keyDef keyDef);

void idl_keyDefAdd (idl_keyDef keyDef, c_metaObject scope, const char *typeName, const char *keyList);

c_char *idl_keyResolve (idl_keyDef keyDef, idl_scope scope, const char *typeName);

void idl_keyDefDefSet (idl_keyDef keyDef);

idl_keyDef idl_keyDefDefGet (void);

#endif /* IDL_KEYDEF_H */