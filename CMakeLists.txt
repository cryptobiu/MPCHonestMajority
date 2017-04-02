cmake_minimum_required(VERSION 2.8)
project(MPCHonestMajority)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -maes -mpclmul" )

SET (BOOST_ROOT $ENV{HOME}/boost_1_60_0/)
SET (BOOST_INCLUDEDIR $ENV{HOME}/boost_1_60_0/)
SET (BOOST_LIBRARYDIR $ENV{HOME}/boost_1_60_0/stage/lib)
find_package(Boost COMPONENTS system thread REQUIRED)

INCLUDE_DIRECTORIES($ENV{HOME}/ntl-9.10.0/include $ENV{HOME} ${BOOST_INCLUDEDIR}
        $ENV{HOME}/libscapi/lib/OTExtensionBristol $ENV{HOME}/libscapi/lib/)

set(SOURCE_FILES AES_PRG.cpp HIM.cpp main.cpp VDM.cpp MPCCommunication.cpp MPCCommunication.h
        TGate.h ArithmeticCircuit.cpp Protocol.cpp Protocol.h TemplateField.h TemplateField.cpp ProtocolTimer.cpp ProtocolTimer.h 			ZpMersenneIntElement.cpp ZpMersenneIntElement.h)


link_directories(/usr/ssl/lib/ $ENV{HOME}/libscapi/install/lib ${BOOST_LIBRARYDIR})


add_executable(MPCHonestMajority ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(MPCHonestMajority $ENV{HOME}/ntl-9.10.0/src/ntl.a
        $ENV{HOME}/libscapi/scapi.a gmp
        OTExtensionBristol
        $ENV{HOME}/libscapi/install/lib/libsimpleot.a
        boost_system boost_thread pthread crypto dl ssl z)
