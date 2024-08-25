*_def           returns a default initialized instance
*_is_def        returns true if it is equalt to the def instance
*_setup         constructor 
*_cleanup       destructor
*_replicate     copy
*_fullswap      move
*_allocate      
*_free

Note on setup/cleanup:
the memory for the instance is already available, this is about setting up or
cleaning up the internals of the structs. To allocate an instance of the struct
use allocate/free.