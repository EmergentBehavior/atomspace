
OPENCOG_ADD_ATOM_TYPES(atom_types.script 
  atom_types.h
  atom_types.definitions
  atom_types.inheritance
  core_types.scm
  core_types.pyx)

ADD_CUSTOM_TARGET(opencog_atom_types DEPENDS atom_types.h
	atom_types.definitions atom_types.inheritance core_types.scm
	core_types.pyx)

# The atom_types.h file is written to the build directory
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

ADD_LIBRARY (value
	atom_types_init.cc
	FloatValue.cc
	LinkValue.cc
	NameServer.cc
	RandomStream.cc
	StreamValue.cc
	StringValue.cc
	ValueFactory.cc
)

# Without this, parallel make will race and crap up the generated files.
ADD_DEPENDENCIES(value opencog_atom_types)

TARGET_LINK_LIBRARIES(value
	${COGUTIL_LIBRARY}
)

INSTALL (TARGETS value EXPORT AtomSpaceTargets
	DESTINATION "lib${LIB_DIR_SUFFIX}/opencog"
)

INSTALL (FILES
	${CMAKE_CURRENT_BINARY_DIR}/atom_types.h
	atom_types.cc
	FloatValue.h
	LinkValue.h
	NameServer.h
	Value.h
	RandomStream.h
	StreamValue.h
	StringValue.h
	ValueFactory.h
	types.h
	DESTINATION "include/opencog/atoms/value"
)

# Install the auto-generated atom types as well
INSTALL (FILES
	${CMAKE_CURRENT_BINARY_DIR}/core_types.scm
	DESTINATION "${DATADIR}/scm/opencog/base"
)
