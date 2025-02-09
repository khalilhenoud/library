RANDOM: This feels like I am recreating parts of C++.

Purpose:
--------
- Generic types in c are either implemented via macros or void pointers. I opted 
for the latter approach since I want to avoid any ugly declaration at the 
beginning of the files. 
- This necessitates that the container holds a ref to the type and a mechanism 
for us to retrieve information about the type. 
- This is a stop gap solution to a full on source code parsing and will later be
augmented with source code parsing to generate the serialization functions and 
helper functionalities.
- This ensures that generic containers can work with any type that adheres to 
the registration, and greatly simplifies the serializer package by removing data
duplication and deferring the serialization to the type in question. This makes
deserialization a lot easier and removes code duplication.
- Most importantly this was done for me to learn.

Downsides:
----------
This approach has a couple of major problems:
  - linking optimizations opportunities are greatly reduces. The types cannot be
  deduced at runtime, which means that function calls cannot be statically 
  linked (you lose a lot of speed because of that). A macro approach would have
  been better for this, but I did not care for it.
  - void pointers do not provide any type safety whatsoever. You need to know
  what you are looking at, and macro code (required in some aspects) is terrible
  to debug.
I am looking for a solution to both these problems (generating offline data
structs that are typed and easily included), but this won't be for a while. An
alternative would be to shift the project or part of it to C++ and use eastl to
provide these functionalities.

NOTES:
------
- It is fine to take the address of an inline function, the standard indicates 
that the compiler will generate another copy of the function. This means that
when the function is called from the same translation unit (in a non vtable 
context) the call is inlined, otherwise in a vtable context, it is a dynamic
call.

type interface:
---------------
#define uint32_t type_id;

*_def           default initialize an instance of the type, create an 
                initialized memory image.
  declaration:
    void  $type$_def(void *ptr);

*_is_def        returns 1 if the type instance is equal to the default or 0
  declaration:
    uint32_t  $type$_is_def(const void *ptr);

*_replicate     copy, shallow vs deep depends on the provided function
  declaration:
    void  $type$_replicate(
      const void *src, void *dst, const allocator_t* allocator);

*_fullswap      move/swap. usefull to transfer resources
  declaration:
    void  $type$_fullswap(void* lhs, void* rhs);

*_serialize     write the content to a byte stream. never serialize the
                allocator, even if the type owns it.
  declaration:
    void  $type$_serialize(const void *src, binary_stream* stream);

*_deserialize   read the content from a byte stream.
  declaration:
    void  $type$_deserialize(
      void *dst, const allocator_t *allocator, binary_stream* stream);

*_hash          used for hash maps (containers are valid in container container
                scenarios)
  declaration:
    uint32_t  $type$_hash(const void *ptr);

*_is_equal      if the type is used for hashing, this becomes necessary in case
                of collision (equivalent to == operator). 1 if same, 0 otherwise
  declaration:
    uint32_t $type$_is_equal(const void *lhs, const void *rhs);

*_size          used to determine the size, so that allocation could happen.
                note that this is the size of the struct itself and not its 
                contained data, same as sizeof(type);
  declaration:
    size_t  $type$_type_size(void);

*_alignment     used to determine the alignment of the type, also for allocation
                purposes. would be equivalent in C++ to alignof(type). some
                restrictions between alignment and types can be enforced.
  declaration:
    size_t  $type$_alignment(void);

*_type_id_count returns the number of inner types the container depend on. this 
                is valid when you have a  $type$ or uint32_t, the _type_id_count
                would be 1 in this case (referring to the uin32_t). this is used
                to support container types. a hashmap would return 2, one for
                the key type and another one for the value type.
  declaration:
    uint32_t  $type$_type_id_count(void);

*_type_ids      returns the list of type_ids the container depend on. the 
                function takes an array of size corresponding to _type_id_count.
                the caller is responsible for providing the correct sized array.
  declaration:
    void  $type$_type_ids(const void *src, type_id *ids);

*_owns_alloc    returns 1 if it contains a reference to the allocator it uses
                internally or 0 otherwise
  declaration:
    uint32_t  $type$_owns_alloc(void);

