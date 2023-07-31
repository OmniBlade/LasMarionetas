# Builds version found in TS2 for binary matching aspirations.
add_library(jpeg STATIC)
add_library(JPEG::JPEG ALIAS jpeg)

configure_file(jconfig.wat ${CMAKE_CURRENT_BINARY_DIR}/jconfig.h COPYONLY)

target_sources(jpeg PRIVATE
    jcapimin.c
    jcapistd.c
    jccoefct.c
    jccolor.c
    jcdctmgr.c
    jchuff.c
    jcinit.c
    jcmainct.c
    jcmarker.c
    jcmaster.c
    jcomapi.c
    jcparam.c
    jcphuff.c
    jcprepct.c
    jcsample.c
    jctrans.c
    jdapimin.c
    jdapistd.c
    jdatadst.c
    jdatasrc.c
    jdcoefct.c
    jdcolor.c
    jddctmgr.c
    jdhuff.c
    jdinput.c
    jdmainct.c
    jdmarker.c
    jdmaster.c
    jdmerge.c
    jdphuff.c
    jdpostct.c
    jdsample.c
    jdtrans.c
    jerror.c
    jfdctflt.c
    jfdctfst.c
    jfdctint.c
    jidctflt.c
    jidctfst.c
    jidctint.c
    jidctred.c
    jmemansi.c
    jmemmgr.c
    jquant1.c
    jquant2.c
    jutils.c
)

target_include_directories(jpeg PUBLIC . ${CMAKE_CURRENT_BINARY_DIR})
