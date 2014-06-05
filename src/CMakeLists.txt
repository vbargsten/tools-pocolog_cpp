rock_library(pocolog_cpp 
    SOURCES 
        Format.cpp 
        Read.cpp 
        Write.cpp 
        Index.cpp 
        InputDataStream.cpp
#         OutputDataStream.cpp
        StreamDescription.cpp 
        Stream.cpp LogFile.cpp IndexFile.cpp FileStream.cpp
    HEADERS 
        Format.hpp 
        Read.hpp 
        Write.hpp 
        Index.hpp 
        DataStream.hpp
    DEPS_PKGCONFIG base-types typelib)

rock_executable(indexer
    SOURCES indexer.cpp
    DEPS pocolog_cpp
    DEPS_PKGCONFIG base-types typelib
)

rock_executable(example_old
    SOURCES example_old.cpp
    DEPS pocolog_cpp
    DEPS_PKGCONFIG base-types typelib
)