*_get_alloc     returns the allocator or NULL. _owns_alloc should return 1
  declaration:
    const allocator_t* $type$_get_alloc(const void *ptr);

*_cleanup       destructor equivalent, cleans up the instance, this does not 
                free up the memory associated with the instance itself, simply
                frees the insides of the type. Only one cleanup function can be
                registered by type.
  declaration:
    void  $type$_cleanup(void *ptr, const allocator_t* allocator);

best practices:
---------------
1- never serialize the allocator reference even if the type holds its own 
reference.
2- the allocator passed to deserialize should be held if the type dictates as 
much.
3- do not serialize the type hash in the type's own serialize function, that is 
the responsability of the owner type.
4- when implementing _is_equal, do not compare held allocators.
5- remember that cleanup is for the element of the struct and not the struct 
itself, that is the responsability of the owner.

type registry:
--------------
This feature makes it necessary for this project to no longer be header only. I
do not want to rely on the static variables in inline functions. thus this will
no longer be a header only module.
As a side effect a internal module.h definition must be created to handle 
__declspec(dll_export) and __declspec(dll_import), as well as change the project
to a library (see type_registry.c).

Template:
---------
#ifndef GUARD_H
#define GUARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "whatever_is_required.h"
#include "whatever_is_required2.h"


////////////////////////////////////////////////////////////////////////////////
//| $type$_t, * = $type$
//|=============================================================================
//| OPERATION                   | SUPPORTED
//|=============================================================================
//|    *_def                    | YES
//|    *_is_def                 | 
//|    *_replicate              |
//|    *_fullswap               |
//|    *_serialize              |
//|    *_deserialize            |
//|    *_hash                   |
//|    *_is_equal               |
//|    *_type_size              |
//|    *_type_alignment         |
//|    *_type_id_count          |
//|    *_type_ids               |
//|    *_owns_alloc             |
//|    *_get_alloc              |
//|    *_cleanup                |
////////////////////////////////////////////////////////////////////////////////
// NOTES/TODO/REMARK:
////////////////////////////////////////////////////////////////////////////////
type defintion of $type$_t;

void $type$_def(void *ptr);
uint32_t $type$_is_def(const void *ptr);
void $type$_replicate(const void *src, void *dst, const allocator_t* allocator);
void $type$_fullswap(void* lhs, void* rhs);
void $type$_serialize(const void *src, binary_stream_t* stream);
void $type$_deserialize(
  void *dst, const allocator_t *allocator, binary_stream_t* stream);
uint32_t $type$_hash(const void *ptr);
uint32_t $type$_is_equal(const void *lhs, const void *rhs);
size_t $type$_type_size(void);
size_t $type$_type_alignment(void);
uint32_t $type$_type_id_count(void);
void $type$_type_ids(const void *src, type_id_t *ids);
uint32_t $type$_owns_alloc(void);
const allocator_t* $type$_get_alloc(const void *ptr);
void $type$_cleanup(void *ptr, const allocator_t* allocator);

////////////////////////////////////////////////////////////////////////////////
the rest of the functions

////////////////////////////////////////////////////////////////////////////////
macro functions

#include "$type$.impl"

#endif

#endif // GUARD_H

TODO:
-----
- A well defined iterator interface can make implementing an algorithms library
container agnostic. Doing so might require us to require some classification 
functions (less or greater functions) per type that require sorting.
- Expand the library considerably, I want to support many more stuff.

MISC:
-----
- 


NOTE: type interface functions are optional in a lot of cases. for example if
the replicate function is missing, the elements of such types are replicated
using memcpy, likewise if setup is missing, the memory is memset to 0.

NOTE: setup functions do not allocate the memory for the instance in question.
they are responsible for the instance internal initialization (whether that
contains memory allocation or not is of no relevance). the logic for cleanup is
similar.

IMPORTANT: 
I suspect we will need *_allocate *_deallocate variants to support deserializing
in an agnostic way (the guid is mapped to _allocate which can be used to 
instantiate an object).

NOTE: the reason why I went with non macro instantiations was compatibility with
C++. But does this hold, maybe I should make a test and see?

