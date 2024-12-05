WIP; this feels like we are recreating C++.

IMPORTANT: all these functions right now, like cvector_setup() and the likes 
take a cvector_t* pointer or a clist_t* pointer, for them to be generic and used
in a meta way, they should take void*, and cast to the appropriate type inside 
the function.

container interface:
*_def           returns a default initialized instance of the container
*_is_def        returns true if the container instance is equal to the default
*_setup         constructor
*_cleanup       destructor
*_replicate     copy, shallow vs deep depends on the provided function
*_fullswap      move/swap. usefull to transfer resources
*_serialize     write the content to a byte stream
*_deserialize   read the content from a byte stream
*_hash          used for hash maps (containers are valid in container container
                scenarios)

to be finalized:
*_guid          unique guid associated with the container
*_size          size of the data struct itself
*_inner_guid    [meta type only] unique guid assocaited with the type the 
                container holds, i.e: a vector of int32_t or the likes.

NOTE: a well defined iterator interface will allow us to work with algorithms in
an abstract manner, without consideration for the container underneath.

iterator interface:
{containertype}_iterator
*_begin
*_end
*_advance
*_deref

type interface:
*_def           returns/sets a default initialized instance of the type
*_is_def        returns whether the instance is equivalent to the default type
*_setup         constructor
*_cleanup       destructor
*_replicate     copy, controls how the type does copy; i.e: deep vs shallow    
*_fullswap      move/swap. 
*_serialize     write the content to a byte stream
*_deserialize   read the content from a byte stream
*_hash          used, when it exists, for hashmaps and the likes

to be finalized:
*_guid          unique guid associated with the container
*_size          size of the data struct itself
*_inner_guid    [meta type only] : unique guid assocaited with the type the 
                structs holds. like a unique ptr to int32_t

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
