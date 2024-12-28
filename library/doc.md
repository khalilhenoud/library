WIP; This feels like I am recreating C++.

IMPORTANT: 
----------
All these functions right now, like cvector_setup() and the likes 
take a cvector_t* pointer or a clist_t* pointer, for them to be generic and used
in a meta way, they should take void*, and cast to the appropriate type inside 
the function. Or at the least have a wrapper inlined function that calls it.

type interface:
---------------
#define uint32_t type_id;

*_def           default initialize an instance of the type, create an 
                initialized memory image.
  declaration:
    void  $type$_def(void *ptr);

*_is_def        returns 1 if the type instance is equal to the default or 0
  declaration:
    int32_t  $type$_is_def(const void *ptr);

*_replicate     copy, shallow vs deep depends on the provided function
  declaration:
    void  $type$_replicate(
      const void *src, void *dst, const allocator_t* allocator);

*_fullswap      move/swap. usefull to transfer resources
  declaration:
    void  $type$_fullswap(void* lhs, void* rhs);

*_serialize     write the content to a byte stream
  declaration:
    void  $type$_serialize(const void *src, binary_stream* stream);

*_deserialize   read the content from a byte stream
  declaration:
    void  $type$_deserialize(void *dst, binary_stream* stream);

*_hash          used for hash maps (containers are valid in container container
                scenarios)
  declaration:
    uint64_t  $type$_hash(const void *ptr);

*_is_equal      if the type is used for hashing, this becomes necessary in case
                of collision (equivalent to == operator). 1 if same, 0 otherwise
  declaration:
    uint32_t $type$_is_equal(const void *lhs, const void *rhs);

*_size          used to determine the size, so that allocation could happen.
                note that this is the size of the struct itself and not its 
                contained data, same as sizeof(type);
  declaration:
    size_t  $type$_size(const void *ptr);

*_alignment     used to determine the alignment of the type, also for allocation
                purposes. would be equivalent in C++ to alignof(type). some
                restrictions between alignment and types can be enforced.
  declaration:
    size_t  $type$_alignment(const void *ptr);

*_type_id_count returns the number of inner types the container depend on. this 
                is valid when you have a  $type$ or uint32_t, the _type_id_count
                would be 1 in this case (referring to the uin32_t). this is used
                to support container types. a hashmap would return 2, one for
                the key type and another one for the value type.
  declaration:
    uint32_t  $type$_type_id_count(const void *ptr);

*_type_ids      returns the list of type_ids the container depend on. the 
                function takes an array of size corresponding to _type_id_count.
                the caller is responsible for providing the correct sized array.
  declaration:
    void  $type$_type_ids(const void *ptr, type_id *ids);

*_owns_alloc    returns 1 if it contains a reference to the allocator it uses
                internally or 0 otherwise
  declaration:
    uint32_t  $type$_owns_alloc(const void* ptr);

*_cleanup       destructor equivalent, cleans up the instance, this does not 
                free up the memory associated with the instance itself, simply
                frees the insides of the type. Only one cleanup function can be
                registered by type.
  declaration:
    void  $type$_cleanup(void *ptr);
    void  $type$_cleanup(void *ptr, const allocator_t* allocator);

type registry:
--------------
This feature makes it necessary for this project to no longer be header only. I
do not want to rely on the static variables in inline functions. thus this will
no longer be a header only module.
As a side effect a internal module.h definition must be created to handle 
__declspec(dll_export) and __declspec(dll_import), as well as change the project
to a library. 

// 4k buffer defined in a source file.
static uint8_t buffer[4 * 1024];
static uint32_t registered = 0;

typedef uint32_t type_id;

typedef
struct vtable_t {
  function_ptrs...;
} vtable_t;

// use fnv to convert #type to a uint32_t hash.
#define get_type_id(type) \
  ...

// register the vtable associated with the type.
LIBRARY_API
void register_type(const type_id type, const vtable_t *ptr);

LIBRARY_API
void associate_alias(const type_id type, const type_id alias);

// sets ptr to the vtable associated with the type.
LIBRARY_API
void get_vtable(const type_id type, vtable_t *ptr);

--------------------------------------------------------------------------------
NOTE: a well defined iterator interface will allow us to work with algorithms in
an abstract manner, without consideration for the container underneath.

iterator interface:
{containertype}_iterator
*_begin
*_end
*_advance
*_deref

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

// NOTE: this function would have custom arguments specific to the type, I do 
// not think it is helpful to the type interface system for serialization 
// purposes. use def instead.
*_setup         constructor, sets up a preallocated instance of the type